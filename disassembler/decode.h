#pragma once

#include <stdint.h>
#include <stdbool.h>

enum SH4_OPC {
	OPC_NONE=0,
	OPC_ADD, OPC_ADDC, OPC_ADDV, OPC_AND, OPC_BF, OPC_BRA, OPC_BRAF, OPC_BSR,
	OPC_BSRF, OPC_BT, OPC_CLRMAC, OPC_CLRS, OPC_CLRT, OPC_CMP, OPC_DIV0S,
	OPC_DIV0U, OPC_DIV1, OPC_DMULS, OPC_DMULU, OPC_DT, OPC_ERROR, OPC_EXTS,
	OPC_EXTU, OPC_FABS, OPC_FADD, OPC_FCMP, OPC_FCNVDS, OPC_FCNVSD, OPC_FDIV,
	OPC_FIPR, OPC_FLDI0, OPC_FLDI1, OPC_FLDS, OPC_FLOAT, OPC_FMAC, OPC_FMOV,
	OPC_FMUL, OPC_FNEG, OPC_FRCHG, OPC_FSCA, OPC_FSCHG, OPC_FSQRT, OPC_FSRRA,
	OPC_FSTS, OPC_FSUB, OPC_FTRC, OPC_FTRV, OPC_JMP, OPC_JSR, OPC_LDC, OPC_LDS,
	OPC_LDTLB, OPC_MAC, OPC_MOV, OPC_MOVA, OPC_MOVCA, OPC_MOVT, OPC_MUL, OPC_MULS,
	OPC_MULU, OPC_NEG, OPC_NEGC, OPC_NOP, OPC_NOT, OPC_OCBI, OPC_OCBP, OPC_OCBWB,
	OPC_OR, OPC_PREF, OPC_ROTCL, OPC_ROTCR, OPC_ROTL, OPC_ROTR, OPC_RTE, OPC_RTS,
	OPC_SETS, OPC_SETT, OPC_SHAD, OPC_SHAL, OPC_SHAR, OPC_SHLD, OPC_SHLL,
	OPC_SHLL16, OPC_SHLL2, OPC_SHLL8, OPC_SHLR, OPC_SHLR16, OPC_SHLR2, OPC_SHLR8,
	OPC_SLEEP, OPC_STC, OPC_STS, OPC_SUB, OPC_SUBC, OPC_SUBV, OPC_SWAP, OPC_TAS,
	OPC_TRAPA, OPC_TST, OPC_XOR, OPC_XTRCT,

	OPC_MAXIMUM
};

enum SH4_REGISTER {
	SH4_REGISTER_NONE=0,

	/* gpr's */
	R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15,
	/* banks */
	R0_BANK0, R1_BANK0, R2_BANK0, R3_BANK0, R4_BANK0, R5_BANK0, R6_BANK0, R7_BANK0,
	R0_BANK1, R1_BANK1, R2_BANK1, R3_BANK1, R4_BANK1, R5_BANK1, R6_BANK1, R7_BANK1,
	/* control */
	SR, SSR, SPC, GBR, VBR, SGR, DBR,
	/* system */
	MACH, MACL, PR, PC, FPSCR, FPUL,
	/* floating point */
	FR0, FR1, FR2, FR3, FR4, FR5, FR6, FR7, FR8, FR9, FR10, FR11, FR12, FR13, FR14, FR15,
	XR0, XR1, XR2, XR3, XR4, XR5, XR6, XR7, XR8, XR9, XR10, XR11, XR12, XR13, XR14, XR15,
	DR0, DR1, DR2, DR3, DR4, DR5, DR6, DR7, DR8, DR9, DR10, DR11, DR12, DR13, DR14, DR15,
	FV0, FV1, FV2, FV3, FV4, FV5, FV6, FV7, FV8, FV9, FV10, FV11, FV12, FV13, FV14, FV15,
	/* wtf */
	XD0, XD1, XD2, XD3,
	XMTRX,

	SH4_REGISTER_MAXIMUM
};

enum SH4_LEN_SUFFIX { LEN_SUFFIX_NONE=0, LEN_SUFFIX_B, LEN_SUFFIX_W, LEN_SUFFIX_L };

#define SH4_FLAG_POST_INCREMENT 1
#define SH4_FLAG_PRE_INCREMENT 2
#define SH4_FLAG_POST_DECREMENT 4
#define SH4_FLAG_PRE_DECREMENT 8

enum SH4_OPER_TYPE {
	OPER_TYPE_NONE=0, GPREG, BANKREG, CTRLREG, SYSREG, FPUREG, IMMEDIATE,
	ADDRESS,
	DEREF_REG, /* like @r0 */
	DEREF_REG_REG, /* like @(r0,gbr) */
	DEREF_REG_IMM, /* like @(#4,gbr) */
};

struct disasm_operand
{
	enum SH4_OPER_TYPE type;
	enum SH4_REGISTER regA;
	enum SH4_REGISTER regB;
	int flags;
	int displacement;
	int16_t immediate;
	uint64_t address;
};

enum SH4_CMP_COND {
	CMP_COND_NONE=0,
	CMP_COND_EQ, CMP_COND_GE, CMP_COND_GT, CMP_COND_HI, CMP_COND_HS, CMP_COND_PL, CMP_COND_PZ, CMP_COND_STR,

	CMP_COND_MAXIMUM
};

struct Instruction
{
	enum SH4_OPC opcode;
	enum SH4_LEN_SUFFIX length_suffix;
	bool delay_slot;
	enum SH4_CMP_COND cond;
	/* operands */
	int operands_n;
	struct disasm_operand operands[3];

	/* string production */
	char string[32];
};

#ifndef __cplusplus
	typedef enum SH4_REGISTER SH4_REGISTER;
	typedef struct Instruction Instruction;
#endif

#ifdef __cplusplus
extern "C" {
#endif
int sh4_decompose(uint16_t insword, Instruction *instr, uint64_t address);
#ifdef __cplusplus
}
#endif
