/*
	lldb ./cmdline 1234
	b decode
	r
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "decode.h"
#include "format.h"

int verbose = 1;

const char *oper_type_tostr(enum SH4_OPER_TYPE t)
{
	switch(t) {
		case OPER_TYPE_NONE: return "NONE";
		case GPREG: return "GPREG";
		case BANKREG: return "BANKREG";
		case CTRLREG: return "CTRLREG";
		case SYSREG: return "SYSREG";
		case FPUREG: return "FPUREG";
		case IMMEDIATE: return "IMMEDIATE";
		case ADDRESS: return "ADDRESS";
		case DEREF_REG: return "REG";
		case DEREF_REG_REG: return "REG_REG";
		case DEREF_REG_IMM: return "REG_IMM";
		default:
			return "ERROR";
	}
}

int disassemble(uint64_t address, uint32_t insword, char *result)
{
	int rc;
	Instruction instr;
	memset(&instr, 0, sizeof(instr));

	rc = sh4_decompose(insword, &instr, address);
	if(verbose)
		printf("sh4_decompose() returned %d\n", rc);
	if(rc) return rc;

	rc = sh4_disassemble(&instr, result, 1024);
	if(verbose)
		printf("sh4_disassemble() returned %d\n", rc);
	if(rc) return rc;

	return 0;
}

const char *flags_tostr(int f)
{
	switch(f)
	{
		case 0: return "";
		case SH4_FLAG_POST_INCREMENT: return "POST_INCREMENT";
		case SH4_FLAG_PRE_INCREMENT: return "PRE_INCREMENT";
		case SH4_FLAG_POST_DECREMENT: return "POST_DECREMENT";
		case SH4_FLAG_PRE_DECREMENT: return "PRE_DECREMENT";
		default:
			return "ERROR";
	}
}

/* main */
int main(int ac, char **av)
{
	char instxt[1024]={'\0'};

	if(ac <= 1) {
		printf("example usage:\n");
		printf("\t%s 1234\n", av[0]);
		return -1;
	}

	if(!strcmp(av[1], "speed")) {
		verbose = 0; // no prints, speed test
		srand(0xCAFE);
		for(int i=0; i<10000000; i++) {
			uint16_t insword = (rand() << 16) ^ rand();
			disassemble(0, insword, instxt);
			// no prints, speed test
			//printf("%08X: %s\n", 0, instxt);
		}
		return 0;
	}

	if(!strcmp(av[1], "all"))
	{
		verbose = 0;
		for(uint16_t insword=0; insword!=0xFFFF; insword++) {
			disassemble(0, insword, instxt);
			printf("%04X: %s\n", insword, instxt);
		}
	}

	if(!strcmp(av[1], "test")) {
		srand(0xCAFE);
		while(1) {
			Instruction instr;
			memset(&instr, 0, sizeof(instr));
			uint16_t insword = (rand() << 16) ^ rand();
			sh4_decompose(insword, &instr, 0);
			//printf("%08X: %d\n", insword, instr.encoding);
		}
	}

	else {
		Instruction instr;
		memset(&instr, 0, sizeof(instr));
		uint16_t insword = strtoul(av[1], NULL, 16);
		printf("insword: 0x%04X\n", insword);
		sh4_decompose(insword, &instr, 0);

		printf("instr.opcode: %d %s\n", instr.opcode, sh4_opc_strs[instr.opcode]);
		printf("instr.length_suffix: %d %s\n", instr.length_suffix, sh4_len_suffix_strs[instr.length_suffix]);
		printf("instr.delay_slot: %d\n", instr.delay_slot);
		printf("instr.cond: %d %s\n", instr.cond, sh4_cmp_cond_strs[instr.cond]);
		printf("instr.operands_n: %d\n", instr.operands_n);
		for(int i=0; i<instr.operands_n; i++) {
			printf("  operands[%d]:\n", i);
			printf("    .type: %d %s\n", instr.operands[i].type, oper_type_tostr(instr.operands[i].type));
			printf("    .regA: %d %s\n", instr.operands[i].regA, sh4_reg_strs[instr.operands[i].regA]);
			printf("    .regB: %d %s\n", instr.operands[i].regB, sh4_reg_strs[instr.operands[i].regB]);
			printf("    .flags: %d %s\n", instr.operands[i].flags, flags_tostr(instr.operands[i].flags));
			printf("    .displacement: 0x%X\n", instr.operands[i].displacement);
			printf("    .immediate: 0x%X\n", instr.operands[i].immediate);
			printf("    .address: 0x%llX\n", instr.operands[i].address);
		}
		
		if(disassemble(0, insword, instxt) == 0)
			printf("%08X: %s\n", insword, instxt);

	}
}

