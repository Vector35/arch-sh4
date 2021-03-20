/* GENERATED FILE */
#include "decode.h"
#include "decode_support.h"
int decode_spec(context *ctx, Instruction *instr)
{
	uint16_t insword = ctx->insword;
	// 0110nnnnmmmm0011 "mov Rm,Rn"
	if((insword & 0xf00f)==0x6003 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) {
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 1110nnnniiiiiiii "mov #imm,Rn"
	else if((insword & 0xf000)==0xe000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0000nnnniiii0000 "movi20 #imm20,Rn"
	else if((insword & 0xf00f) && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVI20;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0000nnnniiii0001 "movi20s #imm20,Rn"
	else if((insword & 0xf00f)==1 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVI20S;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 11000111dddddddd "mova @(disp,PC),R0"
	else if((insword & 0xff00)==0xc700 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_MOVA;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = displ2ea(4, d, ctx->address, 3);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}
	// 1001nnnndddddddd "mov.w @(disp,PC),Rn"
	else if((insword & 0xf000)==0x9000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = displ2ea(2, d, ctx->address, 0);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 1101nnnndddddddd "mov.l @(disp,PC),Rn"
	else if((insword & 0xf000)==0xd000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = displ2ea(4, d, ctx->address, 3);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0110nnnnmmmm0000 "mov.b @Rm,Rn"
	else if((insword & 0xf00f)==0x6000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0110nnnnmmmm0001 "mov.w @Rm,Rn"
	else if((insword & 0xf00f)==0x6001 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0110nnnnmmmm0010 "mov.l @Rm,Rn"
	else if((insword & 0xf00f)==0x6002 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0010nnnnmmmm0000 "mov.b Rm,@Rn"
	else if((insword & 0xf00f)==0x2000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0010nnnnmmmm0001 "mov.w Rm,@Rn"
	else if((insword & 0xf00f)==0x2001 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0010nnnnmmmm0010 "mov.l Rm,@Rn"
	else if((insword & 0xf00f)==0x2002 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0110nnnnmmmm0100 "mov.b @Rm+,Rn"
	else if((insword & 0xf00f)==0x6004 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0110nnnnmmmm0101 "mov.w @Rm+,Rn"
	else if((insword & 0xf00f)==0x6005 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0110nnnnmmmm0110 "mov.l @Rm+,Rn"
	else if((insword & 0xf00f)==0x6006 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0010nnnnmmmm0100 "mov.b Rm,@-Rn"
	else if((insword & 0xf00f)==0x2004 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0010nnnnmmmm0101 "mov.w Rm,@-Rn"
	else if((insword & 0xf00f)==0x2005 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0010nnnnmmmm0110 "mov.l Rm,@-Rn"
	else if((insword & 0xf00f)==0x2006 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm11001011 "mov.b @-Rm,R0"
	else if((insword & 0xf0ff)==0x40cb && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0100mmmm11011011 "mov.w @-Rm,R0"
	else if((insword & 0xf0ff)==0x40db && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0100mmmm11101011 "mov.l @-Rm,R0"
	else if((insword & 0xf0ff)==0x40eb && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100nnnn10001011 "mov.b R0,@Rn+"
	else if((insword & 0xf0ff)==0x408b && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0100nnnn10011011 "mov.w R0,@Rn+"
	else if((insword & 0xf0ff)==0x409b && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0100nnnn10101011 "mov.l R0,@Rn+"
	else if((insword & 0xf0ff)==0x40ab && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 10000100mmmmdddd "mov.b @(disp,Rm),R0"
	else if((insword & 0xff00)==0x8400 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xf)>>0;
		uint8_t m = (insword & 0xf0)>>4;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = DEREF_REG_IMM;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[0].displacement = 1*d;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0011nnnnmmmm0001 "mov.b @(disp12,Rm),Rn"
	else if((insword & 0xf00f)==0x3001 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		// unsupported operand: @(disp12,Rm)
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0011nnnnmmmm0001 "movu.b @(disp12,Rm),Rn"
	else if((insword & 0xf00f)==0x3001 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVU;
		// unsupported operand: @(disp12,Rm)
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 10000101mmmmdddd "mov.w @(disp,Rm),R0"
	else if((insword & 0xff00)==0x8500 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xf)>>0;
		uint8_t m = (insword & 0xf0)>>4;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = DEREF_REG_IMM;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[0].displacement = 2*d;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0011nnnnmmmm0001 "mov.w @(disp12,Rm),Rn"
	else if((insword & 0xf00f)==0x3001 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		// unsupported operand: @(disp12,Rm)
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0011nnnnmmmm0001 "movu.w @(disp12,Rm),Rn"
	else if((insword & 0xf00f)==0x3001 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVU;
		// unsupported operand: @(disp12,Rm)
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0101nnnnmmmmdddd "mov.l @(disp,Rm),Rn"
	else if((insword & 0xf000)==0x5000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xf)>>0;
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = DEREF_REG_IMM;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[0].displacement = 4*d;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0011nnnnmmmm0001 "mov.l @(disp12,Rm),Rn"
	else if((insword & 0xf00f)==0x3001 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		// unsupported operand: @(disp12,Rm)
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 10000000nnnndddd "mov.b R0,@(disp,Rn)"
	else if((insword & 0xff00)==0x8000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xf)>>0;
		uint8_t n = (insword & 0xf0)>>4;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG_IMM;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands[1].displacement = 1*d;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0011nnnnmmmm0001 "mov.b Rm,@(disp12,Rn)"
	else if((insword & 0xf00f)==0x3001 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 10000001nnnndddd "mov.w R0,@(disp,Rn)"
	else if((insword & 0xff00)==0x8100 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xf)>>0;
		uint8_t n = (insword & 0xf0)>>4;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG_IMM;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands[1].displacement = 2*d;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0011nnnnmmmm0001 "mov.w Rm,@(disp12,Rn)"
	else if((insword & 0xf00f)==0x3001 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0001nnnnmmmmdddd "mov.l Rm,@(disp,Rn)"
	else if((insword & 0xf000)==0x1000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xf)>>0;
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = DEREF_REG_IMM;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands[1].displacement = 4*d;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0011nnnnmmmm0001 "mov.l Rm,@(disp12,Rn)"
	else if((insword & 0xf00f)==0x3001 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnnmmmm1100 "mov.b @(R0,Rm),Rn"
	else if((insword & 0xf00f)==0xc && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0000nnnnmmmm1101 "mov.w @(R0,Rm),Rn"
	else if((insword & 0xf00f)==0xd && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0000nnnnmmmm1110 "mov.l @(R0,Rm),Rn"
	else if((insword & 0xf00f)==0xe && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnnmmmm0100 "mov.b Rm,@(R0,Rn)"
	else if((insword & 0xf00f)==4 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0000nnnnmmmm0101 "mov.w Rm,@(R0,Rn)"
	else if((insword & 0xf00f)==5 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0000nnnnmmmm0110 "mov.l Rm,@(R0,Rn)"
	else if((insword & 0xf00f)==6 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 11000100dddddddd "mov.b @(disp,GBR),R0"
	else if((insword & 0xff00)==0xc400 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = DEREF_REG_IMM;
		instr->operands[0].regA = GBR;
		instr->operands[0].displacement = 1*d;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 11000101dddddddd "mov.w @(disp,GBR),R0"
	else if((insword & 0xff00)==0xc500 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = DEREF_REG_IMM;
		instr->operands[0].regA = GBR;
		instr->operands[0].displacement = 2*d;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 11000110dddddddd "mov.l @(disp,GBR),R0"
	else if((insword & 0xff00)==0xc600 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = DEREF_REG_IMM;
		instr->operands[0].regA = GBR;
		instr->operands[0].displacement = 4*d;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 11000000dddddddd "mov.b R0,@(disp,GBR)"
	else if((insword & 0xff00)==0xc000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG_IMM;
		instr->operands[1].regA = GBR;
		instr->operands[1].displacement = 1*d;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 11000001dddddddd "mov.w R0,@(disp,GBR)"
	else if((insword & 0xff00)==0xc100 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG_IMM;
		instr->operands[1].regA = GBR;
		instr->operands[1].displacement = 2*d;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 11000010dddddddd "mov.l R0,@(disp,GBR)"
	else if((insword & 0xff00)==0xc200 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_MOV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG_IMM;
		instr->operands[1].regA = GBR;
		instr->operands[1].displacement = 4*d;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn01110011 "movco.l R0,@Rn"
	else if((insword & 0xf0ff)==0x73 && SUPPORTED(SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVCO;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000mmmm01100011 "movli.l @Rm,R0"
	else if((insword & 0xf0ff)==0x63 && SUPPORTED(SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVLI;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm10101001 "movua.l @Rm,R0"
	else if((insword & 0xf0ff)==0x40a9 && SUPPORTED(SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVUA;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm11101001 "movua.l @Rm+,R0"
	else if((insword & 0xf0ff)==0x40e9 && SUPPORTED(SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVUA;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm11110001 "movml.l Rm,@-R15"
	else if((insword & 0xf0ff)==0x40f1 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVML;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		// unsupported operand: @R15
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100nnnn11110101 "movml.l @R15+,Rn"
	else if((insword & 0xf0ff)==0x40f5 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVML;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		// unsupported operand: @R15
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm11110000 "movmu.l Rm,@-R15"
	else if((insword & 0xf0ff)==0x40f0 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVMU;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		// unsupported operand: @R15
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100nnnn11110100 "movmu.l @R15+,Rn"
	else if((insword & 0xf0ff)==0x40f4 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVMU;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		// unsupported operand: @R15
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn00111001 "movrt Rn"
	else if((insword & 0xf0ff)==0x39 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVRT;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0000nnnn00101001 "movt Rn"
	else if((insword & 0xf0ff)==0x29 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVT;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0000000001101000 "nott"
	else if(insword==0x68 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_NOTT;
		instr->operands_n = 0;
	}
	// 0110nnnnmmmm1000 "swap.b Rm,Rn"
	else if((insword & 0xf00f)==0x6008 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SWAP;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0110nnnnmmmm1001 "swap.w Rm,Rn"
	else if((insword & 0xf00f)==0x6009 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SWAP;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0010nnnnmmmm1101 "xtrct Rm,Rn"
	else if((insword & 0xf00f)==0x200d && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_XTRCT;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0011nnnn0iii1001 "band.b #imm3,@disp12,Rn"
	else if((insword & 0xf08f)==0x3009 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x70)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_BAND;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		// unsupported operand: @disp12
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0011nnnn0iii1001 "bandnot.b #imm3,@(disp12,Rn)"
	else if((insword & 0xf08f)==0x3009 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x70)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_BANDNOT;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0011nnnn0iii1001 "bclr.b #imm3,@(disp12,Rn)"
	else if((insword & 0xf08f)==0x3009 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x70)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_BCLR;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 10000110nnnn0iii "bclr #imm3,Rn"
	else if((insword & 0xff08)==0x8600 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x7)>>0;
		uint8_t n = (insword & 0xf0)>>4;
		instr->operation = SUPERH_BCLR;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0011nnnn0iii1001 "bld.b #imm3,@(disp12,Rn)"
	else if((insword & 0xf08f)==0x3009 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x70)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_BLD;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 10000111nnnn1iii "bld #imm3,Rn"
	else if((insword & 0xff08)==0x8708 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x7)>>0;
		uint8_t n = (insword & 0xf0)>>4;
		instr->operation = SUPERH_BLD;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0011nnnn0iii1001 "bldnot.b #imm3,@(disp12,Rn)"
	else if((insword & 0xf08f)==0x3009 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x70)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_BLDNOT;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0011nnnn0iii1001 "bor.b #imm3,@(disp12,Rn)"
	else if((insword & 0xf08f)==0x3009 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x70)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_BOR;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0011nnnn0iii1001 "bornot.b #imm3,@(disp12,Rn)"
	else if((insword & 0xf08f)==0x3009 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x70)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_BORNOT;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0011nnnn0iii1001 "bset.b #imm3,@(disp12,Rn)"
	else if((insword & 0xf08f)==0x3009 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x70)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_BSET;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 10000110nnnn1iii "bset #imm3,Rn"
	else if((insword & 0xff08)==0x8608 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x7)>>0;
		uint8_t n = (insword & 0xf0)>>4;
		instr->operation = SUPERH_BSET;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0011nnnn0iii1001 "bst.b #imm3,@(disp12,Rn)"
	else if((insword & 0xf08f)==0x3009 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x70)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_BST;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 10000111nnnn0iii "bst #imm3,Rn"
	else if((insword & 0xff08)==0x8700 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x7)>>0;
		uint8_t n = (insword & 0xf0)>>4;
		instr->operation = SUPERH_BST;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0011nnnn0iii1001 "bxor.b #imm3,@(disp12,Rn)"
	else if((insword & 0xf08f)==0x3009 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t i = (insword & 0x70)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_BXOR;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0011nnnnmmmm1100 "add Rm,Rn"
	else if((insword & 0xf00f)==0x300c && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_ADD;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0111nnnniiiiiiii "add #imm,Rn"
	else if((insword & 0xf000)==0x7000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_ADD;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0011nnnnmmmm1110 "addc Rm,Rn"
	else if((insword & 0xf00f)==0x300e && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_ADDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0011nnnnmmmm1111 "addv Rm,Rn"
	else if((insword & 0xf00f)==0x300f && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_ADDV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 10001000iiiiiiii "cmp/eq #imm,R0"
	else if((insword & 0xff00)==0x8800 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		instr->operation = SUPERH_CMP;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
		instr->cond = CMP_COND_EQ;
	}
	// 0011nnnnmmmm0000 "cmp/eq Rm,Rn"
	else if((insword & 0xf00f)==0x3000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_CMP;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->cond = CMP_COND_EQ;
	}
	// 0011nnnnmmmm0010 "cmp/hs Rm,Rn"
	else if((insword & 0xf00f)==0x3002 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_CMP;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->cond = CMP_COND_HS;
	}
	// 0011nnnnmmmm0011 "cmp/ge Rm,Rn"
	else if((insword & 0xf00f)==0x3003 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_CMP;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->cond = CMP_COND_GE;
	}
	// 0011nnnnmmmm0110 "cmp/hi Rm,Rn"
	else if((insword & 0xf00f)==0x3006 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_CMP;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->cond = CMP_COND_HI;
	}
	// 0011nnnnmmmm0111 "cmp/gt Rm,Rn"
	else if((insword & 0xf00f)==0x3007 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_CMP;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->cond = CMP_COND_GT;
	}
	// 0100nnnn00010101 "cmp/pl Rn"
	else if((insword & 0xf0ff)==0x4015 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_CMP;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->cond = CMP_COND_PL;
	}
	// 0100nnnn00010001 "cmp/pz Rn"
	else if((insword & 0xf0ff)==0x4011 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_CMP;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->cond = CMP_COND_PZ;
	}
	// 0010nnnnmmmm1100 "cmp/str Rm,Rn"
	else if((insword & 0xf00f)==0x200c && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_CMP;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->cond = CMP_COND_STR;
	}
	// 0100nnnn10010001 "clips.b Rn"
	else if((insword & 0xf0ff)==0x4091 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_CLIPS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0100nnnn10010101 "clips.w Rn"
	else if((insword & 0xf0ff)==0x4095 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_CLIPS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0100nnnn10000001 "clipu.b Rn"
	else if((insword & 0xf0ff)==0x4081 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_CLIPU;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0100nnnn10000101 "clipu.w Rn"
	else if((insword & 0xf0ff)==0x4085 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_CLIPU;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0010nnnnmmmm0111 "div0s Rm,Rn"
	else if((insword & 0xf00f)==0x2007 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_DIV0S;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0000000000011001 "div0u"
	else if(insword==0x19 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_DIV0U;
		instr->operands_n = 0;
	}
	// 0011nnnnmmmm0100 "div1 Rm,Rn"
	else if((insword & 0xf00f)==0x3004 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_DIV1;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn10010100 "divs R0,Rn"
	else if((insword & 0xf0ff)==0x4094 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_DIVS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn10000100 "divu R0,Rn"
	else if((insword & 0xf0ff)==0x4084 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_DIVU;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0011nnnnmmmm1101 "dmuls.l Rm,Rn"
	else if((insword & 0xf00f)==0x300d && SUPPORTED(SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_DMULS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0011nnnnmmmm0101 "dmulu.l Rm,Rn"
	else if((insword & 0xf00f)==0x3005 && SUPPORTED(SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_DMULU;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100nnnn00010000 "dt Rn"
	else if((insword & 0xf0ff)==0x4010 && SUPPORTED(SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_DT;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0110nnnnmmmm1110 "exts.b Rm,Rn"
	else if((insword & 0xf00f)==0x600e && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_EXTS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0110nnnnmmmm1111 "exts.w Rm,Rn"
	else if((insword & 0xf00f)==0x600f && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_EXTS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0110nnnnmmmm1100 "extu.b Rm,Rn"
	else if((insword & 0xf00f)==0x600c && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_EXTU;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0110nnnnmmmm1101 "extu.w Rm,Rn"
	else if((insword & 0xf00f)==0x600d && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_EXTU;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0000nnnnmmmm1111 "mac.l @Rm+,@Rn+"
	else if((insword & 0xf00f)==0xf && SUPPORTED(SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MAC;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100nnnnmmmm1111 "mac.w @Rm+,@Rn+"
	else if((insword & 0xf00f)==0x400f && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MAC;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0000nnnnmmmm0111 "mul.l Rm,Rn"
	else if((insword & 0xf00f)==7 && SUPPORTED(SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MUL;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100nnnn10000000 "mulr R0,Rn"
	else if((insword & 0xf0ff)==0x4080 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MULR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0010nnnnmmmm1111 "muls.w Rm,Rn"
	else if((insword & 0xf00f)==0x200f && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MULS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0010nnnnmmmm1110 "mulu.w Rm,Rn"
	else if((insword & 0xf00f)==0x200e && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MULU;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 0110nnnnmmmm1011 "neg Rm,Rn"
	else if((insword & 0xf00f)==0x600b && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_NEG;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0110nnnnmmmm1010 "negc Rm,Rn"
	else if((insword & 0xf00f)==0x600a && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_NEGC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0011nnnnmmmm1000 "sub Rm,Rn"
	else if((insword & 0xf00f)==0x3008 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SUB;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0011nnnnmmmm1010 "subc Rm,Rn"
	else if((insword & 0xf00f)==0x300a && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SUBC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0011nnnnmmmm1011 "subv Rm,Rn"
	else if((insword & 0xf00f)==0x300b && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SUBV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0010nnnnmmmm1001 "and Rm,Rn"
	else if((insword & 0xf00f)==0x2009 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_AND;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 11001001iiiiiiii "and #imm,R0"
	else if((insword & 0xff00)==0xc900 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		instr->operation = SUPERH_AND;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}
	// 11001101iiiiiiii "and.b #imm,@(R0,GBR)"
	else if((insword & 0xff00)==0xcd00 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		instr->operation = SUPERH_AND;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = GBR;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0110nnnnmmmm0111 "not Rm,Rn"
	else if((insword & 0xf00f)==0x6007 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_NOT;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0010nnnnmmmm1011 "or Rm,Rn"
	else if((insword & 0xf00f)==0x200b && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_OR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 11001011iiiiiiii "or #imm,R0"
	else if((insword & 0xff00)==0xcb00 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		instr->operation = SUPERH_OR;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}
	// 11001111iiiiiiii "or.b #imm,@(R0,GBR)"
	else if((insword & 0xff00)==0xcf00 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		instr->operation = SUPERH_OR;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = GBR;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0100nnnn00011011 "tas.b @Rn"
	else if((insword & 0xf0ff)==0x401b && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_TAS;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0010nnnnmmmm1000 "tst Rm,Rn"
	else if((insword & 0xf00f)==0x2008 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_TST;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 11001000iiiiiiii "tst #imm,R0"
	else if((insword & 0xff00)==0xc800 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		instr->operation = SUPERH_TST;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}
	// 11001100iiiiiiii "tst.b #imm,@(R0,GBR)"
	else if((insword & 0xff00)==0xcc00 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		instr->operation = SUPERH_TST;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = GBR;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0010nnnnmmmm1010 "xor Rm,Rn"
	else if((insword & 0xf00f)==0x200a && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_XOR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 11001010iiiiiiii "xor #imm,R0"
	else if((insword & 0xff00)==0xca00 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		instr->operation = SUPERH_XOR;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}
	// 11001110iiiiiiii "xor.b #imm,@(R0,GBR)"
	else if((insword & 0xff00)==0xce00 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		instr->operation = SUPERH_XOR;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = GBR;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_B;
	}
	// 0100nnnn00100100 "rotcl Rn"
	else if((insword & 0xf0ff)==0x4024 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_ROTCL;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn00100101 "rotcr Rn"
	else if((insword & 0xf0ff)==0x4025 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_ROTCR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn00000100 "rotl Rn"
	else if((insword & 0xf0ff)==0x4004 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_ROTL;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn00000101 "rotr Rn"
	else if((insword & 0xf0ff)==0x4005 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_ROTR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnnmmmm1100 "shad Rm,Rn"
	else if((insword & 0xf00f)==0x400c && SUPPORTED(SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SHAD;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn00100000 "shal Rn"
	else if((insword & 0xf0ff)==0x4020 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SHAL;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn00100001 "shar Rn"
	else if((insword & 0xf0ff)==0x4021 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SHAR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnnmmmm1101 "shld Rm,Rn"
	else if((insword & 0xf00f)==0x400d && SUPPORTED(SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SHLD;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn00000000 "shll Rn"
	else if((insword & 0xf0ff)==0x4000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SHLL;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn00001000 "shll2 Rn"
	else if((insword & 0xf0ff)==0x4008 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SHLL2;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn00011000 "shll8 Rn"
	else if((insword & 0xf0ff)==0x4018 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SHLL8;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn00101000 "shll16 Rn"
	else if((insword & 0xf0ff)==0x4028 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SHLL16;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn00000001 "shlr Rn"
	else if((insword & 0xf0ff)==0x4001 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SHLR;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn00001001 "shlr2 Rn"
	else if((insword & 0xf0ff)==0x4009 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SHLR2;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn00011001 "shlr8 Rn"
	else if((insword & 0xf0ff)==0x4019 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SHLR8;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn00101001 "shlr16 Rn"
	else if((insword & 0xf0ff)==0x4029 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SHLR16;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 10001011dddddddd "bf label"
	else if((insword & 0xff00)==0x8b00 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_BF;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = displ2ea(2, int8(d), ctx->address, 0);
		instr->operands_n = 1;
	}
	// 10001111dddddddd "bf/s label"
	else if((insword & 0xff00)==0x8f00 && SUPPORTED(SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_BF;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = displ2ea(2, int8(d), ctx->address, 0);
		instr->operands_n = 1;
		instr->delay_slot = true;
	}
	// 10001001dddddddd "bt label"
	else if((insword & 0xff00)==0x8900 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_BT;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = displ2ea(2, int8(d), ctx->address, 0);
		instr->operands_n = 1;
	}
	// 10001101dddddddd "bt/s label"
	else if((insword & 0xff00)==0x8d00 && SUPPORTED(SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_BT;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = displ2ea(2, int8(d), ctx->address, 0);
		instr->operands_n = 1;
		instr->delay_slot = true;
	}
	// 1010dddddddddddd "bra label"
	else if((insword & 0xf000)==0xa000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint16_t d = (insword & 0xfff)>>0;
		instr->operation = SUPERH_BRA;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = displ2ea(2, int12(d), ctx->address, 0);
		instr->operands_n = 1;
		instr->delay_slot = true;
	}
	// 0000mmmm00100011 "braf Rm"
	else if((insword & 0xf0ff)==0x23 && SUPPORTED(SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_BRAF;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands_n = 1;
		instr->delay_slot = true;
	}
	// 1011dddddddddddd "bsr label"
	else if((insword & 0xf000)==0xb000 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint16_t d = (insword & 0xfff)>>0;
		instr->operation = SUPERH_BSR;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = displ2ea(2, int12(d), ctx->address, 0);
		instr->operands_n = 1;
		instr->delay_slot = true;
	}
	// 0000mmmm00000011 "bsrf Rm"
	else if((insword & 0xf0ff)==3 && SUPPORTED(SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_BSRF;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands_n = 1;
		instr->delay_slot = true;
	}
	// 0100mmmm00101011 "jmp @Rm"
	else if((insword & 0xf0ff)==0x402b && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_JMP;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands_n = 1;
		instr->delay_slot = true;
	}
	// 0100mmmm00001011 "jsr @Rm"
	else if((insword & 0xf0ff)==0x400b && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_JSR;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands_n = 1;
		instr->delay_slot = true;
	}
	// 0100mmmm01001011 "jsr/n @Rm"
	else if((insword & 0xf0ff)==0x404b && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_JSR;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands_n = 1;
	}
	// 10000011dddddddd "jsr/n @@(disp8,TBR)"
	else if((insword & 0xff00)==0x8300 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_JSR;
		// unsupported operand: @@(disp8,TBR)
		instr->operands_n = 0;
	}
	// 0000000000001011 "rts"
	else if(insword==0xb && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_RTS;
		instr->operands_n = 0;
		instr->delay_slot = true;
	}
	// 0000000001101011 "rts/n"
	else if(insword==0x6b && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_RTS;
		instr->operands_n = 0;
	}
	// 0000mmmm01111011 "rtv/n Rm"
	else if((insword & 0xf0ff)==0x7b && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_RTV;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands_n = 1;
	}
	// 0000000000101000 "clrmac"
	else if(insword==0x28 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_CLRMAC;
		instr->operands_n = 0;
	}
	// 0000000001001000 "clrs"
	else if(insword==0x48 && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_CLRS;
		instr->operands_n = 0;
	}
	// 0000000000001000 "clrt"
	else if(insword==0x8 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_CLRT;
		instr->operands_n = 0;
	}
	// 0000nnnn11100011 "icbi @Rn"
	else if((insword & 0xf0ff)==0xe3 && SUPPORTED(SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_ICBI;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100mmmm11100101 "ldbank @Rm,R0"
	else if((insword & 0xf0ff)==0x40e5 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDBANK;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = R0;
		instr->operands_n = 2;
	}
	// 0100mmmm00001110 "ldc Rm,SR"
	else if((insword & 0xf0ff)==0x400e && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = SR;
		instr->operands_n = 2;
	}
	// 0100mmmm00000111 "ldc.l @Rm+,SR"
	else if((insword & 0xf0ff)==0x4007 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = SR;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm01001010 "ldc Rm,TBR"
	else if((insword & 0xf0ff)==0x404a && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: TBR
		instr->operands_n = 1;
	}
	// 0100mmmm00011110 "ldc Rm,GBR"
	else if((insword & 0xf0ff)==0x401e && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = GBR;
		instr->operands_n = 2;
	}
	// 0100mmmm00010111 "ldc.l @Rm+,GBR"
	else if((insword & 0xf0ff)==0x4017 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = GBR;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm00101110 "ldc Rm,VBR"
	else if((insword & 0xf0ff)==0x402e && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = VBR;
		instr->operands_n = 2;
	}
	// 0100mmmm00100111 "ldc.l @Rm+,VBR"
	else if((insword & 0xf0ff)==0x4027 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = VBR;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm01011110 "ldc Rm,MOD"
	else if((insword & 0xf0ff)==0x405e && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: MOD
		instr->operands_n = 1;
	}
	// 0100mmmm01010111 "ldc.l @Rm+,MOD"
	else if((insword & 0xf0ff)==0x4057 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: MOD
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm01111110 "ldc Rm,RE"
	else if((insword & 0xf0ff)==0x407e && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: RE
		instr->operands_n = 1;
	}
	// 0100mmmm01110111 "ldc.l @Rm+,RE"
	else if((insword & 0xf0ff)==0x4077 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: RE
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm01101110 "ldc Rm,RS"
	else if((insword & 0xf0ff)==0x406e && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: RS
		instr->operands_n = 1;
	}
	// 0100mmmm01100111 "ldc.l @Rm+,RS"
	else if((insword & 0xf0ff)==0x4067 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: RS
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm00111010 "ldc Rm,SGR"
	else if((insword & 0xf0ff)==0x403a && SUPPORTED(SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = SGR;
		instr->operands_n = 2;
	}
	// 0100mmmm00110110 "ldc.l @Rm+,SGR"
	else if((insword & 0xf0ff)==0x4036 && SUPPORTED(SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = SGR;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm00111110 "ldc Rm,SSR"
	else if((insword & 0xf0ff)==0x403e && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = SSR;
		instr->operands_n = 2;
	}
	// 0100mmmm00110111 "ldc.l @Rm+,SSR"
	else if((insword & 0xf0ff)==0x4037 && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = SSR;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm01001110 "ldc Rm,SPC"
	else if((insword & 0xf0ff)==0x404e && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = SPC;
		instr->operands_n = 2;
	}
	// 0100mmmm01000111 "ldc.l @Rm+,SPC"
	else if((insword & 0xf0ff)==0x4047 && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = SPC;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm11111010 "ldc Rm,DBR"
	else if((insword & 0xf0ff)==0x40fa && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = DBR;
		instr->operands_n = 2;
	}
	// 0100mmmm11110110 "ldc.l @Rm+,DBR"
	else if((insword & 0xf0ff)==0x40f6 && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = CTRLREG;
		instr->operands[1].regA = DBR;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm1nnn1110 "ldc Rm,Rn_BANK"
	else if((insword & 0xf08f)==0x408e && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		uint8_t n = (insword & 0x70)>>4;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = BANKREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0_BANK0+n);
		instr->operands_n = 2;
	}
	// 0100mmmm1nnn0111 "ldc.l @Rm+,Rn_BANK"
	else if((insword & 0xf08f)==0x4087 && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		uint8_t n = (insword & 0x70)>>4;
		instr->operation = SUPERH_LDC;
		instr->privileged = true;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = BANKREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0_BANK0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 10001110dddddddd "ldre @(disp,PC)"
	else if((insword & 0xff00)==0x8e00 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_LDRE;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = displ2ea(4, d, ctx->address, 0);
		instr->operands_n = 1;
	}
	// 10001100dddddddd "ldrs @(disp,PC)"
	else if((insword & 0xff00)==0x8c00 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t d = (insword & 0xff)>>0;
		instr->operation = SUPERH_LDRS;
		instr->operands[0].type = ADDRESS;
		instr->operands[0].address = displ2ea(4, d, ctx->address, 0);
		instr->operands_n = 1;
	}
	// 0100mmmm00001010 "lds Rm,MACH"
	else if((insword & 0xf0ff)==0x400a && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = MACH;
		instr->operands_n = 2;
	}
	// 0100mmmm00000110 "lds.l @Rm+,MACH"
	else if((insword & 0xf0ff)==0x4006 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = MACH;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm00011010 "lds Rm,MACL"
	else if((insword & 0xf0ff)==0x401a && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = MACL;
		instr->operands_n = 2;
	}
	// 0100mmmm00010110 "lds.l @Rm+,MACL"
	else if((insword & 0xf0ff)==0x4016 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = MACL;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm00101010 "lds Rm,PR"
	else if((insword & 0xf0ff)==0x402a && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = PR;
		instr->operands_n = 2;
	}
	// 0100mmmm00100110 "lds.l @Rm+,PR"
	else if((insword & 0xf0ff)==0x4026 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = PR;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm01101010 "lds Rm,DSR"
	else if((insword & 0xf0ff)==0x406a && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: DSR
		instr->operands_n = 1;
	}
	// 0100mmmm01100110 "lds.l @Rm+,DSR"
	else if((insword & 0xf0ff)==0x4066 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: DSR
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm01110110 "lds Rm,A0"
	else if((insword & 0xf0ff)==0x4076 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: A0
		instr->operands_n = 1;
	}
	// 0100mmmm01110110 "lds.l @Rm+,A0"
	else if((insword & 0xf0ff)==0x4076 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: A0
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm10001010 "lds Rm,X0"
	else if((insword & 0xf0ff)==0x408a && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: X0
		instr->operands_n = 1;
	}
	// 0100mmmm10000110 "lds.l @Rm+,X0"
	else if((insword & 0xf0ff)==0x4086 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: X0
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm10011010 "lds Rm,X1"
	else if((insword & 0xf0ff)==0x409a && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: X1
		instr->operands_n = 1;
	}
	// 0100mmmm10010110 "lds.l @Rm+,X1"
	else if((insword & 0xf0ff)==0x4096 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: X1
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm10101010 "lds Rm,Y0"
	else if((insword & 0xf0ff)==0x40aa && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: Y0
		instr->operands_n = 1;
	}
	// 0100mmmm10100110 "lds.l @Rm+,Y0"
	else if((insword & 0xf0ff)==0x40a6 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: Y0
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm10111010 "lds Rm,Y1"
	else if((insword & 0xf0ff)==0x40ba && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: Y1
		instr->operands_n = 1;
	}
	// 0100mmmm10110110 "lds.l @Rm+,Y1"
	else if((insword & 0xf0ff)==0x40b6 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: Y1
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000000000111000 "ldtlb"
	else if(insword==0x38 && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_LDTLB;
		instr->privileged = true;
		instr->operands_n = 0;
	}
	// 0000nnnn11000011 "movca.l R0,@Rn"
	else if((insword & 0xf0ff)==0xc3 && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_MOVCA;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000000000001001 "nop"
	else if(insword==0x9 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_NOP;
		instr->operands_n = 0;
	}
	// 0000nnnn10010011 "ocbi @Rn"
	else if((insword & 0xf0ff)==0x93 && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_OCBI;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0000nnnn10100011 "ocbp @Rn"
	else if((insword & 0xf0ff)==0xa3 && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_OCBP;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0000nnnn10110011 "ocbwb @Rn"
	else if((insword & 0xf0ff)==0xb3 && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_OCBWB;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0000nnnn10000011 "pref @Rn"
	else if((insword & 0xf0ff)==0x83 && SUPPORTED(SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_PREF;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0000nnnn11010011 "prefi @Rn"
	else if((insword & 0xf0ff)==0xd3 && SUPPORTED(SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_PREFI;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0000000001011011 "resbank"
	else if(insword==0x5b && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_RESBANK;
		instr->operands_n = 0;
	}
	// 0000000000101011 "rte"
	else if(insword==0x2b && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_RTE;
		instr->privileged = true;
		instr->operands_n = 0;
		instr->delay_slot = true;
	}
	// 0100nnnn00010100 "setrc Rn"
	else if((insword & 0xf0ff)==0x4014 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_SETRC;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 10000010iiiiiiii "setrc #imm"
	else if((insword & 0xff00)==0x8200 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		instr->operation = SUPERH_SETRC;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands_n = 1;
	}
	// 0000000001011000 "sets"
	else if(insword==0x58 && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_SETS;
		instr->operands_n = 0;
	}
	// 0000000000011000 "sett"
	else if(insword==0x18 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_SETT;
		instr->operands_n = 0;
	}
	// 0000000000011011 "sleep"
	else if(insword==0x1b && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_SLEEP;
		instr->privileged = true;
		instr->operands_n = 0;
	}
	// 0100nnnn11100001 "stbank R0,@Rn"
	else if((insword & 0xf0ff)==0x40e1 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STBANK;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = R0;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0000nnnn00000010 "stc SR,Rn"
	else if((insword & 0xf0ff)==2 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = SR;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn00000011 "stc.l SR,@-Rn"
	else if((insword & 0xf0ff)==0x4003 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = SR;
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn01001010 "stc TBR,Rn"
	else if((insword & 0xf0ff)==0x4a && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		// unsupported operand: TBR
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0000nnnn00010010 "stc GBR,Rn"
	else if((insword & 0xf0ff)==0x12 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = GBR;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn00010011 "stc.l GBR,@-Rn"
	else if((insword & 0xf0ff)==0x4013 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = GBR;
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn00100010 "stc VBR,Rn"
	else if((insword & 0xf0ff)==0x22 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = VBR;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn00100011 "stc.l VBR,@-Rn"
	else if((insword & 0xf0ff)==0x4023 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = VBR;
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn01010010 "stc MOD,Rn"
	else if((insword & 0xf0ff)==0x52 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		// unsupported operand: MOD
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn01010011 "stc.l MOD,@-Rn"
	else if((insword & 0xf0ff)==0x4053 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		// unsupported operand: MOD
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn01110010 "stc RE,Rn"
	else if((insword & 0xf0ff)==0x72 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		// unsupported operand: RE
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn01110011 "stc.l RE,@-Rn"
	else if((insword & 0xf0ff)==0x4073 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		// unsupported operand: RE
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn01100010 "stc RS,Rn"
	else if((insword & 0xf0ff)==0x62 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		// unsupported operand: RS
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn01100011 "stc.l RS,@-Rn"
	else if((insword & 0xf0ff)==0x4063 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		// unsupported operand: RS
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn00111010 "stc SGR,Rn"
	else if((insword & 0xf0ff)==0x3a && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = SGR;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn00110010 "stc.l SGR,@-Rn"
	else if((insword & 0xf0ff)==0x4032 && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = SGR;
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn00110010 "stc SSR,Rn"
	else if((insword & 0xf0ff)==0x32 && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = SSR;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn00110011 "stc.l SSR,@-Rn"
	else if((insword & 0xf0ff)==0x4033 && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = SSR;
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn01000010 "stc SPC,Rn"
	else if((insword & 0xf0ff)==0x42 && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = SPC;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn01000011 "stc.l SPC,@-Rn"
	else if((insword & 0xf0ff)==0x4043 && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = SPC;
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn11111010 "stc DBR,Rn"
	else if((insword & 0xf0ff)==0xfa && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = DBR;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn11110010 "stc.l DBR,@-Rn"
	else if((insword & 0xf0ff)==0x40f2 && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = CTRLREG;
		instr->operands[0].regA = DBR;
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn1mmm0010 "stc Rm_BANK,Rn"
	else if((insword & 0xf08f)==0x82 && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0x70)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = BANKREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0_BANK0+m);
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn1mmm0011 "stc.l Rm_BANK,@-Rn"
	else if((insword & 0xf08f)==0x4083 && SUPPORTED(SH3|SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0x70)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STC;
		instr->privileged = true;
		instr->operands[0].type = BANKREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0_BANK0+m);
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn00001010 "sts MACH,Rn"
	else if((insword & 0xf0ff)==0xa && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = MACH;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn00000010 "sts.l MACH,@-Rn"
	else if((insword & 0xf0ff)==0x4002 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = MACH;
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn00011010 "sts MACL,Rn"
	else if((insword & 0xf0ff)==0x1a && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = MACL;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn00010010 "sts.l MACL,@-Rn"
	else if((insword & 0xf0ff)==0x4012 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = MACL;
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn00101010 "sts PR,Rn"
	else if((insword & 0xf0ff)==0x2a && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = PR;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100nnnn00100010 "sts.l PR,@-Rn"
	else if((insword & 0xf0ff)==0x4022 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = PR;
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn01101010 "sts DSR,Rn"
	else if((insword & 0xf0ff)==0x6a && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		// unsupported operand: DSR
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn01100010 "sts.l DSR,@-Rn"
	else if((insword & 0xf0ff)==0x4062 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		// unsupported operand: DSR
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn01111010 "sts A0,Rn"
	else if((insword & 0xf0ff)==0x7a && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		// unsupported operand: A0
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn01100010 "sts.l A0,@-Rn"
	else if((insword & 0xf0ff)==0x4062 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		// unsupported operand: A0
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn10001010 "sts X0,Rn"
	else if((insword & 0xf0ff)==0x8a && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		// unsupported operand: X0
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn10000010 "sts.l X0,@-Rn"
	else if((insword & 0xf0ff)==0x4082 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		// unsupported operand: X0
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn10011010 "sts X1,Rn"
	else if((insword & 0xf0ff)==0x9a && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		// unsupported operand: X1
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn10010010 "sts.l X1,@-Rn"
	else if((insword & 0xf0ff)==0x4092 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		// unsupported operand: X1
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn10101010 "sts Y0,Rn"
	else if((insword & 0xf0ff)==0xaa && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		// unsupported operand: Y0
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn10100010 "sts.l Y0,@-Rn"
	else if((insword & 0xf0ff)==0x40a2 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		// unsupported operand: Y0
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000nnnn10111010 "sts Y1,Rn"
	else if((insword & 0xf0ff)==0xba && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		// unsupported operand: Y1
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0100nnnn10110010 "sts.l Y1,@-Rn"
	else if((insword & 0xf0ff)==0x40b2 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		// unsupported operand: Y1
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0000000010101011 "synco"
	else if(insword==0xab && SUPPORTED(SH4A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_SYNCO;
		instr->operands_n = 0;
	}
	// 11000011iiiiiiii "trapa #imm"
	else if((insword & 0xff00)==0xc300 && SUPPORTED(SH1|SH2|SH3|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		int8_t i = int8((insword & 0xff)>>0);
		instr->operation = SUPERH_TRAPA;
		instr->operands[0].type = IMMEDIATE;
		instr->operands[0].immediate = i;
		instr->operands_n = 1;
	}
	// 1111nnnnmmmm1100 "fmov FRm,FRn"
	else if((insword & 0xf00f)==0xf00c && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 2;
	}
	// 1111nnnnmmmm1000 "fmov.s @Rm,FRn"
	else if((insword & 0xf00f)==0xf008 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 2;
	}
	// 1111nnnnmmmm1010 "fmov.s FRm,@Rn"
	else if((insword & 0xf00f)==0xf00a && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 1111nnnnmmmm1001 "fmov.s @Rm+,FRn"
	else if((insword & 0xf00f)==0xf009 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 2;
	}
	// 1111nnnnmmmm1011 "fmov.s FRm,@-Rn"
	else if((insword & 0xf00f)==0xf00b && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 1111nnnnmmmm0110 "fmov.s @(R0,Rm),FRn"
	else if((insword & 0xf00f)==0xf006 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 2;
	}
	// 1111nnnnmmmm0111 "fmov.s FRm,@(R0,Rn)"
	else if((insword & 0xf00f)==0xf007 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0011nnnnmmmm0001 "fmov.s @(disp12,Rm),FRn"
	else if((insword & 0xf00f)==0x3001 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		// unsupported operand: @(disp12,Rm)
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 1;
	}
	// 0011nnnnmmmm0001 "fmov.s FRm,@(disp12,Rn)"
	else if((insword & 0xf00f)==0x3001 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
	}
	// 1111nnn0mmm01100 "fmov DRm,DRn"
	else if((insword & 0xf11f)==0xf00c && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
	}
	// 1111nnn1mmm01100 "fmov DRm,XDn"
	else if((insword & 0xf11f)==0xf10c && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		// unsupported operand: XDn
		instr->operands_n = 1;
	}
	// 1111nnn0mmm11100 "fmov XDm,DRn"
	else if((insword & 0xf11f)==0xf01c && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FMOV;
		// unsupported operand: XDm
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 1;
	}
	// 1111nnn1mmm11100 "fmov XDm,XDn"
	else if((insword & 0xf11f)==0xf11c && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FMOV;
		// unsupported operand: XDm
		// unsupported operand: XDn
		instr->operands_n = 0;
	}
	// 1111nnn0mmmm1000 "fmov.d @Rm,DRn"
	else if((insword & 0xf10f)==0xf008 && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
	}
	// 1111nnn1mmmm1000 "fmov.d @Rm,XDn"
	else if((insword & 0xf10f)==0xf108 && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: XDn
		instr->operands_n = 1;
	}
	// 1111nnnnmmm01010 "fmov.d DRm,@Rn"
	else if((insword & 0xf01f)==0xf00a && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 1111nnnnmmm11010 "fmov.d XDm,@Rn"
	else if((insword & 0xf01f)==0xf01a && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		// unsupported operand: XDm
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 1111nnn0mmmm1001 "fmov.d @Rm+,DRn"
	else if((insword & 0xf10f)==0xf009 && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
	}
	// 1111nnn1mmmm1001 "fmov.d @Rm+,XDn"
	else if((insword & 0xf10f)==0xf109 && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: XDn
		instr->operands_n = 1;
	}
	// 1111nnnnmmm01011 "fmov.d DRm,@-Rn"
	else if((insword & 0xf01f)==0xf00b && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 1111nnnnmmm11011 "fmov.d XDm,@-Rn"
	else if((insword & 0xf01f)==0xf01b && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		// unsupported operand: XDm
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 1111nnn0mmmm0110 "fmov.d @(R0,Rm),DRn"
	else if((insword & 0xf10f)==0xf006 && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
	}
	// 1111nnn1mmmm0110 "fmov.d @(R0,Rm),XDn"
	else if((insword & 0xf10f)==0xf106 && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SUPERH_REGISTER)(R0+m);
		// unsupported operand: XDn
		instr->operands_n = 1;
	}
	// 1111nnnnmmm00111 "fmov.d DRm,@(R0,Rn)"
	else if((insword & 0xf01f)==0xf007 && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		instr->operands[1].type = DEREF_REG_REG;
		instr->operands[1].regA = R0;
		instr->operands[1].regB = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 1111nnnnmmm10111 "fmov.d XDm,@(R0,Rn)"
	else if((insword & 0xf01f)==0xf017 && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		// unsupported operand: XDm
		instr->operands[0].type = DEREF_REG_REG;
		instr->operands[0].regA = R0;
		instr->operands[0].regB = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 1;
	}
	// 0011nnn0mmmm0001 "fmov.d @(disp12,Rm),DRn"
	else if((insword & 0xf10f)==0x3001 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FMOV;
		// unsupported operand: @(disp12,Rm)
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 1;
	}
	// 0011nnnnmmm00001 "fmov.d DRm,@(disp12,Rn)"
	else if((insword & 0xf01f)==0x3001 && SUPPORTED(SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMOV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		// unsupported operand: @(disp12,Rn)
		instr->operands_n = 1;
	}
	// 1111nnnn10001101 "fldi0 FRn"
	else if((insword & 0xf0ff)==0xf08d && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FLDI0;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 1;
	}
	// 1111nnnn10011101 "fldi1 FRn"
	else if((insword & 0xf0ff)==0xf09d && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FLDI1;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 1;
	}
	// 1111mmmm00011101 "flds FRm,FPUL"
	else if((insword & 0xf0ff)==0xf01d && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FLDS;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = FPUL;
		instr->operands_n = 2;
	}
	// 1111nnnn00001101 "fsts FPUL,FRn"
	else if((insword & 0xf0ff)==0xf00d && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FSTS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = FPUL;
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 2;
	}
	// 1111nnnn01011101 "fabs FRn"
	else if((insword & 0xf0ff)==0xf05d && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FABS;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 1;
	}
	// 1111nnnn01001101 "fneg FRn"
	else if((insword & 0xf0ff)==0xf04d && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FNEG;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 1;
	}
	// 1111nnnnmmmm0000 "fadd FRm,FRn"
	else if((insword & 0xf00f)==0xf000 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FADD;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 2;
	}
	// 1111nnnnmmmm0001 "fsub FRm,FRn"
	else if((insword & 0xf00f)==0xf001 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FSUB;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 2;
	}
	// 1111nnnnmmmm0010 "fmul FRm,FRn"
	else if((insword & 0xf00f)==0xf002 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMUL;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 2;
	}
	// 1111nnnnmmmm1110 "fmac FR0,FRm,FRn"
	else if((insword & 0xf00f)==0xf00e && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FMAC;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = FR0;
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[2].type = FPUREG;
		instr->operands[2].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 3;
	}
	// 1111nnnnmmmm0011 "fdiv FRm,FRn"
	else if((insword & 0xf00f)==0xf003 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FDIV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 2;
	}
	// 1111nnnn01101101 "fsqrt FRn"
	else if((insword & 0xf0ff)==0xf06d && SUPPORTED(SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FSQRT;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 1;
	}
	// 1111nnnnmmmm0100 "fcmp/eq FRm,FRn"
	else if((insword & 0xf00f)==0xf004 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FCMP;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 2;
		instr->cond = CMP_COND_EQ;
	}
	// 1111nnnnmmmm0101 "fcmp/gt FRm,FRn"
	else if((insword & 0xf00f)==0xf005 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf0)>>4;
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FCMP;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 2;
		instr->cond = CMP_COND_GT;
	}
	// 1111nnnn00101101 "float FPUL,FRn"
	else if((insword & 0xf0ff)==0xf02d && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FLOAT;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = FPUL;
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 2;
	}
	// 1111mmmm00111101 "ftrc FRm,FPUL"
	else if((insword & 0xf0ff)==0xf03d && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FTRC;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = FPUL;
		instr->operands_n = 2;
	}
	// 1111nnmm11101101 "fipr FVm,FVn"
	else if((insword & 0xf0ff)==0xf0ed && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0x300)>>8;
		uint8_t n = (insword & 0xc00)>>10;
		instr->operation = SUPERH_FIPR;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FV0+4*m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FV0+4*n);
		instr->operands_n = 2;
	}
	// 1111nn0111111101 "ftrv XMTRX,FVn"
	else if((insword & 0xf3ff)==0xf1fd && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xc00)>>10;
		instr->operation = SUPERH_FTRV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = XMTRX;
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(FV0+4*n);
		instr->operands_n = 2;
	}
	// 1111nnnn01111101 "fsrra FRn"
	else if((insword & 0xf0ff)==0xf07d && SUPPORTED(SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_FSRRA;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(FR0+n);
		instr->operands_n = 1;
	}
	// 1111nnn011111101 "fsca FPUL,DRn"
	else if((insword & 0xf1ff)==0xf0fd && SUPPORTED(SH4A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FSCA;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = FPUL;
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
	}
	// 1111nnn001011101 "fabs DRn"
	else if((insword & 0xf1ff)==0xf05d && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FABS;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 1;
	}
	// 1111nnn001001101 "fneg DRn"
	else if((insword & 0xf1ff)==0xf04d && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FNEG;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 1;
	}
	// 1111nnn0mmm00000 "fadd DRm,DRn"
	else if((insword & 0xf11f)==0xf000 && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FADD;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
	}
	// 1111nnn0mmm00001 "fsub DRm,DRn"
	else if((insword & 0xf11f)==0xf001 && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FSUB;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
	}
	// 1111nnn0mmm00010 "fmul DRm,DRn"
	else if((insword & 0xf11f)==0xf002 && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FMUL;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
	}
	// 1111nnn0mmm00011 "fdiv DRm,DRn"
	else if((insword & 0xf11f)==0xf003 && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FDIV;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
	}
	// 1111nnn001101101 "fsqrt DRn"
	else if((insword & 0xf1ff)==0xf06d && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FSQRT;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 1;
	}
	// 1111nnn0mmm00100 "fcmp/eq DRm,DRn"
	else if((insword & 0xf11f)==0xf004 && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FCMP;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
		instr->cond = CMP_COND_EQ;
	}
	// 1111nnn0mmm00101 "fcmp/gt DRm,DRn"
	else if((insword & 0xf11f)==0xf005 && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe0)>>5;
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FCMP;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
		instr->cond = CMP_COND_GT;
	}
	// 1111nnn000101101 "float FPUL,DRn"
	else if((insword & 0xf1ff)==0xf02d && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FLOAT;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = FPUL;
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
	}
	// 1111mmm000111101 "ftrc DRm,FPUL"
	else if((insword & 0xf1ff)==0xf03d && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FTRC;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = FPUL;
		instr->operands_n = 2;
	}
	// 1111mmm010111101 "fcnvds DRm,FPUL"
	else if((insword & 0xf1ff)==0xf0bd && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FCNVDS;
		instr->operands[0].type = FPUREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(DR0+2*m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = FPUL;
		instr->operands_n = 2;
	}
	// 1111nnn010101101 "fcnvsd FPUL,DRn"
	else if((insword & 0xf1ff)==0xf0ad && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xe00)>>9;
		instr->operation = SUPERH_FCNVSD;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = FPUL;
		instr->operands[1].type = FPUREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(DR0+2*n);
		instr->operands_n = 2;
	}
	// 0100mmmm01101010 "lds Rm,FPSCR"
	else if((insword & 0xf0ff)==0x406a && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = FPSCR;
		instr->operands_n = 2;
	}
	// 0000nnnn01101010 "sts FPSCR,Rn"
	else if((insword & 0xf0ff)==0x6a && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = FPSCR;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100mmmm01100110 "lds.l @Rm+,FPSCR"
	else if((insword & 0xf0ff)==0x4066 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = FPSCR;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100nnnn01100010 "sts.l FPSCR,@-Rn"
	else if((insword & 0xf0ff)==0x4062 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = FPSCR;
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100mmmm01011010 "lds Rm,FPUL"
	else if((insword & 0xf0ff)==0x405a && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].type = GPREG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = FPUL;
		instr->operands_n = 2;
	}
	// 0000nnnn01011010 "sts FPUL,Rn"
	else if((insword & 0xf0ff)==0x5a && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = FPUL;
		instr->operands[1].type = GPREG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
	}
	// 0100mmmm01010110 "lds.l @Rm+,FPUL"
	else if((insword & 0xf0ff)==0x4056 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t m = (insword & 0xf00)>>8;
		instr->operation = SUPERH_LDS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		instr->operands[0].type = DEREF_REG;
		instr->operands[0].regA = (SUPERH_REGISTER)(R0+m);
		instr->operands[1].type = SYSREG;
		instr->operands[1].regA = FPUL;
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 0100nnnn01010010 "sts.l FPUL,@-Rn"
	else if((insword & 0xf0ff)==0x4052 && SUPPORTED(SH2E|SH3E|SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		uint8_t n = (insword & 0xf00)>>8;
		instr->operation = SUPERH_STS;
		instr->operands[0].type = SYSREG;
		instr->operands[0].regA = FPUL;
		instr->operands[1].flags |= SUPERH_FLAG_PRE_DECREMENT;
		instr->operands[1].type = DEREF_REG;
		instr->operands[1].regA = (SUPERH_REGISTER)(R0+n);
		instr->operands_n = 2;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 1111101111111101 "frchg"
	else if(insword==0xfbfd && SUPPORTED(SH4|SH4A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_FRCHG;
		instr->operands_n = 0;
	}
	// 1111001111111101 "fschg"
	else if(insword==0xf3fd && SUPPORTED(SH4|SH4A|SH2A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_FSCHG;
		instr->operands_n = 0;
	}
	// 1111011111111101 "fpchg"
	else if(insword==0xf7fd && SUPPORTED(SH4A)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_FPCHG;
		instr->operands_n = 0;
	}
	// 111101AADDDD0000 "movs.w @-As,Ds"
	else if((insword & 0xfc0f)==0xf400 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		// unsupported operand: @As
		// unsupported operand: Ds
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 111101AADDDD0100 "movs.w @As,Ds"
	else if((insword & 0xfc0f)==0xf404 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		// unsupported operand: @As
		// unsupported operand: Ds
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 111101AADDDD1000 "movs.w @As+,Ds"
	else if((insword & 0xfc0f)==0xf408 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		// unsupported operand: @As
		// unsupported operand: Ds
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 111101AADDDD1100 "movs.w @As+Ix,Ds"
	else if((insword & 0xfc0f)==0xf40c && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		// unsupported operand: @As+Ix
		// unsupported operand: Ds
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 111101AADDDD0001 "movs.w Ds,@-As"
	else if((insword & 0xfc0f)==0xf401 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		// unsupported operand: Ds
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		// unsupported operand: @As
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 111101AADDDD0101 "movs.w Ds,@As"
	else if((insword & 0xfc0f)==0xf405 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		// unsupported operand: Ds
		// unsupported operand: @As
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 111101AADDDD1001 "movs.w Ds,@As+"
	else if((insword & 0xfc0f)==0xf409 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		// unsupported operand: Ds
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		// unsupported operand: @As
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 111101AADDDD1101 "movs.w Ds,@As+Is"
	else if((insword & 0xfc0f)==0xf40d && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		// unsupported operand: Ds
		// unsupported operand: @As+Is
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_W;
	}
	// 111101AADDDD0010 "movs.l @-As,Ds"
	else if((insword & 0xfc0f)==0xf402 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		// unsupported operand: @As
		// unsupported operand: Ds
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 111101AADDDD0110 "movs.l @As,Ds"
	else if((insword & 0xfc0f)==0xf406 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		// unsupported operand: @As
		// unsupported operand: Ds
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 111101AADDDD1010 "movs.l @As+,Ds"
	else if((insword & 0xfc0f)==0xf40a && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		// unsupported operand: @As
		// unsupported operand: Ds
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 111101AADDDD1110 "movs.l @As+Is,Ds"
	else if((insword & 0xfc0f)==0xf40e && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		// unsupported operand: @As+Is
		// unsupported operand: Ds
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 111101AADDDD0011 "movs.l Ds,@-As"
	else if((insword & 0xfc0f)==0xf403 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		// unsupported operand: Ds
		instr->operands[0].flags |= SUPERH_FLAG_PRE_DECREMENT;
		// unsupported operand: @As
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 111101AADDDD0111 "movs.l Ds,@As"
	else if((insword & 0xfc0f)==0xf407 && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		// unsupported operand: Ds
		// unsupported operand: @As
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 111101AADDDD1011 "movs.l Ds,@As+"
	else if((insword & 0xfc0f)==0xf40b && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		// unsupported operand: Ds
		instr->operands[0].flags |= SUPERH_FLAG_POST_INCREMENT;
		// unsupported operand: @As
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	// 111101AADDDD1111 "movs.l Ds,@As+Is"
	else if((insword & 0xfc0f)==0xf40f && SUPPORTED(DSP)) { 
		debug_pattern_matched();
		instr->operation = SUPERH_MOVS;
		// unsupported operand: Ds
		// unsupported operand: @As+Is
		instr->operands_n = 0;
		instr->length_suffix = LEN_SUFFIX_L;
	}
	else {
		return -1;
	}
	return 0;
}
