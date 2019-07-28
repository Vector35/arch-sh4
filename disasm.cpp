/* no binja includes! keep separate to ease compile/link into tester */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "disasm.h"

// get 8-bit signed int
static int int8(uint16_t i)
{
	uint32_t result = i;
	if(i & 0x80)
		result = -(128 - (i&0x7F));
	return result;
}

// get 12-bit signed int
static int int12(uint16_t i)
{
	int result = i;
	if(i & 0x800)
		result = -(2048 - (i&0x7FF));
	return result;
}

// displacement to effective address
static uint64_t displ2ea(unsigned int scale, int disp, uint32_t base)
{
	int dest = (base+4) + scale*disp;

	if(dest < 0) {
		dest = -dest;
		dest = dest ^ 0xFFFFFFFFFFFFFFFF;
		dest = dest + 1;
	}

	return dest;
}

int disasm(uint32_t addr, uint16_t insword, struct disasm_result *result)
{
	int rc = 0;
	char buff[128];

	const char *cr2str[5] = {"sr","gbr","vbr","ssr","spc"};
	enum SH4_REGISTER cr2id[5] = {SR,GBR,VBR,SSR,SPC};

	// 0111nnnniiiiiiii "add #imm,Rn"
	if((insword & 0xf000) == 0x7000) {
		int16_t i = int8(insword & 0xff);
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_ADD;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = i;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "add #%d,r%d", i, n);
	}

	// 0011nnnnmmmm1100 "add Rm,Rn"
	else if((insword & 0xf00f) == 0x300c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_ADD;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "add r%d,r%d", m, n);
	}

	// 0011nnnnmmmm1110 "addc Rm,Rn"
	else if((insword & 0xf00f) == 0x300e) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_ADDC;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "addc r%d,r%d", m, n);
	}

	// 0011nnnnmmmm1111 "addv Rm,Rn"
	else if((insword & 0xf00f) == 0x300f) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_ADDV;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "addv r%d,r%d", m, n);
	}

	// 11001001iiiiiiii "and #imm,r0"
	else if((insword & 0xff00) == 0xc900) {
		int16_t i = int8(insword & 0xff);
		result->opcode = OPC_AND;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = i;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		sprintf(result->string, "and #%d,r0", i);
	}

	// 0010nnnnmmmm1001 "and Rm,Rn"
	else if((insword & 0xf00f) == 0x2009) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_AND;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "and r%d,r%d", m, n);
	}

	// 11001101iiiiiiii "and.b #imm,@(r0,gbr)"
	else if((insword & 0xff00) == 0xcd00) {
		int16_t i = int8(insword & 0xff);
		result->opcode = OPC_AND;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = i;
		result->operands[1].type = DEREF_REG_REG;
		result->operands[1].regA = (SH4_REGISTER)(GBR);
		result->operands[1].regB = R0;
		sprintf(result->string, "and.b #%d,@(r0,gbr)", i);
	}

	// 10001011dddddddd "bf label"
	else if((insword & 0xff00) == 0x8b00) {
		uint64_t d = displ2ea(2, int8(insword & 0xff), addr);
		result->opcode = OPC_BF;
		result->operands[0].type = ADDRESS;
		result->operands[0].immediate = d;
		sprintf(result->string, "bf 0x%016llx", d);
	}

	// 10001111dddddddd "bf.s label"
	else if((insword & 0xff00) == 0x8f00) {
		uint64_t d = displ2ea(2, int8(insword & 0xff), addr);
		result->opcode = OPC_BF;
		result->delay_slot = true;
		result->operands[0].type = ADDRESS;
		result->operands[0].immediate = d;
		sprintf(result->string, "bf.s 0x%016llx", d);
	}

	// 1010dddddddddddd "bra label"
	else if((insword & 0xf000) == 0xa000) {
		uint64_t d = displ2ea(2, int12(insword & 0xfff), addr);
		result->opcode = OPC_BRA;
		result->operands[0].type = ADDRESS;
		result->operands[0].immediate = d;
		sprintf(result->string, "bra 0x%016llx", d);
	}

	// 0000mmmm00100011 "braf Rm"
	else if((insword & 0xf0ff) == 0x23) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_BRAF;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		sprintf(result->string, "braf r%d", m);
	}

	// 1011dddddddddddd "bsr label"
	else if((insword & 0xf000) == 0xb000) {
		uint64_t d = displ2ea(2, int12(insword & 0xfff), addr);
		result->opcode = OPC_BSR;
		result->operands[0].type = ADDRESS;
		result->operands[0].immediate = d;
		sprintf(result->string, "bsr 0x%016llx", d);
	}

	// 0000mmmm00000011 "bsrf Rm"
	else if((insword & 0xf0ff) == 0x3) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_BSRF;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		sprintf(result->string, "bsrf r%d", m);
	}

	// 10001001dddddddd "bt label"
	else if((insword & 0xff00) == 0x8900) {
		uint64_t d = displ2ea(2, int8(insword & 0xff), addr);
		result->opcode = OPC_BT;
		result->operands[0].type = ADDRESS;
		result->operands[0].immediate = d;
		sprintf(result->string, "bt 0x%016llx", d);
	}

	// 10001101dddddddd "bt.s label"
	else if((insword & 0xff00) == 0x8d00) {
		uint64_t d = displ2ea(2, int8(insword & 0xff), addr);
		result->opcode = OPC_BT;
		result->delay_slot = true;
		result->operands[0].type = ADDRESS;
		result->operands[0].immediate = d;
		sprintf(result->string, "bt.s 0x%016llx", d);
	}

	// 0000000000101000 "clrmac"
	else if(insword == 0x28) {
		result->opcode = OPC_CLRMAC;
		sprintf(result->string, "clrmac");
	}

	// 0000000001001000 "clrs"
	else if(insword == 0x48) {
		result->opcode = OPC_CLRS;
		sprintf(result->string, "clrs");
	}

	// 0000000000001000 "clrt"
	else if(insword == 0x8) {
		result->opcode = OPC_CLRT;
		sprintf(result->string, "clrt");
	}

	// 10001000iiiiiiii "cmp/eq #imm,r0"
	else if((insword & 0xff00) == 0x8800) {
		int16_t i = int8(insword & 0xff);
		result->opcode = OPC_CMP;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = i;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		sprintf(result->string, "cmp/eq #%d,r0", i);
	}

	// 0011nnnnmmmm0000 "cmp/eq Rm,Rn"
	else if((insword & 0xf00f) == 0x3000) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_CMP;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "cmp/eq r%d,r%d", m, n);
	}

	// 0011nnnnmmmm0011 "cmp/ge Rm,Rn"
	else if((insword & 0xf00f) == 0x3003) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_CMP;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "cmp/ge r%d,r%d", m, n);
	}

	// 0011nnnnmmmm0111 "cmp/gt Rm,Rn"
	else if((insword & 0xf00f) == 0x3007) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_CMP;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "cmp/gt r%d,r%d", m, n);
	}

	// 0011nnnnmmmm0110 "cmp/hi Rm,Rn"
	else if((insword & 0xf00f) == 0x3006) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_CMP;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "cmp/hi r%d,r%d", m, n);
	}

	// 0011nnnnmmmm0010 "cmp/hs Rm,Rn"
	else if((insword & 0xf00f) == 0x3002) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_CMP;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "cmp/hs r%d,r%d", m, n);
	}

	// 0100nnnn00010101 "cmp/pl Rn"
	else if((insword & 0xf0ff) == 0x4015) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_CMP;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "cmp/pl r%d", n);
	}

	// 0100nnnn00010001 "cmp/pz Rn"
	else if((insword & 0xf0ff) == 0x4011) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_CMP;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "cmp/pz r%d", n);
	}

	// 0010nnnnmmmm1100 "cmp/str Rm,Rn"
	else if((insword & 0xf00f) == 0x200c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_CMP;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "cmp/str r%d,r%d", m, n);
	}

	// 0010nnnnmmmm0111 "div0s Rm,Rn"
	else if((insword & 0xf00f) == 0x2007) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_DIV0S;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "div0s r%d,r%d", m, n);
	}

	// 0000000000011001 "div0u"
	else if(insword == 0x19) {
		result->opcode = OPC_DIV0U;
		sprintf(result->string, "div0u");
	}

	// 0011nnnnmmmm0100 "div1 Rm,Rn"
	else if((insword & 0xf00f) == 0x3004) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_DIV1;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "div1 r%d,r%d", m, n);
	}

	// 0011nnnnmmmm1101 "dmuls.l Rm,Rn"
	else if((insword & 0xf00f) == 0x300d) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_DMULS;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "dmuls.l r%d,r%d", m, n);
	}

	// 0011nnnnmmmm0101 "dmulu.l Rm,Rn"
	else if((insword & 0xf00f) == 0x3005) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_DMULU;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "dmulu.l r%d,r%d", m, n);
	}

	// 0100nnnn00010000 "dt Rn"
	else if((insword & 0xf0ff) == 0x4010) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_DT;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "dt r%d", n);
	}

	// 0110nnnnmmmm1110 "exts.b Rm,Rn"
	else if((insword & 0xf00f) == 0x600e) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_EXTS;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "exts.b r%d,r%d", m, n);
	}

	// 0110nnnnmmmm1111 "exts.w Rm,Rn"
	else if((insword & 0xf00f) == 0x600f) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_EXTS;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "exts.w r%d,r%d", m, n);
	}

	// 0110nnnnmmmm1100 "extu.b Rm,Rn"
	else if((insword & 0xf00f) == 0x600c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_EXTU;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "extu.b r%d,r%d", m, n);
	}

	// 0110nnnnmmmm1101 "extu.w Rm,Rn"
	else if((insword & 0xf00f) == 0x600d) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_EXTU;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "extu.w r%d,r%d", m, n);
	}

	// 1111nnn001011101 "fabs DRn"
	else if((insword & 0xf1ff) == 0xf05d) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FABS;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fabs fr%d", n);
	}

	// 1111nnnn01011101 "fabs FRn"
	else if((insword & 0xf0ff) == 0xf05d) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FABS;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fabs fr%d", n);
	}

	// 1111nnn0mmmm0000 "fadd DRm,DRn"
	else if((insword & 0xf10f) == 0xf000) {
		uint16_t m = (insword & 0xF0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FADD;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fadd fr%d,fr%d", m, n);
	}

	// 1111nnnnmmmm0000 "fadd FRm,FRn"
	else if((insword & 0xf00f) == 0xf000) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FADD;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fadd fr%d,fr%d", m, n);
	}

	// 1111nnn0mmmm0100 "fcmp/eq DRm,DRn"
	else if((insword & 0xf11f) == 0xf004) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FCMP;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fcmp/eq fr%d,fr%d", m, n);
	}

	// 1111nnnnmmmm0100 "fcmp/eq FRm,FRn"
	else if((insword & 0xf00f) == 0xf004) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FCMP;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fcmp/eq fr%d,fr%d", m, n);
	}

	// 1111nnn0mmm00101 "fcmp/gt DRm,DRn"
	else if((insword & 0xf11f) == 0xf005) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FCMP;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fcmp/gt fr%d,fr%d", m, n);
	}

	// 1111nnnnmmmm0101 "fcmp/gt FRm,FRn"
	else if((insword & 0xf00f) == 0xf005) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FCMP;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fcmp/gt fr%d,fr%d", m, n);
	}

	// 1111mmm010111101 "fcnvds DRm,fpul"
	else if((insword & 0xf1ff) == 0xf0bd) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_FCNVDS;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(DR0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(FPUL);
		sprintf(result->string, "fcnvds dr%d,fpul", m);
	}

	// 1111nnn010101101 "fcnvsd fpul,DRn"
	else if((insword & 0xf1ff) == 0xf0ad) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FCNVSD;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(FPUL);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(DR0 + n);
		sprintf(result->string, "fcnvsd fpul,dr%d", n);
	}

	// 1111nnn0mmmm0011 "fdiv DRm,DRn"
	else if((insword & 0xf10f) == 0xf003) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FDIV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fdiv fr%d,fr%d", m, n);
	}

	// 1111nnnnmmmm0011 "fdiv FRm,FRn"
	else if((insword & 0xf00f) == 0xf003) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FDIV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fdiv fr%d,fr%d", m, n);
	}

	// 1111nnmm11101101 "fipr fvm,fvn"
	else if((insword & 0xf0ff) == 0xf0ed) {
		uint16_t m = (insword & 0x300)>>8;
		uint16_t n = (insword & 0xc00)>>10;
		result->opcode = OPC_FIPR;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FV0 + 4*m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FV0 + 4*n);
		sprintf(result->string, "fipr fv%d,fv%d", 4*m, 4*n);
	}

	// 1111nnnn10001101 "fldi0 FRn"
	else if((insword & 0xf0ff) == 0xf08d) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FLDI0;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fldi0 fr%d", n);
	}

	// 1111nnnn10011101 "fldi1 FRn"
	else if((insword & 0xf0ff) == 0xf09d) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FLDI1;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fldi1 fr%d", n);
	}

	// 1111mmmm00011101 "flds FRm,fpul"
	else if((insword & 0xf0ff) == 0xf01d) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_FLDS;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(FPUL);
		sprintf(result->string, "flds fr%d,fpul", m);
	}

	// 1111nnn000101101 "float fpul,DRn"
	else if((insword & 0xf1ff) == 0xf02d) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FLOAT;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(FPUL);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "float fpul,fr%d", n);
	}

	// 1111nnnn00101101 "float fpul,FRn"
	else if((insword & 0xf0ff) == 0xf02d) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FLOAT;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(FPUL);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "float fpul,fr%d", n);
	}

	// 1111nnnnmmmm1110 "fmac fr0,FRm,FRn"
	else if((insword & 0xf00f) == 0xf00e) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMAC;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[2].type = FPUREG;
		result->operands[2].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmac fr0,fr%d,fr%d", m, n);
	}

	// 1111nnn0mmm01100 "fmov DRm,DRn"
	else if((insword & 0xf11f) == 0xf00c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov fr%d,fr%d", m, n);
	}

	// 1111nnn1mmm01100 "fmov DRm,XDn"
	else if((insword & 0xf11f) == 0xf10c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov fr%d,fr%d", m, n);
	}

	// 1111nnnnmmmm1100 "fmov FRm,FRn"
	else if((insword & 0xf00f) == 0xf00c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov fr%d,fr%d", m, n);
	}

	// 1111nnn0mmm11100 "fmov XDm,DRn"
	else if((insword & 0xf11f) == 0xf01c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(XD0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov XD%d,fr%d", m, n);
	}

	// 1111nnn1mmm11100 "fmov XDm,XDn"
	else if((insword & 0xf11f) == 0xf11c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(XD0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov XD%d,fr%d", m, n);
	}

	// 1111nnn0mmmm0110 "fmov @(r0,Rm),DRn"
	else if((insword & 0xf10f) == 0xf006) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = DEREF_REG_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0);
		result->operands[0].regB = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov @(r0,r%d),fr%d", m, n);
	}

	// 1111nnnnmmmm0110 "fmov @(r0,Rm),XDn"
	else if((insword & 0xf10f) == 0xf106) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = DEREF_REG_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0);
		result->operands[0].regB = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov @(r0,r%d),fr%d", m, n);
	}

	// 1111nnn0mmmm1001 "fmov @Rm+,DRn"
	else if((insword & 0xf10f) == 0xf009) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov @r%d+,fr%d", m, n);
	}

	// 1111nnnnmmmm1001 "fmov @Rm+,XDn"
	else if((insword & 0xf10f) == 0xf109) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov @r%d+,fr%d", m, n);
	}

	// 1111nnn0mmmm1000 "fmov @Rm,DRn"
	else if((insword & 0xf10f) == 0xf008) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov @r%d,fr%d", m, n);
	}

	// 1111nnnnmmmm1000 "fmov @Rm,XDn"
	else if((insword & 0xf10f) == 0xf108) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xF00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov @r%d,fr%d", m, n);
	}

	// 1111nnnnmmm00111 "fmov DRm,@(r0,Rn)"
	else if((insword & 0xf01f) == 0xf007) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = DEREF_REG_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		result->operands[1].regB = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "fmov fr%d,@(r0,r%d)", m, n);
	}

	// 1111nnnnmmm01011 "fmov DRm,@-Rn"
	else if((insword & 0xf01f) == 0xf00b) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "fmov fr%d,@-r%d", m, n);
	}

	// 1111nnnnmmm01010 "fmov DRm,@Rn"
	else if((insword & 0xf01f) == 0xf00a) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "fmov fr%d,@r%d", m, n);
	}

	// 1111nnnnmmmm0111 "fmov XDm,@(r0,Rn)"
	else if((insword & 0xf00f) == 0xf007) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = DEREF_REG_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		result->operands[1].regB = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "fmov fr%d,@(r0,r%d)", m, n);
	}

	// 1111nnnnmmm11011 "fmov XDm,@-Rn"
	else if((insword & 0xf00f) == 0xf00b) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "fmov fr%d,@-r%d", m, n);
	}

	// 1111nnnnmmmm1010 "fmov XDm,@Rn"
	else if((insword & 0xf00f) == 0xf00a) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "fmov fr%d,@r%d", m, n);
	}

	// 1111nnnnmmmm0110 "fmov.s @(r0,Rm),FRn"
	else if((insword & 0xf00f) == 0xf006) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->delay_slot = true;
		result->operands[0].type = DEREF_REG_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0);
		result->operands[0].regB = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov.s @(r0,r%d),fr%d", m, n);
	}

	// 1111nnnnmmmm1001 "fmov.s @Rm+,FRn"
	else if((insword & 0xf00f) == 0xf009) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->delay_slot = true;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov.s @r%d+,fr%d", m, n);
	}

	// 1111nnnnmmmm1000 "fmov.s @Rm,FRn"
	else if((insword & 0xf00f) == 0xf008) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->delay_slot = true;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmov.s @r%d,fr%d", m, n);
	}

	// 1111nnnnmmmm0111 "fmov.s FRm,@(r0,Rn)"
	else if((insword & 0xf00f) == 0xf007) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->delay_slot = true;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = DEREF_REG_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		result->operands[1].regB = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "fmov.s fr%d,@(r0,r%d)", m, n);
	}

	// 1111nnnnmmmm1011 "fmov.s FRm,@-Rn"
	else if((insword & 0xf00f) == 0xf00b) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->delay_slot = true;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "fmov.s fr%d,@-r%d", m, n);
	}

	// 1111nnnnmmmm1010 "fmov.s FRm,@Rn"
	else if((insword & 0xf00f) == 0xf00a) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMOV;
		result->delay_slot = true;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "fmov.s fr%d,@r%d", m, n);
	}

	// 1111nnn0mmmm0010 "fmul DRm,DRn"
	else if((insword & 0xf10f) == 0xf002) {
		uint16_t m = (insword & 0xF0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMUL;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmul fr%d,fr%d", m, n);
	}

	// 1111nnnnmmmm0010 "fmul FRm,FRn"
	else if((insword & 0xf00f) == 0xf002) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FMUL;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fmul fr%d,fr%d", m, n);
	}

	// 1111nnn001001101 "fneg DRn"
	else if((insword & 0xf1ff) == 0xf04d) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FNEG;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fneg fr%d", n);
	}

	// 1111nnnn01001101 "fneg FRn"
	else if((insword & 0xf0ff) == 0xf04d) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FNEG;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fneg fr%d", n);
	}

	// 1111101111111101 "frchg"
	else if(insword == 0xfbfd) {
		result->opcode = OPC_FRCHG;
		sprintf(result->string, "frchg");
	}

	// 1111001111111101 "fschg"
	else if(insword == 0xf3fd) {
		result->opcode = OPC_FSCHG;
		sprintf(result->string, "fschg");
	}

	// 1111nnn001101101 "fsqrt DRn"
	else if((insword & 0xf1ff) == 0xf06d) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FSQRT;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fsqrt fr%d", n);
	}

	// 1111nnnn01101101 "fsqrt FRn"
	else if((insword & 0xf0ff) == 0xf06d) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FSQRT;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fsqrt fr%d", n);
	}

	// 1111nnnn00001101 "fsts fpul,FRn"
	else if((insword & 0xf0ff) == 0xf00d) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FSTS;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(FPUL);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fsts fpul,fr%d", n);
	}

	// 1111nnn0mmmm0001 "fsub DRm,DRn"
	else if((insword & 0xf10f) == 0xf001) {
		uint16_t m = (insword & 0xF0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FSUB;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fsub fr%d,fr%d", m, n);
	}

	// 1111nnnnmmmm0001 "fsub FRm,FRn"
	else if((insword & 0xf00f) == 0xf001) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_FSUB;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		sprintf(result->string, "fsub fr%d,fr%d", m, n);
	}

	// 1111mmm000111101 "ftrc DRm,fpul"
	else if((insword & 0xf1ff) == 0xf03d) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_FTRC;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(FPUL);
		sprintf(result->string, "ftrc fr%d,fpul", m);
	}

	// 1111mmmm00111101 "ftrc FRm,fpul"
	else if((insword & 0xf0ff) == 0xf03d) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_FTRC;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(FPUL);
		sprintf(result->string, "ftrc fr%d,fpul", m);
	}

	// 1111nn0111111101 "ftrv xmtrx,fvn"
	else if((insword & 0xf3ff) == 0xf1fd) {
		uint16_t n = (insword & 0xc00)>>10;
		result->opcode = OPC_FTRV;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(XMTRX);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(FV0 + 4*n);
		sprintf(result->string, "ftrv xmtrx,fv%d", 4*n);
	}

	// 0100mmmm00101011 "jmp @Rm"
	else if((insword & 0xf0ff) == 0x402b) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_JMP;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		sprintf(result->string, "jmp @r%d", m);
	}

	// 0100mmmm00001011 "jsr @Rm"
	else if((insword & 0xf0ff) == 0x400b) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_JSR;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		sprintf(result->string, "jsr @r%d", m);
	}

	// 0100mmmm11110110 "ldc Rm,dbr"
	else if((insword & 0xf0ff) == 0x40f6) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(DBR);
		sprintf(result->string, "ldc.l @r%d+,dbr", m);
	}

	// MANUAL
	else if((insword & 0xf0ff) == 0x40fa) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(DBR);
		sprintf(result->string, "ldc r%d,dbr", m);
	}

	// 0100mmmm00011110 "ldc Rm,gbr"
	else if((insword & 0xf0ff) == 0x401e) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(GBR);
		sprintf(result->string, "ldc r%d,gbr", m);
	}

	// 0100mmmm1nnn1110 "ldc Rm,Rn_bank"
	else if((insword & 0xf08f) == 0x408e) {
		uint16_t m = (insword & 0xf00)>>8;
		uint16_t n = (insword & 0x70)>>4;
		result->opcode = OPC_LDC;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = BANKREG;
		result->operands[1].regA = (SH4_REGISTER)(BANKREG + n);
		sprintf(result->string, "ldc r%d,r%d_bank", m, n);
	}

	// 0100mmmm01001110 "ldc Rm,spc"
	else if((insword & 0xf0ff) == 0x404e) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(SPC);
		sprintf(result->string, "ldc r%d,spc", m);
	}

	// 0100mmmm00001110 "ldc Rm,sr"
	else if((insword & 0xf0ff) == 0x400e) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(SR);
		sprintf(result->string, "ldc r%d,sr", m);
	}

	// 0100mmmm00111110 "ldc Rm,ssr"
	else if((insword & 0xf0ff) == 0x403e) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(SSR);
		sprintf(result->string, "ldc r%d,ssr", m);
	}

	// MANUAL
	// 0100mmmm00111010 "ldc Rm,sgr"
	else if((insword & 0xf0ff) == 0x403A) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(SGR);
		sprintf(result->string, "ldc r%d,sgr", m);
	}

	// 0100mmmm00101110 "ldc Rm,vbr"
	else if((insword & 0xf0ff) == 0x402e) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(VBR);
		sprintf(result->string, "ldc r%d,vbr", m);
	}

	// 0100mmmm11110110 "ldc.l @Rm+,dbr"
	else if((insword & 0xf0ff) == 0x40f6) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(DBR);
		sprintf(result->string, "ldc.l @r%d+,dbr", m);
	}

	// MANUAL
	// 0100mmmmnnnn1110
	else if((insword & 0xf00f) == 0x400E) {
		uint16_t m = (insword & 0xf00)>>8;
		uint16_t n = (insword & 0xf0)>>4;
		if(n < 5) {
			result->opcode = OPC_LDC;
			result->operands[0].type = GPREG;
			result->operands[0].regA = (SH4_REGISTER)(R0 + m);
			result->operands[1].type = CTRLREG;
			result->operands[1].regA = cr2id[n];

			sprintf(result->string, "ldc r%d,%s", m, cr2str[n]);
		}
		else {
			result->opcode = OPC_LDC;
			result->operands[0].type = GPREG;
			result->operands[0].regA = (SH4_REGISTER)(R0 + m);
			result->operands[1].type = BANKREG;
			result->operands[1].regA = (SH4_REGISTER)(BANKREG + n);
			sprintf(result->string, "ldc r%d,r%d_bank", m, n);
		}
	}

	// 0100mmmm00010111 "ldc.l @Rm+,gbr"
	else if((insword & 0xf0ff) == 0x4017) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(GBR);
		sprintf(result->string, "ldc.l @r%d+,gbr", m);
	}

	// 0100mmmm1nnn0111 "ldc.l @Rm+,Rn_bank"
	else if((insword & 0xf08f) == 0x4087) {
		uint16_t m = (insword & 0xf00)>>8;
		uint16_t n = (insword & 0x70)>>4;
		result->opcode = OPC_LDC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = BANKREG;
		result->operands[1].regA = (SH4_REGISTER)(BANKREG + n);
		sprintf(result->string, "ldc.l @r%d+,r%d_bank", m, n);
	}

	// 0100mmmm01000111 "ldc.l @Rm+,spc"
	else if((insword & 0xf0ff) == 0x4047) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(SPC);
		sprintf(result->string, "ldc.l @r%d+,spc", m);
	}

	// 0100mmmm00000111 "ldc.l @Rm+,sr"
	else if((insword & 0xf0ff) == 0x4007) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(SR);
		sprintf(result->string, "ldc.l @r%d+,sr", m);
	}

	// 0100mmmm00110111 "ldc.l @Rm+,ssr"
	else if((insword & 0xf0ff) == 0x4037) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(SSR);
		sprintf(result->string, "ldc.l @r%d+,ssr", m);
	}

	// MANUAL
	// 0100mmmm00110110 "ldc.l @Rm+,sgr"
	else if((insword & 0xf0ff) == 0x4036) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(SGR);
		sprintf(result->string, "ldc.l @r%d+,sgr", m);
	}

	// 0100mmmm00100111 "ldc.l @Rm+,vbr"
	else if((insword & 0xf0ff) == 0x4027) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = CTRLREG;
		result->operands[1].regA = (SH4_REGISTER)(VBR);
		sprintf(result->string, "ldc.l @r%d+,vbr", m);
	}

	// MANUAL
	// 0100mmmmnnnn0111 "ldc.l @Rm+,vbr"
	else if((insword & 0xf00f) == 0x4007) {
		uint16_t m = (insword & 0xf00)>>8;
		uint16_t n = (insword & 0xf0)>>4;
		if(n < 5) {
			result->opcode = OPC_LDC;
			result->length_suffix = LEN_SUFFIX_L;
			result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
			result->operands[0].type = DEREF_REG;
			result->operands[0].regA = (SH4_REGISTER)(R0 + m);
			result->operands[1].type = CTRLREG;
			result->operands[1].regA = cr2id[n];

			sprintf(result->string, "ldc.l @r%d+,%s", m, cr2str[n]);
		}
		else {
			result->opcode = OPC_LDC;
			result->length_suffix = LEN_SUFFIX_L;
			result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
			result->operands[0].type = DEREF_REG;
			result->operands[0].regA = (SH4_REGISTER)(R0 + m);
			result->operands[1].type = BANKREG;
			result->operands[1].regA = (SH4_REGISTER)(BANKREG + n);
			sprintf(result->string, "ldc.l @r%d+,r%d_bank", m, n);
		}
	}

	// 0100mmmm01101010 "lds Rm,fpscr"
	else if((insword & 0xf0ff) == 0x406a) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDS;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(FPSCR);
		sprintf(result->string, "lds r%d,fpscr", m);
	}

	// 0100mmmm01011010 "lds Rm,fpul"
	else if((insword & 0xf0ff) == 0x405a) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDS;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(FPUL);
		sprintf(result->string, "lds r%d,fpul", m);
	}

	// 0100mmmm00001010 "lds Rm,mach"
	else if((insword & 0xf0ff) == 0x400a) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDS;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(MACH);
		sprintf(result->string, "lds r%d,mach", m);
	}

	// 0100mmmm00011010 "lds Rm,macl"
	else if((insword & 0xf0ff) == 0x401a) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDS;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(MACL);
		sprintf(result->string, "lds r%d,macl", m);
	}

	// 0100mmmm00101010 "lds Rm,pr"
	else if((insword & 0xf0ff) == 0x402a) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDS;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(PR);
		sprintf(result->string, "lds r%d,pr", m);
	}

	// 0100mmmm01100110 "lds.l @Rm+,fpscr"
	else if((insword & 0xf0ff) == 0x4066) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDS;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(FPSCR);
		sprintf(result->string, "lds.l @r%d+,fpscr", m);
	}

	// 0100mmmm01010110 "lds.l @Rm+,fpul"
	else if((insword & 0xf0ff) == 0x4056) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDS;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(FPUL);
		sprintf(result->string, "lds.l @r%d+,fpul", m);
	}

	// 0100mmmm00000110 "lds.l @Rm+,mach"
	else if((insword & 0xf0ff) == 0x4006) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDS;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(MACH);
		sprintf(result->string, "lds.l @r%d+,mach", m);
	}

	// 0100mmmm00010110 "lds.l @Rm+,macl"
	else if((insword & 0xf0ff) == 0x4016) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDS;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(MACL);
		sprintf(result->string, "lds.l @r%d+,macl", m);
	}

	// 0100mmmm00100110 "lds.l @Rm+,pr"
	else if((insword & 0xf0ff) == 0x4026) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_LDS;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = SYSREG;
		result->operands[1].regA = (SH4_REGISTER)(PR);
		sprintf(result->string, "lds.l @r%d+,pr", m);
	}

	// 0000000000111000 "ldtlb"
	else if(insword == 0x38) {
		result->opcode = OPC_LDTLB;
		sprintf(result->string, "ldtlb");
	}

	// 0000nnnnmmmm1111 "mac.l @Rm+,@Rn+"
	else if((insword & 0xf00f) == 0xf) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MAC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mac.l @r%d+,@r%d+", m, n);
	}

	// 0100nnnnmmmm1111 "mac.w @Rm+,@Rn+"
	else if((insword & 0xf00f) == 0x400f) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MAC;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mac.w @r%d+,@r%d+", m, n);
	}

	// 1110nnnniiiiiiii "mov #imm,Rn"
	else if((insword & 0xf000) == 0xe000) {
		int16_t i = int8(insword & 0xff);
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = i;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov #%d,r%d", i, n);
	}

	// 0110nnnnmmmm0011 "mov Rm,Rn"
	else if((insword & 0xf00f) == 0x6003) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov r%d,r%d", m, n);
	}

	// 0000nnnnmmmm1100 "mov.b @(r0,Rm),Rn"
	else if((insword & 0xf00f) == 0xc) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = DEREF_REG_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0);
		result->operands[0].regB = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.b @(r0,r%d),r%d", m, n);
	}

	// 11000100dddddddd "mov.b @(disp,gbr),r0"
	else if((insword & 0xff00) == 0xc400) {
		uint16_t d = insword & 0xff;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = DEREF_REG_IMM;
		result->operands[0].regA = (SH4_REGISTER)(GBR);
		result->operands[0].displacement = d;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		sprintf(result->string, "mov.b @(%d,gbr),r0", d);
	}

	// 10000100mmmmdddd "mov.b @(disp,Rm),r0"
	else if((insword & 0xff00) == 0x8400) {
		uint16_t d = insword & 0xf;
		uint16_t m = (insword & 0xf0)>>4;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = DEREF_REG_IMM;
		result->operands[0].regA = (SH4_REGISTER)(R0 + d);
		result->operands[0].displacement = R0 + m;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		sprintf(result->string, "mov.b @(%d,r%d),r0", d, m);
	}

	// 0110nnnnmmmm0100 "mov.b @Rm+,Rn"
	else if((insword & 0xf00f) == 0x6004) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.b @r%d+,r%d", m, n);
	}

	// 0110nnnnmmmm0000 "mov.b @Rm,Rn"
	else if((insword & 0xf00f) == 0x6000) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.b @r%d,r%d", m, n);
	}

	// 11000000dddddddd "mov.b r0,@(disp,gbr)"
	else if((insword & 0xff00) == 0xc000) {
		uint16_t d = insword & 0xff;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0);
		result->operands[1].type = DEREF_REG_IMM;
		result->operands[1].regA = (SH4_REGISTER)(GBR);
		result->operands[1].displacement = d;
		sprintf(result->string, "mov.b r0,@(%d,gbr)", d);
	}

	// 10000000nnnndddd "mov.b r0,@(disp,Rn)"
	else if((insword & 0xff00) == 0x8000) {
		uint16_t d = insword & 0xf;
		uint16_t n = (insword & 0xf0)>>4;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0);
		result->operands[1].type = DEREF_REG_IMM;
		result->operands[1].regA = (SH4_REGISTER)(R0 + d);
		result->operands[1].displacement = R0 + n;
		sprintf(result->string, "mov.b r0,@(%d,r%d)", d, n);
	}

	// 0000nnnnmmmm0100 "mov.b Rm,@(r0,Rn)"
	else if((insword & 0xf00f) == 0x4) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = DEREF_REG_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		result->operands[1].regB = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.b r%d,@(r0,r%d)", m, n);
	}

	// 0010nnnnmmmm0100 "mov.b Rm,@-Rn"
	else if((insword & 0xf00f) == 0x2004) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.b r%d,@-r%d", m, n);
	}

	// 0010nnnnmmmm0000 "mov.b Rm,@Rn"
	else if((insword & 0xf00f) == 0x2000) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.b r%d,@r%d", m, n);
	}

	// 0000nnnnmmmm1110 "mov.l @(r0,Rm),Rn"
	else if((insword & 0xf00f) == 0xe) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = DEREF_REG_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0);
		result->operands[0].regB = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.l @(r0,r%d),r%d", m, n);
	}

	// 11000110dddddddd "mov.l @(disp,gbr),r0"
	else if((insword & 0xff00) == 0xc600) {
		uint16_t d = insword & 0xff;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = DEREF_REG_IMM;
		result->operands[0].regA = (SH4_REGISTER)(GBR);
		result->operands[0].displacement = 4*d;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		sprintf(result->string, "mov.l @(%d,gbr),r0", 4*d);
	}

	// 1101nnnndddddddd "mov.l @(disp,PC),Rn"
	else if((insword & 0xf000) == 0xd000) {
		uint64_t d = displ2ea(4,insword & 0xff,addr);
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = ADDRESS;
		result->operands[0].immediate = d;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.l 0x%016llx,r%d", d, n);
	}

	// 0101nnnnmmmmdddd "mov.l @(disp,Rm),Rn"
	else if((insword & 0xf000) == 0x5000) {
		uint16_t d = insword & 0xf;
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = DEREF_REG_IMM;
		result->operands[0].regA = (SH4_REGISTER)(R0 + 4*d);
		result->operands[0].displacement = R0 + m;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.l @(%d,r%d),r%d", 4*d, m, n);
	}

	// 0110nnnnmmmm0110 "mov.l @Rm+,Rn"
	else if((insword & 0xf00f) == 0x6006) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.l @r%d+,r%d", m, n);
	}

	// 0110nnnnmmmm0010 "mov.l @Rm,Rn"
	else if((insword & 0xf00f) == 0x6002) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.l @r%d,r%d", m, n);
	}

	// 11000010dddddddd "mov.l r0,@(disp,gbr)"
	else if((insword & 0xff00) == 0xc200) {
		uint16_t d = insword & 0xff;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0);
		result->operands[1].type = DEREF_REG_IMM;
		result->operands[1].regA = (SH4_REGISTER)(GBR);
		result->operands[1].displacement = 4*d;
		sprintf(result->string, "mov.l r0,@(%d,gbr)", 4*d);
	}

	// 0000nnnnmmmm0110 "mov.l Rm,@(r0,Rn)"
	else if((insword & 0xf00f) == 0x6) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = DEREF_REG_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		result->operands[1].regB = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.l r%d,@(r0,r%d)", m, n);
	}

	// 0001nnnnmmmmdddd "mov.l Rm,@(disp,Rn)"
	else if((insword & 0xf000) == 0x1000) {
		uint16_t d = insword & 0xf;
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = DEREF_REG_IMM;
		result->operands[1].regA = (SH4_REGISTER)(R0 + 4*d);
		result->operands[1].displacement = R0 + n;
		sprintf(result->string, "mov.l r%d,@(%d,r%d)", m, 4*d, n);
	}

	// 0010nnnnmmmm0110 "mov.l Rm,@-Rn"
	else if((insword & 0xf00f) == 0x2006) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.l r%d,@-r%d", m, n);
	}

	// 0010nnnnmmmm0010 "mov.l Rm,@Rn"
	else if((insword & 0xf00f) == 0x2002) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.l r%d,@r%d", m, n);
	}

	// 0000nnnnmmmm1101 "mov.w @(r0,Rm),Rn"
	else if((insword & 0xf00f) == 0xd) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = DEREF_REG_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0);
		result->operands[0].regB = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.w @(r0,r%d),r%d", m, n);
	}

	// 11000101dddddddd "mov.w @(disp,gbr),r0"
	else if((insword & 0xff00) == 0xc500) {
		uint16_t d = insword & 0xff;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = DEREF_REG_IMM;
		result->operands[0].regA = (SH4_REGISTER)(GBR);
		result->operands[0].displacement = 2*d;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		sprintf(result->string, "mov.w @(%d,gbr),r0", 2*d);
	}

	// 1001nnnndddddddd "mov.w @(disp,PC),Rn"
	else if((insword & 0xf000) == 0x9000) {
		uint16_t d = (addr+4) + 2*(insword & 0xff);
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = d;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.w 0x%016x,r%d", d, n);
	}

	// 10000101mmmmdddd "mov.w @(disp,Rm),r0"
	else if((insword & 0xff00) == 0x8500) {
		uint16_t d = insword & 0xf;
		uint16_t m = (insword & 0xf0)>>4;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = DEREF_REG_IMM;
		result->operands[0].regA = (SH4_REGISTER)(R0 + 2*d);
		result->operands[0].displacement = R0 + m;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		sprintf(result->string, "mov.w @(%d,r%d),r0", 2*d,m);
	}

	// 0110nnnnmmmm0101 "mov.w @Rm+,Rn"
	else if((insword & 0xf00f) == 0x6005) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.w @r%d+,r%d", m, n);
	}

	// 0110nnnnmmmm0001 "mov.w @Rm,Rn"
	else if((insword & 0xf00f) == 0x6001) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.w @r%d,r%d", m, n);
	}

	// 11000001dddddddd "mov.w r0,@(disp,gbr)"
	else if((insword & 0xff00) == 0xc100) {
		uint16_t d = insword & 0xff;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0);
		result->operands[1].type = DEREF_REG_IMM;
		result->operands[1].regA = (SH4_REGISTER)(GBR);
		result->operands[1].displacement = 2*d;
		sprintf(result->string, "mov.w r0,@(%d,gbr)", 2*d);
	}

	// 10000001nnnndddd "mov.w r0,@(disp,Rn)"
	else if((insword & 0xff00) == 0x8100) {
		uint16_t d = insword & 0xf;
		uint16_t n = (insword & 0xf0)>>4;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0);
		result->operands[1].type = DEREF_REG_IMM;
		result->operands[1].regA = (SH4_REGISTER)(R0 + 2*d);
		result->operands[1].displacement = R0 + n;
		sprintf(result->string, "mov.w r0,@(%d,r%d)", 2*d, n);
	}

	// 0000nnnnmmmm0101 "mov.w Rm,@(r0,Rn)"
	else if((insword & 0xf00f) == 0x5) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = DEREF_REG_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		result->operands[1].regB = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.w r%d,@(r0,r%d)", m, n);
	}

	// 0010nnnnmmmm0101 "mov.w Rm,@-Rn"
	else if((insword & 0xf00f) == 0x2005) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.w r%d,@-r%d", m, n);
	}

	// 0010nnnnmmmm0001 "mov.w Rm,@Rn"
	else if((insword & 0xf00f) == 0x2001) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOV;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mov.w r%d,@r%d", m, n);
	}

	// 11000111dddddddd "mova @(disp,PC),r0"
	else if((insword & 0xff00) == 0xc700) {
		uint64_t d = displ2ea(4, insword & 0xff, addr);
		result->opcode = OPC_MOVA;
		result->operands[0].type = ADDRESS;
		result->operands[0].immediate = d;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		sprintf(result->string, "mova 0x%016llx,r0", d);
	}

	// 0000nnnn11000011 "movca.l r0,@Rn"
	else if((insword & 0xf0ff) == 0xc3) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOVCA;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0);
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "movca.l r0,@r%d", n);
	}

	// 0000nnnn00101001 "movt Rn"
	else if((insword & 0xf0ff) == 0x29) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MOVT;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "movt r%d", n);
	}

	// 0000nnnnmmmm0111 "mul.l Rm,Rn"
	else if((insword & 0xf00f) == 0x7) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MUL;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mul.l r%d,r%d", m, n);
	}

	// 0010nnnnmmmm1111 "muls.w Rm,Rn"
	else if((insword & 0xf00f) == 0x200f) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MULS;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "muls.w r%d,r%d", m, n);
	}

	// 0010nnnnmmmm1110 "mulu.w Rm,Rn"
	else if((insword & 0xf00f) == 0x200e) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_MULU;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "mulu.w r%d,r%d", m, n);
	}

	// 0110nnnnmmmm1011 "neg Rm,Rn"
	else if((insword & 0xf00f) == 0x600b) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_NEG;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "neg r%d,r%d", m, n);
	}

	// 0110nnnnmmmm1010 "negc Rm,Rn"
	else if((insword & 0xf00f) == 0x600a) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_NEGC;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "negc r%d,r%d", m, n);
	}

	// 0000000000001001 "nop"
	else if(insword == 0x9) {
		result->opcode = OPC_NOP;
		sprintf(result->string, "nop");
	}

	// 0110nnnnmmmm0111 "not Rm,Rn"
	else if((insword & 0xf00f) == 0x6007) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_NOT;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "not r%d,r%d", m, n);
	}

	// 0000nnnn10010011 "ocbi @Rn"
	else if((insword & 0xf0ff) == 0x93) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_OCBI;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "ocbi @r%d", n);
	}

	// 0000nnnn10100011 "ocbp @Rn"
	else if((insword & 0xf0ff) == 0xa3) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_OCBP;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "ocbp @r%d", n);
	}

	// 0000nnnn10110011 "ocbwb @Rn"
	else if((insword & 0xf0ff) == 0xb3) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_OCBWB;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "ocbwb @r%d", n);
	}

	// 11001011iiiiiiii "or #imm,r0"
	else if((insword & 0xff00) == 0xcb00) {
		int16_t i = int8(insword & 0xff);
		result->opcode = OPC_OR;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = i;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		sprintf(result->string, "or #%d,r0", i);
	}

	// 0010nnnnmmmm1011 "or Rm,Rn"
	else if((insword & 0xf00f) == 0x200b) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_OR;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "or r%d,r%d", m, n);
	}

	// 11001111iiiiiiii "or.b #imm,@(r0,gbr)"
	else if((insword & 0xff00) == 0xcf00) {
		int16_t i = int8(insword & 0xff);
		result->opcode = OPC_OR;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = i;
		result->operands[1].type = DEREF_REG_REG;
		result->operands[1].regA = (SH4_REGISTER)(GBR);
		result->operands[1].regB = R0;
		sprintf(result->string, "or.b #%d,@(r0,gbr)", i);
	}

	// 0000nnnn10000011 "pref @Rn"
	else if((insword & 0xf0ff) == 0x83) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_PREF;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "pref @r%d", n);
	}

	// 0100nnnn00100100 "rotcl Rn"
	else if((insword & 0xf0ff) == 0x4024) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_ROTCL;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "rotcl r%d", n);
	}

	// 0100nnnn00100101 "rotcr Rn"
	else if((insword & 0xf0ff) == 0x4025) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_ROTCR;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "rotcr r%d", n);
	}

	// 0100nnnn00000100 "rotl Rn"
	else if((insword & 0xf0ff) == 0x4004) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_ROTL;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "rotl r%d", n);
	}

	// 0100nnnn00000101 "rotr Rn"
	else if((insword & 0xf0ff) == 0x4005) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_ROTR;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "rotr r%d", n);
	}

	// 0000000000101011 "rte"
	else if(insword == 0x2b) {
		result->opcode = OPC_RTE;
		sprintf(result->string, "rte");
	}

	// 0000000000001011 "rts"
	else if(insword == 0xb) {
		result->opcode = OPC_RTS;
		sprintf(result->string, "rts");
	}

	// 0000000000011000 "sett"
	else if(insword == 0x18) {
		result->opcode = OPC_SETT;
		sprintf(result->string, "sett");
	}

	// 0000000001011000 "sett"
	else if(insword == 0x58) {
		result->opcode = OPC_SETS;
		sprintf(result->string, "sets");
	}

	// 0100nnnnmmmm1100 "shad Rm,Rn"
	else if((insword & 0xf00f) == 0x400c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SHAD;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "shad r%d,r%d", m, n);
	}

	// 0100nnnn00100000 "shal Rn"
	else if((insword & 0xf0ff) == 0x4020) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SHAL;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "shal r%d", n);
	}

	// 0100nnnn00100001 "shar Rn"
	else if((insword & 0xf0ff) == 0x4021) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SHAR;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "shar r%d", n);
	}

	// 0100nnnnmmmm1101 "shld Rm,Rn"
	else if((insword & 0xf00f) == 0x400d) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SHLD;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "shld r%d,r%d", m, n);
	}

	// 0100nnnn00000000 "shll Rn"
	else if((insword & 0xf0ff) == 0x4000) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SHLL;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "shll r%d", n);
	}

	// 0100nnnn00101000 "shll16 Rn"
	else if((insword & 0xf0ff) == 0x4028) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SHLL16;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "shll16 r%d", n);
	}

	// 0100nnnn00001000 "shll2 Rn"
	else if((insword & 0xf0ff) == 0x4008) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SHLL2;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "shll2 r%d", n);
	}

	// 0100nnnn00011000 "shll8 Rn"
	else if((insword & 0xf0ff) == 0x4018) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SHLL8;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "shll8 r%d", n);
	}

	// 0100nnnn00000001 "shlr Rn"
	else if((insword & 0xf0ff) == 0x4001) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SHLR;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "shlr r%d", n);
	}

	// 0100nnnn00101001 "shlr16 Rn"
	else if((insword & 0xf0ff) == 0x4029) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SHLR16;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "shlr16 r%d", n);
	}

	// 0100nnnn00001001 "shlr2 Rn"
	else if((insword & 0xf0ff) == 0x4009) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SHLR2;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "shlr2 r%d", n);
	}

	// 0100nnnn00011001 "shlr8 Rn"
	else if((insword & 0xf0ff) == 0x4019) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SHLR8;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "shlr8 r%d", n);
	}

	// 0000000000011011 "sleep"
	else if(insword == 0x1b) {
		result->opcode = OPC_SLEEP;
		sprintf(result->string, "sleep");
	}

	// 0000nnnn1mmm0010 "stc Rn,Rm_bank"
	else if((insword & 0xF08F) == 0x0082) {
		uint16_t n = (insword & 0xf00)>>8;
		uint16_t m = (insword & 0x70)>>4;
		result->opcode = OPC_STC;
		result->operands[0].type = BANKREG;
		result->operands[0].regA = (SH4_REGISTER)(BANKREG + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc r%d_bank,r%d", m, n);
	}

	// 0000nnnn11111010 "stc dbr,Rn"
	else if((insword & 0xf0ff) == 0xfa) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(DBR);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc dbr,r%d", n);
	}

	// 0000nnnn00010010 "stc gbr,Rn"
	else if((insword & 0xf0ff) == 0x12) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(GBR);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc gbr,r%d", n);
	}

	// 0000nnnn00111010 "stc sgr,Rn"
	else if((insword & 0xf0ff) == 0x3a) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(SGR);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc sgr,r%d", n);
	}

	// 0000nnnn01000010 "stc spc,Rn"
	else if((insword & 0xf0ff) == 0x42) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(SPC);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc spc,r%d", n);
	}

	// 0000nnnn00000010 "stc sr,Rn"
	else if((insword & 0xf0ff) == 0x2) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(SR);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc sr,r%d", n);
	}

	// 0000nnnn00110010 "stc ssr,Rn"
	else if((insword & 0xf0ff) == 0x32) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(SSR);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc ssr,r%d", n);
	}

	// 0000nnnn00100010 "stc vbr,Rn"
	else if((insword & 0xf0ff) == 0x22) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(VBR);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc vbr,r%d", n);
	}

	// MANUAL
	// 0000nnnn1mmm0010 "stc Rm_bank,Rn"
	else if((insword & 0xf08f) == 0x82) {
		uint16_t m = (insword & 0x70)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->operands[0].type = BANKREG;
		result->operands[0].regA = (SH4_REGISTER)(BANKREG + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc r%d_bank,r%d", m, n);
	}

	// MANUAL
	// 0000nnnn0mmm0010 "stc Rm_bank,Rn"
	else if((insword & 0xf08f) == 0x2) {
		uint16_t m = (insword & 0x70)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		if(m < 5) {
			result->opcode = OPC_STC;
			result->operands[0].type = GPREG;
			result->operands[0].regA = (SH4_REGISTER)(R0 + n);
			result->operands[1].type = GPREG;
			result->operands[1].regA = (SH4_REGISTER)(R0 + cr2id[m]);
			sprintf(result->string, "stc %s,r%d", cr2str[m], n);
		}
		else {
			result->opcode = OPC_STC;
			result->operands[0].type = BANKREG;
			result->operands[0].regA = (SH4_REGISTER)(BANKREG + m);
			result->operands[1].type = GPREG;
			result->operands[1].regA = (SH4_REGISTER)(R0 + n);
			sprintf(result->string, "stc r%d_bank,r%d", m, n);
		}
	}

	// 0100nnnn11110010 "stc.l dbr,@-Rn"
	else if((insword & 0xf0ff) == 0x40f2) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(DBR);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc.l dbr,@-r%d", n);
	}

	// 0100nnnn00010011 "stc.l gbr,@-Rn"
	else if((insword & 0xf0ff) == 0x4013) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(GBR);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc.l gbr,@-r%d", n);
	}

	// 0100nnnn1mmm0011 "stc.l Rm_bank,@-Rn"
	else if((insword & 0xf08f) == 0x4083) {
		uint16_t m = (insword & 0x70)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = BANKREG;
		result->operands[0].regA = (SH4_REGISTER)(BANKREG + m);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc.l r%d_bank,@-r%d", m, n);
	}

	// MANUAL
	// 0100nnnn0mmm0011 "stc.l Rm_bank,@-Rn"
	else if((insword & 0xf08f) == 0x4003) {
		uint16_t m = (insword & 0x70)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		if(m < 5) {
			result->opcode = OPC_STC;
			result->length_suffix = LEN_SUFFIX_L;
			result->operands[0].type = CTRLREG;
			result->operands[0].regA = (SH4_REGISTER)(cr2id[m]);
			result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
			result->operands[1].type = DEREF_REG;
			result->operands[1].regA = (SH4_REGISTER)(R0 + cr2id[m]);
			sprintf(result->string, "stc.l %s,@-r%d", cr2str[m], n);
		}
		else {
			result->opcode = OPC_STC;
			result->length_suffix = LEN_SUFFIX_L;
			result->operands[0].type = BANKREG;
			result->operands[0].regA = (SH4_REGISTER)(BANKREG + m);
			result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
			result->operands[1].type = DEREF_REG;
			result->operands[1].regA = (SH4_REGISTER)(R0 + n);
			sprintf(result->string, "stc.l r%d_bank,@-r%d", m, n);
		}
	}

	// 0100nnnn00110010 "stc.l sgr,@-Rn"
	else if((insword & 0xf0ff) == 0x4032) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(SGR);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc.l sgr,@-r%d", n);
	}

	// 0100nnnn01000011 "stc.l spc,@-Rn"
	else if((insword & 0xf0ff) == 0x4043) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(SPC);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc.l spc,@-r%d", n);
	}

	// 0100nnnn00000011 "stc.l sr,@-Rn"
	else if((insword & 0xf0ff) == 0x4003) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(SR);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc.l sr,@-r%d", n);
	}

	// 0100nnnn00110011 "stc.l ssr,@-Rn"
	else if((insword & 0xf0ff) == 0x4033) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(SSR);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc.l ssr,@-r%d", n);
	}

	// 0100nnnn00100011 "stc.l vbr,@-Rn"
	else if((insword & 0xf0ff) == 0x4023) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STC;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = CTRLREG;
		result->operands[0].regA = (SH4_REGISTER)(VBR);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "stc.l vbr,@-r%d", n);
	}

	// 0000nnnn01101010 "sts fpscr,Rn"
	else if((insword & 0xf0ff) == 0x6a) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STS;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(FPSCR);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "sts fpscr,r%d", n);
	}

	// 0000nnnn01011010 "sts fpul,Rn"
	else if((insword & 0xf0ff) == 0x5a) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STS;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(FPUL);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "sts fpul,r%d", n);
	}

	// 0000nnnn00001010 "sts mach,Rn"
	else if((insword & 0xf0ff) == 0xa) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STS;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(MACH);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "sts mach,r%d", n);
	}

	// 0000nnnn00011010 "sts macl,Rn"
	else if((insword & 0xf0ff) == 0x1a) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STS;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(MACL);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "sts macl,r%d", n);
	}

	// 0000nnnn00101010 "sts pr,Rn"
	else if((insword & 0xf0ff) == 0x2a) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STS;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(PR);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "sts pr,r%d", n);
	}

	// 0100nnnn01100010 "sts.l fpscr,@-Rn"
	else if((insword & 0xf0ff) == 0x4062) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STS;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(FPSCR);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "sts.l fpscr,@-r%d", n);
	}

	// 0100nnnn01010010 "sts.l fpul,@-Rn"
	else if((insword & 0xf0ff) == 0x4052) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STS;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(FPUL);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "sts.l fpul,@-r%d", n);
	}

	// 0100nnnn00000010 "sts.l mach,@-Rn"
	else if((insword & 0xf0ff) == 0x4002) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STS;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(MACH);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "sts.l mach,@-r%d", n);
	}

	// 0100nnnn00010010 "sts.l macl,@-Rn"
	else if((insword & 0xf0ff) == 0x4012) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STS;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(MACL);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "sts.l macl,@-r%d", n);
	}

	// 0100nnnn00100010 "sts.l pr,@-Rn"
	else if((insword & 0xf0ff) == 0x4022) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_STS;
		result->length_suffix = LEN_SUFFIX_L;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(PR);
		result->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		result->operands[1].type = DEREF_REG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "sts.l pr,@-r%d", n);
	}

	// 0011nnnnmmmm1000 "sub Rm,Rn"
	else if((insword & 0xf00f) == 0x3008) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SUB;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "sub r%d,r%d", m, n);
	}

	// 0011nnnnmmmm1010 "subc Rm,Rn"
	else if((insword & 0xf00f) == 0x300a) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SUBC;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "subc r%d,r%d", m, n);
	}

	// 0011nnnnmmmm1011 "subv Rm,Rn"
	else if((insword & 0xf00f) == 0x300b) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SUBV;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "subv r%d,r%d", m, n);
	}

	// 0110nnnnmmmm1000 "swap.b Rm,Rn"
	else if((insword & 0xf00f) == 0x6008) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SWAP;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "swap.b r%d,r%d", m, n);
	}

	// 0110nnnnmmmm1001 "swap.w Rm,Rn"
	else if((insword & 0xf00f) == 0x6009) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_SWAP;
		result->length_suffix = LEN_SUFFIX_W;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "swap.w r%d,r%d", m, n);
	}

	// 0100nnnn00011011 "tas.b @Rn"
	else if((insword & 0xf0ff) == 0x401b) {
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_TAS;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = DEREF_REG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "tas.b @r%d", n);
	}

	// 11000011iiiiiiii "trapa #imm"
	else if((insword & 0xff00) == 0xc300) {
		int16_t i = int8(insword & 0xff);
		result->opcode = OPC_TRAPA;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = i;
		sprintf(result->string, "trapa #%d", i);
	}

	// 11001000iiiiiiii "tst #imm,r0"
	else if((insword & 0xff00) == 0xc800) {
		int16_t i = int8(insword & 0xff);
		result->opcode = OPC_TST;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = i;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		sprintf(result->string, "tst #%d,r0", i);
	}

	// 0010nnnnmmmm1000 "tst Rm,Rn"
	else if((insword & 0xf00f) == 0x2008) {
		int16_t m = (insword & 0xf0)>>4;
		int16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_TST;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "tst r%d,r%d", m, n);
	}

	// 11001100iiiiiiii "tst.b #imm,@(r0,gbr)"
	else if((insword & 0xff00) == 0xcc00) {
		int16_t i = int8(insword & 0xff);
		result->opcode = OPC_TST;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = i;
		result->operands[1].type = DEREF_REG_REG;
		result->operands[1].regA = (SH4_REGISTER)(GBR);
		result->operands[1].regB = R0;
		sprintf(result->string, "tst.b #%d,@(r0,gbr)", i);
	}

	// 11001010iiiiiiii "xor #imm,r0"
	else if((insword & 0xff00) == 0xca00) {
		int16_t i = int8(insword & 0xff);
		result->opcode = OPC_XOR;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = i;
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0);
		sprintf(result->string, "xor #%d,r0", i);
	}

	// 0010nnnnmmmm1010 "xor Rm,Rn"
	else if((insword & 0xf00f) == 0x200a) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_XOR;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "xor r%d,r%d", m, n);
	}

	// 11001110iiiiiiii "xor.b #imm,@(r0,gbr)"
	else if((insword & 0xff00) == 0xce00) {
		int16_t i = int8(insword & 0xff);
		result->opcode = OPC_XOR;
		result->length_suffix = LEN_SUFFIX_B;
		result->operands[0].type = IMMEDIATE;
		result->operands[0].immediate = i;
		result->operands[1].type = DEREF_REG_REG;
		result->operands[1].regA = (SH4_REGISTER)(GBR);
		result->operands[1].regB = R0;
		sprintf(result->string, "xor.b #%d,@(r0,gbr)", i);
	}

	// 0010nnnnmmmm1101 "xtrct Rm,Rn"
	else if((insword & 0xf00f) == 0x200d) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		result->opcode = OPC_XTRCT;
		result->operands[0].type = GPREG;
		result->operands[0].regA = (SH4_REGISTER)(R0 + m);
		result->operands[1].type = GPREG;
		result->operands[1].regA = (SH4_REGISTER)(R0 + n);
		sprintf(result->string, "xtrct r%d,r%d", m, n);
	}

	// 1111nnnn01111101 "fsrra Rn"
	else if((insword & 0xf0ff) == 0xf07d) {
		uint16_t m = (insword & 0xf00)>>8;
		result->opcode = OPC_FSRRA;
		result->operands[0].type = FPUREG;
		result->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		sprintf(result->string, "fsrra fr%d", m);
	}

	// 1111nnn011111101 "fsca fpul, drn"
	else if((insword & 0xf1ff) == 0xf0fd) {
		uint16_t m = (insword & 0xE00)>>9;
		result->opcode = OPC_FSCA;
		result->operands[0].type = SYSREG;
		result->operands[0].regA = (SH4_REGISTER)(FPUL);
		result->operands[1].type = FPUREG;
		result->operands[1].regA = (SH4_REGISTER)(DR0 + 2*m);
		sprintf(result->string, "fsca fpul,dr%d", 2*m);
	}

	// didn't disassemble!
	else {
		result->opcode = OPC_ERROR;
		sprintf(result->string, "error");
		rc = -1;
	}

	return rc;
}
