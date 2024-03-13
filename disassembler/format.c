#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "decode.h"
#include "format.h"

// in parallel with enum SUPERH_REGISTER
const char *superh_reg_strs[] = {
	"error",
	/* "gpr"'"s" */
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
	/* "banks" */
	"r0_bank", "r1_bank", "r2_bank", "r3_bank", "r4_bank", "r5_bank", "r6_bank", "r7_bank",
	"r0_bank1", "r1_bank1", "r2_bank1", "r3_bank1", "r4_bank1", "r5_bank1", "r6_bank1", "r7_bank1",
	/* "control" */
	"sr", "ssr", "spc", "gbr", "vbr", "sgr", "dbr",
	/* "system" */
	"mach", "macl", "pr", "pc", "fpscr", "fpul",
	/* "floating" "point" */
	"fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7",
	"fr8", "fr9", "fr10", "fr11", "fr12", "fr13", "fr14", "fr15",

	"xr0", "xr1", "xr2", "xr3", "xr4", "xr5", "xr6", "xr7",
	"xr8", "xr9", "xr10", "xr11", "xr12", "xr13", "xr14", "xr15",

	"dr0", "dr1", "dr2", "dr3", "dr4", "dr5", "dr6", "dr7",
	"dr8", "dr9", "dr10", "dr11", "dr12", "dr13", "dr14", "dr15",

	"fv0", "fv1", "fv2", "fv3", "fv4", "fv5", "fv6", "fv7",
	"fv8", "fv9", "fv10", "fv11", "fv12", "fv13", "fv14", "fv15",

	/* "wtf" */
	"xd0", "xd1", "xd2", "xd3",
	"xmtrx",
};

const char *superh_cmp_cond_strs[] = {
	"error",
	"eq", "ge", "gt", "hi", "hs", "pl", "pz", "str"
};

const char *superh_len_suffix_strs[] = { "", ".b", ".w", ".l" };

int superh_disassemble(Instruction *instr, char *result, size_t buf_sz)
{
	/* default answer */
	result[0] = '\0';

	/* mnemonic */
	strcpy(result, operation_to_str(instr->operation));
	/* size modifier suffixes {.b, .w, .l} */
	if(instr->length_suffix == LEN_SUFFIX_B)
		strcat(result, ".b");
	if(instr->length_suffix == LEN_SUFFIX_W)
		strcat(result, ".w");
	if(instr->length_suffix == LEN_SUFFIX_L)
		strcat(result, ".l");
	/* delay slot suffix .s (/s from doc) */
	if(instr->delay_slot && (instr->operation==SUPERH_BT || instr->operation==SUPERH_BF))
		strcat(result, ".s");
	/* conditional "/xxx" if this is a cmp */
	if(instr->operation == SUPERH_CMP || instr->operation == SUPERH_FCMP) {
		if(instr->cond > CMP_COND_NONE && instr->cond < CMP_COND_MAXIMUM) {
			strcat(result, "/");
			strcat(result, superh_cmp_cond_strs[instr->cond]);
		}
	}

	/* operands */
	if(instr->operands_n)
		strcat(result, " ");

	for(int i=0; i < (instr->operands_n); ++i) {
		char buf[32];

		switch(instr->operands[i].type) {
			case GPREG:
			case BANKREG:
			case CTRLREG:
			case SYSREG:
			case FPUREG:
				strcat(result, superh_reg_strs[instr->operands[i].regA]);
				break;
			case IMMEDIATE:
				snprintf(buf, sizeof(buf), "#%d", instr->operands[i].immediate);
				strcat(result, buf);
				break;
			case ADDRESS:
				snprintf(buf, sizeof(buf), "0x%016" PRIx64, instr->operands[i].address);
				strcat(result, buf);
				break;
			case DEREF_REG:
				strcat(result, "@");
				if(instr->operands[i].flags & SUPERH_FLAG_PRE_INCREMENT) strcat(result, "+");
				if(instr->operands[i].flags & SUPERH_FLAG_PRE_DECREMENT) strcat(result, "-");
				strcat(result, superh_reg_strs[instr->operands[i].regA]);
				if(instr->operands[i].flags & SUPERH_FLAG_POST_INCREMENT) strcat(result, "+");
				if(instr->operands[i].flags & SUPERH_FLAG_POST_DECREMENT) strcat(result, "-");
				break;
			case DEREF_REG_REG:
				snprintf(buf, sizeof(buf), "@(%s,%s)", superh_reg_strs[instr->operands[i].regA], superh_reg_strs[instr->operands[i].regB]);
				strcat(result, buf);
				break;
			case DEREF_REG_IMM:
				snprintf(buf, sizeof(buf), "@(%d,%s)", instr->operands[i].displacement, superh_reg_strs[instr->operands[i].regA]);
				strcat(result, buf);
				break;
			default:
				printf("MAJOR ERROR\n");
				break;
		}

		if(i != instr->operands_n-1)
			strcat(result, ",");
	}

	return 0;
}

