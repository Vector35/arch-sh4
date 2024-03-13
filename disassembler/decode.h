#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "operations.h"

enum SUPERH_REGISTER {
	SUPERH_REGISTER_NONE=0,

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
	XD0, XD1, XD2, XD3,
	XMTRX,

	SUPERH_REGISTER_MAXIMUM
};

enum SUPERH_LEN_SUFFIX { LEN_SUFFIX_NONE=0, LEN_SUFFIX_B, LEN_SUFFIX_W, LEN_SUFFIX_L };

#define SUPERH_FLAG_POST_INCREMENT 1
#define SUPERH_FLAG_PRE_INCREMENT 2
#define SUPERH_FLAG_POST_DECREMENT 4
#define SUPERH_FLAG_PRE_DECREMENT 8

enum SUPERH_OPER_TYPE {
	OPER_TYPE_NONE=0, GPREG, BANKREG, CTRLREG, SYSREG, FPUREG, IMMEDIATE,
	ADDRESS,
	DEREF_REG, /* like @r0 */
	DEREF_REG_REG, /* like @(r0,gbr) */
	DEREF_REG_IMM, /* like @(#4,gbr) */
};

struct disasm_operand
{
	enum SUPERH_OPER_TYPE type;
	enum SUPERH_REGISTER regA;
	enum SUPERH_REGISTER regB;
	int flags;
	int displacement;
	int16_t immediate;
	uint64_t address;
};

enum SUPERH_CMP_COND {
	CMP_COND_NONE=0,
	CMP_COND_EQ, CMP_COND_GE, CMP_COND_GT, CMP_COND_HI, CMP_COND_HS, CMP_COND_PL, CMP_COND_PZ, CMP_COND_STR,

	CMP_COND_MAXIMUM
};

#define SH1 (1<<0)
#define SH2 (1<<1)
#define SH3 (1<<2)
#define SH4 (1<<3)
#define SH4A (1<<4)
#define SH2A (1<<5)
#define DSP (1<<6)
#define SH2E (1<<7)
#define SH3E (1<<8)

/* disassembler request */
struct context {
	uint16_t insword;
	uint64_t address;
	bool FPSCR_SZ;
	bool FPSCR_PR;
	uint32_t feature_flags;
};

struct Instruction
{
	enum Operation operation;
	enum SUPERH_LEN_SUFFIX length_suffix;
	enum SUPERH_CMP_COND cond;
	/* operands */
	int operands_n;
	struct disasm_operand operands[3];
	/* other flags */
	bool privileged;
	bool delay_slot;
	int size;
};

#ifndef __cplusplus
	typedef enum SUPERH_REGISTER SUPERH_REGISTER;
	typedef struct context context;
	typedef struct Instruction Instruction;
#endif

#ifdef __cplusplus
extern "C" {
#endif
int superh_decompose(uint16_t insword, Instruction *instr, uint64_t address);
#ifdef __cplusplus
}
#endif
