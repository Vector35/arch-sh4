#include "decode.h"

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
static uint64_t displ2ea(int scale, int disp, int64_t base)
{
	//printf("%s(%d, %d, 0x%016llx)\n", __func__, scale, disp, base);
	int64_t dest = (base+4) + (scale*disp);
	return dest;
}

int sh4_decompose(uint16_t insword, struct Instruction *instr, uint64_t addr)
{
	int rc = 0;
	char buff[128];

	const char *cr2str[5] = {"sr","gbr","vbr","ssr","spc"};
	enum SH4_REGISTER cr2id[5] = {SR,GBR,VBR,SSR,SPC};

	// 0111nnnniiiiiiii "add #imm,Rn"
	if((insword & 0xf000) == 0x7000) {
		int16_t i = int8(insword & 0xff);
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_ADD;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm1100 "add Rm,Rn"
	else if((insword & 0xf00f) == 0x300c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_ADD;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm1110 "addc Rm,Rn"
	else if((insword & 0xf00f) == 0x300e) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_ADDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm1111 "addv Rm,Rn"
	else if((insword & 0xf00f) == 0x300f) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_ADDV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 11001001iiiiiiii "and #imm,r0"
	else if((insword & 0xff00) == 0xc900) {
		int16_t i = int8(insword & 0xff);
		instr->opcode = OPC_AND;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm1001 "and Rm,Rn"
	else if((insword & 0xf00f) == 0x2009) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_AND;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 11001101iiiiiiii "and.b #imm,@(r0,gbr)"
	else if((insword & 0xff00) == 0xcd00) {
		int16_t i = int8(insword & 0xff);
		instr->opcode = OPC_AND;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = GBR;
		instr->operands_n = 2;
	}

	// 10001011dddddddd "bf label"
	else if((insword & 0xff00) == 0x8b00) {
		uint64_t d = displ2ea(2, int8(insword & 0xff), addr);
		instr->opcode = OPC_BF;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = d;
		instr->operands_n = 1;
	}

	// 10001111dddddddd "bf.s label"
	else if((insword & 0xff00) == 0x8f00) {
		uint64_t d = displ2ea(2, int8(insword & 0xff), addr);
		instr->opcode = OPC_BF;
		instr->delay_slot = true;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = d;
		instr->operands_n = 1;
	}

	// 1010dddddddddddd "bra label"
	else if((insword & 0xf000) == 0xa000) {
		uint64_t d = displ2ea(2, int12(insword & 0xfff), addr);
		instr->opcode = OPC_BRA;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = d;
		instr->operands_n = 1;
	}

	// 0000mmmm00100011 "braf Rm"
	else if((insword & 0xf0ff) == 0x23) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_BRAF;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands_n = 1;
	}

	// 1011dddddddddddd "bsr label"
	else if((insword & 0xf000) == 0xb000) {
		uint64_t d = displ2ea(2, int12(insword & 0xfff), addr);
		instr->opcode = OPC_BSR;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = d;
		instr->operands_n = 1;
	}

	// 0000mmmm00000011 "bsrf Rm"
	else if((insword & 0xf0ff) == 0x3) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_BSRF;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands_n = 1;
	}

	// 10001001dddddddd "bt label"
	else if((insword & 0xff00) == 0x8900) {
		uint64_t d = displ2ea(2, int8(insword & 0xff), addr);
		instr->opcode = OPC_BT;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = d;
		instr->operands_n = 1;
	}

	// 10001101dddddddd "bt.s label"
	else if((insword & 0xff00) == 0x8d00) {
		uint64_t d = displ2ea(2, int8(insword & 0xff), addr);
		instr->opcode = OPC_BT;
		instr->delay_slot = true;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = d;
		instr->operands_n = 1;
	}

	// 0000000000101000 "clrmac"
	else if(insword == 0x28) {
		instr->opcode = OPC_CLRMAC;
		instr->operands_n = 0;
	}

	// 0000000001001000 "clrs"
	else if(insword == 0x48) {
		instr->opcode = OPC_CLRS;
		instr->operands_n = 0;
	}

	// 0000000000001000 "clrt"
	else if(insword == 0x8) {
		instr->opcode = OPC_CLRT;
		instr->operands_n = 0;
	}

	// 10001000iiiiiiii "cmp/eq #imm,r0"
	else if((insword & 0xff00) == 0x8800) {
		int16_t i = int8(insword & 0xff);
		instr->opcode = OPC_CMP;
		instr->cond = CMP_COND_EQ;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm0000 "cmp/eq Rm,Rn"
	else if((insword & 0xf00f) == 0x3000) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_CMP;
		instr->cond = CMP_COND_EQ;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm0011 "cmp/ge Rm,Rn"
	else if((insword & 0xf00f) == 0x3003) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_CMP;
		instr->cond = CMP_COND_GE;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm0111 "cmp/gt Rm,Rn"
	else if((insword & 0xf00f) == 0x3007) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_CMP;
		instr->cond = CMP_COND_GT;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm0110 "cmp/hi Rm,Rn"
	else if((insword & 0xf00f) == 0x3006) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_CMP;
		instr->cond = CMP_COND_HI;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm0010 "cmp/hs Rm,Rn"
	else if((insword & 0xf00f) == 0x3002) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_CMP;
		instr->cond = CMP_COND_HS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn00010101 "cmp/pl Rn"
	else if((insword & 0xf0ff) == 0x4015) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_CMP;
		instr->cond = CMP_COND_PL;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00010001 "cmp/pz Rn"
	else if((insword & 0xf0ff) == 0x4011) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_CMP;
		instr->cond = CMP_COND_PZ;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0010nnnnmmmm1100 "cmp/str Rm,Rn"
	else if((insword & 0xf00f) == 0x200c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_CMP;
		instr->cond = CMP_COND_STR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm0111 "div0s Rm,Rn"
	else if((insword & 0xf00f) == 0x2007) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_DIV0S;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000000000011001 "div0u"
	else if(insword == 0x19) {
		instr->opcode = OPC_DIV0U;
		instr->operands_n = 0;
	}

	// 0011nnnnmmmm0100 "div1 Rm,Rn"
	else if((insword & 0xf00f) == 0x3004) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_DIV1;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm1101 "dmuls.l Rm,Rn"
	else if((insword & 0xf00f) == 0x300d) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_DMULS;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm0101 "dmulu.l Rm,Rn"
	else if((insword & 0xf00f) == 0x3005) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_DMULU;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn00010000 "dt Rn"
	else if((insword & 0xf0ff) == 0x4010) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_DT;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0110nnnnmmmm1110 "exts.b Rm,Rn"
	else if((insword & 0xf00f) == 0x600e) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_EXTS;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm1111 "exts.w Rm,Rn"
	else if((insword & 0xf00f) == 0x600f) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_EXTS;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm1100 "extu.b Rm,Rn"
	else if((insword & 0xf00f) == 0x600c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_EXTU;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm1101 "extu.w Rm,Rn"
	else if((insword & 0xf00f) == 0x600d) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_EXTU;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn001011101 "fabs DRn"
	else if((insword & 0xf1ff) == 0xf05d) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FABS;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 1;
	}

	// 1111nnnn01011101 "fabs FRn"
	else if((insword & 0xf0ff) == 0xf05d) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FABS;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 1;
	}

	// 1111nnn0mmmm0000 "fadd DRm,DRn"
	else if((insword & 0xf10f) == 0xf000) {
		uint16_t m = (insword & 0xF0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FADD;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm0000 "fadd FRm,FRn"
	else if((insword & 0xf00f) == 0xf000) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FADD;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn0mmmm0100 "fcmp/eq DRm,DRn"
	else if((insword & 0xf11f) == 0xf004) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FCMP;
		instr->cond = CMP_COND_EQ;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm0100 "fcmp/eq FRm,FRn"
	else if((insword & 0xf00f) == 0xf004) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FCMP;
		instr->cond = CMP_COND_EQ;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn0mmm00101 "fcmp/gt DRm,DRn"
	else if((insword & 0xf11f) == 0xf005) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FCMP;
		instr->cond = CMP_COND_GT;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm0101 "fcmp/gt FRm,FRn"
	else if((insword & 0xf00f) == 0xf005) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FCMP;
		instr->cond = CMP_COND_GT;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111mmm010111101 "fcnvds DRm,fpul"
	else if((insword & 0xf1ff) == 0xf0bd) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_FCNVDS;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(DR0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(FPUL);
		instr->operands_n = 2;
	}

	// 1111nnn010101101 "fcnvsd fpul,DRn"
	else if((insword & 0xf1ff) == 0xf0ad) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FCNVSD;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(FPUL);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(DR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn0mmmm0011 "fdiv DRm,DRn"
	else if((insword & 0xf10f) == 0xf003) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FDIV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm0011 "fdiv FRm,FRn"
	else if((insword & 0xf00f) == 0xf003) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FDIV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnmm11101101 "fipr fvm,fvn"
	else if((insword & 0xf0ff) == 0xf0ed) {
		uint16_t m = (insword & 0x300)>>8;
		uint16_t n = (insword & 0xc00)>>10;
		instr->opcode = OPC_FIPR;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FV0 + 4*m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FV0 + 4*n);
		instr->operands_n = 2;
	}

	// 1111nnnn10001101 "fldi0 FRn"
	else if((insword & 0xf0ff) == 0xf08d) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FLDI0;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 1;
	}

	// 1111nnnn10011101 "fldi1 FRn"
	else if((insword & 0xf0ff) == 0xf09d) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FLDI1;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 1;
	}

	// 1111mmmm00011101 "flds FRm,fpul"
	else if((insword & 0xf0ff) == 0xf01d) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_FLDS;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(FPUL);
		instr->operands_n = 2;
	}

	// 1111nnn000101101 "float fpul,DRn"
	else if((insword & 0xf1ff) == 0xf02d) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FLOAT;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(FPUL);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnn00101101 "float fpul,FRn"
	else if((insword & 0xf0ff) == 0xf02d) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FLOAT;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(FPUL);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm1110 "fmac fr0,FRm,FRn"
	else if((insword & 0xf00f) == 0xf00e) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMAC;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[2].type = FPUREG;
		instr->operands[2].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 3;
	}

	// 1111nnn0mmm01100 "fmov DRm,DRn"
	else if((insword & 0xf11f) == 0xf00c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn1mmm01100 "fmov DRm,XDn"
	else if((insword & 0xf11f) == 0xf10c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm1100 "fmov FRm,FRn"
	else if((insword & 0xf00f) == 0xf00c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn0mmm11100 "fmov XDm,DRn"
	else if((insword & 0xf11f) == 0xf01c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(XD0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn1mmm11100 "fmov XDm,XDn"
	else if((insword & 0xf11f) == 0xf11c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(XD0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn0mmmm0110 "fmov @(r0,Rm),DRn"
	else if((insword & 0xf10f) == 0xf006) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm0110 "fmov @(r0,Rm),XDn"
	else if((insword & 0xf10f) == 0xf106) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn0mmmm1001 "fmov @Rm+,DRn"
	else if((insword & 0xf10f) == 0xf009) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm1001 "fmov @Rm+,XDn"
	else if((insword & 0xf10f) == 0xf109) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn0mmmm1000 "fmov @Rm,DRn"
	else if((insword & 0xf10f) == 0xf008) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm1000 "fmov @Rm,XDn"
	else if((insword & 0xf10f) == 0xf108) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xF00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmm00111 "fmov DRm,@(r0,Rn)"
	else if((insword & 0xf01f) == 0xf007) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmm01011 "fmov DRm,@-Rn"
	else if((insword & 0xf01f) == 0xf00b) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmm01010 "fmov DRm,@Rn"
	else if((insword & 0xf01f) == 0xf00a) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm0111 "fmov XDm,@(r0,Rn)"
	else if((insword & 0xf00f) == 0xf007) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmm11011 "fmov XDm,@-Rn"
	else if((insword & 0xf00f) == 0xf00b) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm1010 "fmov XDm,@Rn"
	else if((insword & 0xf00f) == 0xf00a) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm0110 "fmov.s @(r0,Rm),FRn"
	else if((insword & 0xf00f) == 0xf006) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->delay_slot = true;
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm1001 "fmov.s @Rm+,FRn"
	else if((insword & 0xf00f) == 0xf009) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->delay_slot = true;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm1000 "fmov.s @Rm,FRn"
	else if((insword & 0xf00f) == 0xf008) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->delay_slot = true;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm0111 "fmov.s FRm,@(r0,Rn)"
	else if((insword & 0xf00f) == 0xf007) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->delay_slot = true;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm1011 "fmov.s FRm,@-Rn"
	else if((insword & 0xf00f) == 0xf00b) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->delay_slot = true;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm1010 "fmov.s FRm,@Rn"
	else if((insword & 0xf00f) == 0xf00a) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMOV;
		instr->delay_slot = true;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn0mmmm0010 "fmul DRm,DRn"
	else if((insword & 0xf10f) == 0xf002) {
		uint16_t m = (insword & 0xF0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMUL;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm0010 "fmul FRm,FRn"
	else if((insword & 0xf00f) == 0xf002) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FMUL;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn001001101 "fneg DRn"
	else if((insword & 0xf1ff) == 0xf04d) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FNEG;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 1;
	}

	// 1111nnnn01001101 "fneg FRn"
	else if((insword & 0xf0ff) == 0xf04d) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FNEG;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 1;
	}

	// 1111101111111101 "frchg"
	else if(insword == 0xfbfd) {
		instr->opcode = OPC_FRCHG;
		instr->operands_n = 0;
	}

	// 1111001111111101 "fschg"
	else if(insword == 0xf3fd) {
		instr->opcode = OPC_FSCHG;
		instr->operands_n = 0;
	}

	// 1111nnn001101101 "fsqrt DRn"
	else if((insword & 0xf1ff) == 0xf06d) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FSQRT;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 1;
	}

	// 1111nnnn01101101 "fsqrt FRn"
	else if((insword & 0xf0ff) == 0xf06d) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FSQRT;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 1;
	}

	// 1111nnnn00001101 "fsts fpul,FRn"
	else if((insword & 0xf0ff) == 0xf00d) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FSTS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(FPUL);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnn0mmmm0001 "fsub DRm,DRn"
	else if((insword & 0xf10f) == 0xf001) {
		uint16_t m = (insword & 0xF0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FSUB;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnnmmmm0001 "fsub FRm,FRn"
	else if((insword & 0xf00f) == 0xf001) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_FSUB;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FR0 + n);
		instr->operands_n = 2;
	}

	// 1111mmm000111101 "ftrc DRm,fpul"
	else if((insword & 0xf1ff) == 0xf03d) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_FTRC;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(FPUL);
		instr->operands_n = 2;
	}

	// 1111mmmm00111101 "ftrc FRm,fpul"
	else if((insword & 0xf0ff) == 0xf03d) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_FTRC;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(FPUL);
		instr->operands_n = 2;
	}

	// 1111nn0111111101 "ftrv xmtrx,fvn"
	else if((insword & 0xf3ff) == 0xf1fd) {
		uint16_t n = (insword & 0xc00)>>10;
		instr->opcode = OPC_FTRV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(XMTRX);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(FV0 + 4*n);
		instr->operands_n = 2;
	}

	// 0100mmmm00101011 "jmp @Rm"
	else if((insword & 0xf0ff) == 0x402b) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_JMP;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands_n = 1;
	}

	// 0100mmmm00001011 "jsr @Rm"
	else if((insword & 0xf0ff) == 0x400b) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_JSR;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands_n = 1;
	}

	// 0100mmmm11110110 "ldc Rm,dbr"
	else if((insword & 0xf0ff) == 0x40f6) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(DBR);
		instr->operands_n = 2;
	}

	// MANUAL
	else if((insword & 0xf0ff) == 0x40fa) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(DBR);
		instr->operands_n = 2;
	}

	// 0100mmmm00011110 "ldc Rm,gbr"
	else if((insword & 0xf0ff) == 0x401e) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = GBR;
		instr->operands_n = 2;
	}

	// 0100mmmm1nnn1110 "ldc Rm,Rn_bank"
	else if((insword & 0xf08f) == 0x408e) {
		uint16_t m = (insword & 0xf00)>>8;
		uint16_t n = (insword & 0x70)>>4;
		instr->opcode = OPC_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = BANKREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0_BANK0 + n);
		instr->operands_n = 2;
	}

	// 0100mmmm01001110 "ldc Rm,spc"
	else if((insword & 0xf0ff) == 0x404e) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(SPC);
		instr->operands_n = 2;
	}

	// 0100mmmm00001110 "ldc Rm,sr"
	else if((insword & 0xf0ff) == 0x400e) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(SR);
		instr->operands_n = 2;
	}

	// 0100mmmm00111110 "ldc Rm,ssr"
	else if((insword & 0xf0ff) == 0x403e) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(SSR);
		instr->operands_n = 2;
	}

	// MANUAL
	// 0100mmmm00111010 "ldc Rm,sgr"
	else if((insword & 0xf0ff) == 0x403A) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(SGR);
		instr->operands_n = 2;
	}

	// 0100mmmm00101110 "ldc Rm,vbr"
	else if((insword & 0xf0ff) == 0x402e) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(VBR);
		instr->operands_n = 2;
	}

	// 0100mmmm11110110 "ldc.l @Rm+,dbr"
	else if((insword & 0xf0ff) == 0x40f6) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(DBR);
		instr->operands_n = 2;
	}

	// MANUAL
	// 0100mmmmnnnn1110
	else if((insword & 0xf00f) == 0x400E) {
		uint16_t m = (insword & 0xf00)>>8;
		uint16_t n = (insword & 0xf0)>>4;
		instr->operands_n = 2;
		if(n < 5) {
			instr->opcode = OPC_LDC;
			instr->operands[0].type = GPREG;
			instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
			instr->operands[1].type = CTRLREG;
			instr->operands[1].regA = cr2id[n];
			}
		else {
			instr->opcode = OPC_LDC;
			instr->operands[0].type = GPREG;
			instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
			instr->operands[1].type = BANKREG;
			instr->operands[1].regA = (SH4_REGISTER)(R0_BANK0 + n);
			}
	}

	// 0100mmmm00010111 "ldc.l @Rm+,gbr"
	else if((insword & 0xf0ff) == 0x4017) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = GBR;
		instr->operands_n = 2;
	}

	// 0100mmmm1nnn0111 "ldc.l @Rm+,Rn_bank"
	else if((insword & 0xf08f) == 0x4087) {
		uint16_t m = (insword & 0xf00)>>8;
		uint16_t n = (insword & 0x70)>>4;
		instr->opcode = OPC_LDC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = BANKREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0_BANK0 + n);
		instr->operands_n = 2;
	}

	// 0100mmmm01000111 "ldc.l @Rm+,spc"
	else if((insword & 0xf0ff) == 0x4047) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(SPC);
		instr->operands_n = 2;
	}

	// 0100mmmm00000111 "ldc.l @Rm+,sr"
	else if((insword & 0xf0ff) == 0x4007) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(SR);
		instr->operands_n = 2;
	}

	// 0100mmmm00110111 "ldc.l @Rm+,ssr"
	else if((insword & 0xf0ff) == 0x4037) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(SSR);
		instr->operands_n = 2;
	}

	// MANUAL
	// 0100mmmm00110110 "ldc.l @Rm+,sgr"
	else if((insword & 0xf0ff) == 0x4036) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(SGR);
		instr->operands_n = 2;
	}

	// 0100mmmm00100111 "ldc.l @Rm+,vbr"
	else if((insword & 0xf0ff) == 0x4027) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = (SH4_REGISTER)(VBR);
		instr->operands_n = 2;
	}

	// MANUAL
	// 0100mmmmnnnn0111 "ldc.l @Rm+,vbr"
	else if((insword & 0xf00f) == 0x4007) {
		uint16_t m = (insword & 0xf00)>>8;
		uint16_t n = (insword & 0xf0)>>4;
		instr->operands_n = 2;
		if(n < 5) {
			instr->opcode = OPC_LDC;
			instr->length_suffix = LEN_SUFFIX_L;
			instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
			instr->operands[0].type = DEREF_REG;
			instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
			instr->operands[1].type = CTRLREG;
			instr->operands[1].regA = cr2id[n];
			}
		else {
			instr->opcode = OPC_LDC;
			instr->length_suffix = LEN_SUFFIX_L;
			instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
			instr->operands[0].type = DEREF_REG;
			instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
			instr->operands[1].type = BANKREG;
			instr->operands[1].regA = (SH4_REGISTER)(R0_BANK0 + n);
			}
	}

	// 0100mmmm01101010 "lds Rm,fpscr"
	else if((insword & 0xf0ff) == 0x406a) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(FPSCR);
		instr->operands_n = 2;
	}

	// 0100mmmm01011010 "lds Rm,fpul"
	else if((insword & 0xf0ff) == 0x405a) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(FPUL);
		instr->operands_n = 2;
	}

	// 0100mmmm00001010 "lds Rm,mach"
	else if((insword & 0xf0ff) == 0x400a) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(MACH);
		instr->operands_n = 2;
	}

	// 0100mmmm00011010 "lds Rm,macl"
	else if((insword & 0xf0ff) == 0x401a) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(MACL);
		instr->operands_n = 2;
	}

	// 0100mmmm00101010 "lds Rm,pr"
	else if((insword & 0xf0ff) == 0x402a) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(PR);
		instr->operands_n = 2;
	}

	// 0100mmmm01100110 "lds.l @Rm+,fpscr"
	else if((insword & 0xf0ff) == 0x4066) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDS;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(FPSCR);
		instr->operands_n = 2;
	}

	// 0100mmmm01010110 "lds.l @Rm+,fpul"
	else if((insword & 0xf0ff) == 0x4056) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDS;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(FPUL);
		instr->operands_n = 2;
	}

	// 0100mmmm00000110 "lds.l @Rm+,mach"
	else if((insword & 0xf0ff) == 0x4006) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDS;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(MACH);
		instr->operands_n = 2;
	}

	// 0100mmmm00010110 "lds.l @Rm+,macl"
	else if((insword & 0xf0ff) == 0x4016) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDS;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(MACL);
		instr->operands_n = 2;
	}

	// 0100mmmm00100110 "lds.l @Rm+,pr"
	else if((insword & 0xf0ff) == 0x4026) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_LDS;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = (SH4_REGISTER)(PR);
		instr->operands_n = 2;
	}

	// 0000000000111000 "ldtlb"
	else if(insword == 0x38) {
		instr->opcode = OPC_LDTLB;
		instr->operands_n = 0;
	}

	// 0000nnnnmmmm1111 "mac.l @Rm+,@Rn+"
	else if((insword & 0xf00f) == 0xf) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MAC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnnmmmm1111 "mac.w @Rm+,@Rn+"
	else if((insword & 0xf00f) == 0x400f) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MAC;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 1110nnnniiiiiiii "mov #imm,Rn"
	else if((insword & 0xf000) == 0xe000) {
		int16_t i = int8(insword & 0xff);
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm0011 "mov Rm,Rn"
	else if((insword & 0xf00f) == 0x6003) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnnmmmm1100 "mov.b @(r0,Rm),Rn"
	else if((insword & 0xf00f) == 0xc) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 11000100dddddddd "mov.b @(disp,gbr),r0"
	else if((insword & 0xff00) == 0xc400) {
		uint16_t d = insword & 0xff;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = DEREF_REG_IMM;
		instr->operands[0].regA = GBR;
		instr->operands[0].displacement = d;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}

	// 10000100mmmmdddd "mov.b @(disp,Rm),r0"
	else if((insword & 0xff00) == 0x8400) {
		uint16_t d = insword & 0xf;
		uint16_t m = (insword & 0xf0)>>4;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = DEREF_REG_IMM;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[0].displacement = d;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm0100 "mov.b @Rm+,Rn"
	else if((insword & 0xf00f) == 0x6004) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm0000 "mov.b @Rm,Rn"
	else if((insword & 0xf00f) == 0x6000) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 11000000dddddddd "mov.b r0,@(disp,gbr)"
	else if((insword & 0xff00) == 0xc000) {
		uint16_t d = insword & 0xff;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG_IMM;
		instr->operands[1].regA = GBR;
		instr->operands[1].displacement = d;
		instr->operands_n = 2;
	}

	// 10000000nnnndddd "mov.b r0,@(disp,Rn)"
	else if((insword & 0xff00) == 0x8000) {
		uint16_t d = insword & 0xf;
		uint16_t n = (insword & 0xf0)>>4;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG_IMM;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands[1].displacement = d;
		instr->operands_n = 2;
	}

	// 0000nnnnmmmm0100 "mov.b Rm,@(r0,Rn)"
	else if((insword & 0xf00f) == 0x4) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm0100 "mov.b Rm,@-Rn"
	else if((insword & 0xf00f) == 0x2004) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm0000 "mov.b Rm,@Rn"
	else if((insword & 0xf00f) == 0x2000) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnnmmmm1110 "mov.l @(r0,Rm),Rn"
	else if((insword & 0xf00f) == 0xe) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 11000110dddddddd "mov.l @(disp,gbr),r0"
	else if((insword & 0xff00) == 0xc600) {
		uint16_t d = insword & 0xff;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = DEREF_REG_IMM;
		instr->operands[0].regA = GBR;
		instr->operands[0].displacement = 4*d;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}

	// 1101nnnndddddddd "mov.l @(disp,PC),Rn"
	else if((insword & 0xf000) == 0xd000) {
		uint16_t n = (insword & 0xf00)>>8;
		uint8_t disp = insword & 0xFF;
		uint64_t eaddr = (addr & 0xFFFFFFFFC) + 4 + 4*disp;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = eaddr;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0101nnnnmmmmdddd "mov.l @(disp,Rm),Rn"
	else if((insword & 0xf000) == 0x5000) {
		uint16_t d = insword & 0xf;
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = DEREF_REG_IMM;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[0].displacement = 4*d;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm0110 "mov.l @Rm+,Rn"
	else if((insword & 0xf00f) == 0x6006) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm0010 "mov.l @Rm,Rn"
	else if((insword & 0xf00f) == 0x6002) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 11000010dddddddd "mov.l r0,@(disp,gbr)"
	else if((insword & 0xff00) == 0xc200) {
		uint16_t d = insword & 0xff;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG_IMM;
		instr->operands[1].regA = GBR;
		instr->operands[1].displacement = 4*d;
		instr->operands_n = 2;
	}

	// 0000nnnnmmmm0110 "mov.l Rm,@(r0,Rn)"
	else if((insword & 0xf00f) == 0x6) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0001nnnnmmmmdddd "mov.l Rm,@(disp,Rn)"
	else if((insword & 0xf000) == 0x1000) {
		uint16_t d = insword & 0xf;
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = DEREF_REG_IMM;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands[1].displacement = 4*d;
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm0110 "mov.l Rm,@-Rn"
	else if((insword & 0xf00f) == 0x2006) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm0010 "mov.l Rm,@Rn"
	else if((insword & 0xf00f) == 0x2002) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnnmmmm1101 "mov.w @(r0,Rm),Rn"
	else if((insword & 0xf00f) == 0xd) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 11000101dddddddd "mov.w @(disp,gbr),r0"
	else if((insword & 0xff00) == 0xc500) {
		uint16_t d = insword & 0xff;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = DEREF_REG_IMM;
		instr->operands[0].regA = GBR;
		instr->operands[0].displacement = 2*d;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}

	// 1001nnnndddddddd "mov.w dddddddd,Rn"
	else if((insword & 0xf000) == 0x9000) {
		int16_t disp = insword & 0xff;
		int64_t eaddr = (addr+4) + 2*disp;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = eaddr;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 10000101mmmmdddd "mov.w @(disp,Rm),r0"
	else if((insword & 0xff00) == 0x8500) {
		uint16_t d = insword & 0xf;
		uint16_t m = (insword & 0xf0)>>4;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = DEREF_REG_IMM;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[0].displacement = 2*d;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm0101 "mov.w @Rm+,Rn"
	else if((insword & 0xf00f) == 0x6005) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].flags |= SH4_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm0001 "mov.w @Rm,Rn"
	else if((insword & 0xf00f) == 0x6001) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 11000001dddddddd "mov.w r0,@(disp,gbr)"
	else if((insword & 0xff00) == 0xc100) {
		uint16_t d = insword & 0xff;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG_IMM;
		instr->operands[1].regA = GBR;
		instr->operands[1].displacement = 2*d;
		instr->operands_n = 2;
	}

	// 10000001nnnndddd "mov.w r0,@(disp,Rn)"
	else if((insword & 0xff00) == 0x8100) {
		uint16_t d = insword & 0xf;
		uint16_t n = (insword & 0xf0)>>4;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG_IMM;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands[1].displacement = 2*d;
		instr->operands_n = 2;
	}

	// 0000nnnnmmmm0101 "mov.w Rm,@(r0,Rn)"
	else if((insword & 0xf00f) == 0x5) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm0101 "mov.w Rm,@-Rn"
	else if((insword & 0xf00f) == 0x2005) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm0001 "mov.w Rm,@Rn"
	else if((insword & 0xf00f) == 0x2001) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOV;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 11000111dddddddd "mova @(disp,PC),r0"
	else if((insword & 0xff00) == 0xc700) {
		uint8_t disp = insword & 0xFF;
		uint64_t eaddr = (addr & 0xFFFFFFFFC) + 4 + 4*disp;
		instr->opcode = OPC_MOVA;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = eaddr;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}

	// 0000nnnn11000011 "movca.l r0,@Rn"
	else if((insword & 0xf0ff) == 0xc3) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOVCA;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn00101001 "movt Rn"
	else if((insword & 0xf0ff) == 0x29) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MOVT;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0000nnnnmmmm0111 "mul.l Rm,Rn"
	else if((insword & 0xf00f) == 0x7) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MUL;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm1111 "muls.w Rm,Rn"
	else if((insword & 0xf00f) == 0x200f) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MULS;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm1110 "mulu.w Rm,Rn"
	else if((insword & 0xf00f) == 0x200e) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_MULU;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm1011 "neg Rm,Rn"
	else if((insword & 0xf00f) == 0x600b) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_NEG;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm1010 "negc Rm,Rn"
	else if((insword & 0xf00f) == 0x600a) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_NEGC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000000000001001 "nop"
	else if(insword == 0x9) {
		instr->opcode = OPC_NOP;
		instr->operands_n = 0;
	}

	// 0110nnnnmmmm0111 "not Rm,Rn"
	else if((insword & 0xf00f) == 0x6007) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_NOT;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn10010011 "ocbi @Rn"
	else if((insword & 0xf0ff) == 0x93) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_OCBI;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0000nnnn10100011 "ocbp @Rn"
	else if((insword & 0xf0ff) == 0xa3) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_OCBP;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0000nnnn10110011 "ocbwb @Rn"
	else if((insword & 0xf0ff) == 0xb3) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_OCBWB;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 11001011iiiiiiii "or #imm,r0"
	else if((insword & 0xff00) == 0xcb00) {
		int16_t i = int8(insword & 0xff);
		instr->opcode = OPC_OR;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm1011 "or Rm,Rn"
	else if((insword & 0xf00f) == 0x200b) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_OR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 11001111iiiiiiii "or.b #imm,@(r0,gbr)"
	else if((insword & 0xff00) == 0xcf00) {
		int16_t i = int8(insword & 0xff);
		instr->opcode = OPC_OR;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = GBR;
		instr->operands_n = 2;
	}

	// 0000nnnn10000011 "pref @Rn"
	else if((insword & 0xf0ff) == 0x83) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_PREF;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00100100 "rotcl Rn"
	else if((insword & 0xf0ff) == 0x4024) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_ROTCL;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00100101 "rotcr Rn"
	else if((insword & 0xf0ff) == 0x4025) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_ROTCR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00000100 "rotl Rn"
	else if((insword & 0xf0ff) == 0x4004) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_ROTL;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00000101 "rotr Rn"
	else if((insword & 0xf0ff) == 0x4005) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_ROTR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0000000000101011 "rte"
	else if(insword == 0x2b) {
		instr->opcode = OPC_RTE;
		instr->operands_n = 0;
	}

	// 0000000000001011 "rts"
	else if(insword == 0xb) {
		instr->opcode = OPC_RTS;
		instr->operands_n = 0;
	}

	// 0000000000011000 "sett"
	else if(insword == 0x18) {
		instr->opcode = OPC_SETT;
		instr->operands_n = 0;
	}

	// 0000000001011000 "sett"
	else if(insword == 0x58) {
		instr->opcode = OPC_SETS;
		instr->operands_n = 0;
	}

	// 0100nnnnmmmm1100 "shad Rm,Rn"
	else if((insword & 0xf00f) == 0x400c) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SHAD;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn00100000 "shal Rn"
	else if((insword & 0xf0ff) == 0x4020) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SHAL;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00100001 "shar Rn"
	else if((insword & 0xf0ff) == 0x4021) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SHAR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnnmmmm1101 "shld Rm,Rn"
	else if((insword & 0xf00f) == 0x400d) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SHLD;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn00000000 "shll Rn"
	else if((insword & 0xf0ff) == 0x4000) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SHLL;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00101000 "shll16 Rn"
	else if((insword & 0xf0ff) == 0x4028) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SHLL16;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00001000 "shll2 Rn"
	else if((insword & 0xf0ff) == 0x4008) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SHLL2;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00011000 "shll8 Rn"
	else if((insword & 0xf0ff) == 0x4018) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SHLL8;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00000001 "shlr Rn"
	else if((insword & 0xf0ff) == 0x4001) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SHLR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00101001 "shlr16 Rn"
	else if((insword & 0xf0ff) == 0x4029) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SHLR16;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00001001 "shlr2 Rn"
	else if((insword & 0xf0ff) == 0x4009) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SHLR2;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0100nnnn00011001 "shlr8 Rn"
	else if((insword & 0xf0ff) == 0x4019) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SHLR8;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 0000000000011011 "sleep"
	else if(insword == 0x1b) {
		instr->opcode = OPC_SLEEP;
		instr->operands_n = 0;
	}

	// 0000nnnn1mmm0010 "stc Rn,Rm_bank"
	else if((insword & 0xF08F) == 0x0082) {
		uint16_t n = (insword & 0xf00)>>8;
		uint16_t m = (insword & 0x70)>>4;
		instr->opcode = OPC_STC;
		instr->operands[0].type = BANKREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0_BANK0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn11111010 "stc dbr,Rn"
	else if((insword & 0xf0ff) == 0xfa) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = (SH4_REGISTER)(DBR);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn00010010 "stc gbr,Rn"
	else if((insword & 0xf0ff) == 0x12) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = GBR;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn00111010 "stc sgr,Rn"
	else if((insword & 0xf0ff) == 0x3a) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = (SH4_REGISTER)(SGR);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn01000010 "stc spc,Rn"
	else if((insword & 0xf0ff) == 0x42) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = (SH4_REGISTER)(SPC);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn00000010 "stc sr,Rn"
	else if((insword & 0xf0ff) == 0x2) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = (SH4_REGISTER)(SR);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn00110010 "stc ssr,Rn"
	else if((insword & 0xf0ff) == 0x32) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = (SH4_REGISTER)(SSR);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn00100010 "stc vbr,Rn"
	else if((insword & 0xf0ff) == 0x22) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = (SH4_REGISTER)(VBR);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// MANUAL
	// 0000nnnn1mmm0010 "stc Rm_bank,Rn"
	else if((insword & 0xf08f) == 0x82) {
		uint16_t m = (insword & 0x70)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->operands[0].type = BANKREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0_BANK0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// MANUAL
	// 0000nnnn0mmm0010 "stc Rm_bank,Rn"
	else if((insword & 0xf08f) == 0x2) {
		uint16_t m = (insword & 0x70)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->operands_n = 2;
		if(m < 5) {
			instr->opcode = OPC_STC;
			instr->operands[0].type = GPREG;
			instr->operands[0].regA = (SH4_REGISTER)(R0_BANK0 + n);
			instr->operands[1].type = GPREG;
			instr->operands[1].regA = (SH4_REGISTER)(R0 + cr2id[m]);
			}
		else {
			instr->opcode = OPC_STC;
			instr->operands[0].type = BANKREG;
			instr->operands[0].regA = (SH4_REGISTER)(R0_BANK0 + m);
			instr->operands[1].type = GPREG;
			instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
			}
	}

	// 0100nnnn11110010 "stc.l dbr,@-Rn"
	else if((insword & 0xf0ff) == 0x40f2) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = (SH4_REGISTER)(DBR);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn00010011 "stc.l gbr,@-Rn"
	else if((insword & 0xf0ff) == 0x4013) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = GBR;
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn1mmm0011 "stc.l Rm_bank,@-Rn"
	else if((insword & 0xf08f) == 0x4083) {
		uint16_t m = (insword & 0x70)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = BANKREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0_BANK0 + m);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// MANUAL
	// 0100nnnn0mmm0011 "stc.l Rm_bank,@-Rn"
	else if((insword & 0xf08f) == 0x4003) {
		uint16_t m = (insword & 0x70)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->operands_n = 2;
		if(m < 5) {
			instr->opcode = OPC_STC;
			instr->length_suffix = LEN_SUFFIX_L;
			instr->operands[0].type = CTRLREG;
			instr->operands[0].regA = (SH4_REGISTER)(cr2id[m]);
			instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
			instr->operands[1].type = DEREF_REG;
			instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
			}
		else {
			instr->opcode = OPC_STC;
			instr->length_suffix = LEN_SUFFIX_L;
			instr->operands[0].type = BANKREG;
			instr->operands[0].regA = (SH4_REGISTER)(R0_BANK0 + m);
			instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
			instr->operands[1].type = DEREF_REG;
			instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
			}
	}

	// 0100nnnn00110010 "stc.l sgr,@-Rn"
	else if((insword & 0xf0ff) == 0x4032) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = (SH4_REGISTER)(SGR);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn01000011 "stc.l spc,@-Rn"
	else if((insword & 0xf0ff) == 0x4043) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = (SH4_REGISTER)(SPC);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn00000011 "stc.l sr,@-Rn"
	else if((insword & 0xf0ff) == 0x4003) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = (SH4_REGISTER)(SR);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn00110011 "stc.l ssr,@-Rn"
	else if((insword & 0xf0ff) == 0x4033) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = (SH4_REGISTER)(SSR);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn00100011 "stc.l vbr,@-Rn"
	else if((insword & 0xf0ff) == 0x4023) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STC;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = (SH4_REGISTER)(VBR);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn01101010 "sts fpscr,Rn"
	else if((insword & 0xf0ff) == 0x6a) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(FPSCR);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn01011010 "sts fpul,Rn"
	else if((insword & 0xf0ff) == 0x5a) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(FPUL);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn00001010 "sts mach,Rn"
	else if((insword & 0xf0ff) == 0xa) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(MACH);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn00011010 "sts macl,Rn"
	else if((insword & 0xf0ff) == 0x1a) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(MACL);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0000nnnn00101010 "sts pr,Rn"
	else if((insword & 0xf0ff) == 0x2a) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(PR);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn01100010 "sts.l fpscr,@-Rn"
	else if((insword & 0xf0ff) == 0x4062) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STS;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(FPSCR);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn01010010 "sts.l fpul,@-Rn"
	else if((insword & 0xf0ff) == 0x4052) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STS;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(FPUL);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn00000010 "sts.l mach,@-Rn"
	else if((insword & 0xf0ff) == 0x4002) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STS;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(MACH);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn00010010 "sts.l macl,@-Rn"
	else if((insword & 0xf0ff) == 0x4012) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STS;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(MACL);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn00100010 "sts.l pr,@-Rn"
	else if((insword & 0xf0ff) == 0x4022) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_STS;
		instr->length_suffix = LEN_SUFFIX_L;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(PR);
		instr->operands[1].flags |= SH4_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm1000 "sub Rm,Rn"
	else if((insword & 0xf00f) == 0x3008) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SUB;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm1010 "subc Rm,Rn"
	else if((insword & 0xf00f) == 0x300a) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SUBC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0011nnnnmmmm1011 "subv Rm,Rn"
	else if((insword & 0xf00f) == 0x300b) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SUBV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm1000 "swap.b Rm,Rn"
	else if((insword & 0xf00f) == 0x6008) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SWAP;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0110nnnnmmmm1001 "swap.w Rm,Rn"
	else if((insword & 0xf00f) == 0x6009) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_SWAP;
		instr->length_suffix = LEN_SUFFIX_W;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 0100nnnn00011011 "tas.b @Rn"
	else if((insword & 0xf0ff) == 0x401b) {
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_TAS;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 1;
	}

	// 11000011iiiiiiii "trapa #imm"
	else if((insword & 0xff00) == 0xc300) {
		int16_t i = int8(insword & 0xff);
		instr->opcode = OPC_TRAPA;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands_n = 1;
	}

	// 11001000iiiiiiii "tst #imm,r0"
	else if((insword & 0xff00) == 0xc800) {
		int16_t i = int8(insword & 0xff);
		instr->opcode = OPC_TST;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm1000 "tst Rm,Rn"
	else if((insword & 0xf00f) == 0x2008) {
		int16_t m = (insword & 0xf0)>>4;
		int16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_TST;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 11001100iiiiiiii "tst.b #imm,@(r0,gbr)"
	else if((insword & 0xff00) == 0xcc00) {
		int16_t i = int8(insword & 0xff);
		instr->opcode = OPC_TST;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = GBR;
		instr->operands_n = 2;
	}

	// 11001010iiiiiiii "xor #imm,r0"
	else if((insword & 0xff00) == 0xca00) {
		int16_t i = int8(insword & 0xff);
		instr->opcode = OPC_XOR;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm1010 "xor Rm,Rn"
	else if((insword & 0xf00f) == 0x200a) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_XOR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 11001110iiiiiiii "xor.b #imm,@(r0,gbr)"
	else if((insword & 0xff00) == 0xce00) {
		int16_t i = int8(insword & 0xff);
		instr->opcode = OPC_XOR;
		instr->length_suffix = LEN_SUFFIX_B;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = GBR;
		instr->operands_n = 2;
	}

	// 0010nnnnmmmm1101 "xtrct Rm,Rn"
	else if((insword & 0xf00f) == 0x200d) {
		uint16_t m = (insword & 0xf0)>>4;
		uint16_t n = (insword & 0xf00)>>8;
		instr->opcode = OPC_XTRCT;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SH4_REGISTER)(R0 + m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SH4_REGISTER)(R0 + n);
		instr->operands_n = 2;
	}

	// 1111nnnn01111101 "fsrra Rn"
	else if((insword & 0xf0ff) == 0xf07d) {
		uint16_t m = (insword & 0xf00)>>8;
		instr->opcode = OPC_FSRRA;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SH4_REGISTER)(FR0 + m);
		instr->operands_n = 1;
	}

	// 1111nnn011111101 "fsca fpul, drn"
	else if((insword & 0xf1ff) == 0xf0fd) {
		uint16_t m = (insword & 0xE00)>>9;
		instr->opcode = OPC_FSCA;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = (SH4_REGISTER)(FPUL);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SH4_REGISTER)(DR0 + 2*m);
		instr->operands_n = 2;
	}

	// didn't disassemble!
	else {
		instr->opcode = OPC_ERROR;
		instr->operands_n = 0;
		rc = -1;
	}

	return rc;
}

