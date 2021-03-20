/* GENERATED FILE */
#include "operations.h"
const char *operation_to_str(enum Operation oper)
{
	switch(oper) {
		case SUPERH_ADD: return "add";
		case SUPERH_ADDC: return "addc";
		case SUPERH_ADDV: return "addv";
		case SUPERH_AND: return "and";
		case SUPERH_BAND: return "band";
		case SUPERH_BANDNOT: return "bandnot";
		case SUPERH_BCLR: return "bclr";
		case SUPERH_BF: return "bf";
		case SUPERH_BLD: return "bld";
		case SUPERH_BLDNOT: return "bldnot";
		case SUPERH_BOR: return "bor";
		case SUPERH_BORNOT: return "bornot";
		case SUPERH_BRA: return "bra";
		case SUPERH_BRAF: return "braf";
		case SUPERH_BSET: return "bset";
		case SUPERH_BSR: return "bsr";
		case SUPERH_BSRF: return "bsrf";
		case SUPERH_BST: return "bst";
		case SUPERH_BT: return "bt";
		case SUPERH_BXOR: return "bxor";
		case SUPERH_CLIPS: return "clips";
		case SUPERH_CLIPU: return "clipu";
		case SUPERH_CLRMAC: return "clrmac";
		case SUPERH_CLRS: return "clrs";
		case SUPERH_CLRT: return "clrt";
		case SUPERH_CMP: return "cmp";
		case SUPERH_DCF: return "dcf";
		case SUPERH_DCT: return "dct";
		case SUPERH_DIV0S: return "div0s";
		case SUPERH_DIV0U: return "div0u";
		case SUPERH_DIV1: return "div1";
		case SUPERH_DIVS: return "divs";
		case SUPERH_DIVU: return "divu";
		case SUPERH_DMULS: return "dmuls";
		case SUPERH_DMULU: return "dmulu";
		case SUPERH_DT: return "dt";
		case SUPERH_EXTS: return "exts";
		case SUPERH_EXTU: return "extu";
		case SUPERH_FABS: return "fabs";
		case SUPERH_FADD: return "fadd";
		case SUPERH_FCMP: return "fcmp";
		case SUPERH_FCNVDS: return "fcnvds";
		case SUPERH_FCNVSD: return "fcnvsd";
		case SUPERH_FDIV: return "fdiv";
		case SUPERH_FIPR: return "fipr";
		case SUPERH_FLDI0: return "fldi0";
		case SUPERH_FLDI1: return "fldi1";
		case SUPERH_FLDS: return "flds";
		case SUPERH_FLOAT: return "float";
		case SUPERH_FMAC: return "fmac";
		case SUPERH_FMOV: return "fmov";
		case SUPERH_FMUL: return "fmul";
		case SUPERH_FNEG: return "fneg";
		case SUPERH_FPCHG: return "fpchg";
		case SUPERH_FRCHG: return "frchg";
		case SUPERH_FSCA: return "fsca";
		case SUPERH_FSCHG: return "fschg";
		case SUPERH_FSQRT: return "fsqrt";
		case SUPERH_FSRRA: return "fsrra";
		case SUPERH_FSTS: return "fsts";
		case SUPERH_FSUB: return "fsub";
		case SUPERH_FTRC: return "ftrc";
		case SUPERH_FTRV: return "ftrv";
		case SUPERH_ICBI: return "icbi";
		case SUPERH_JMP: return "jmp";
		case SUPERH_JSR: return "jsr";
		case SUPERH_LDBANK: return "ldbank";
		case SUPERH_LDC: return "ldc";
		case SUPERH_LDRE: return "ldre";
		case SUPERH_LDRS: return "ldrs";
		case SUPERH_LDS: return "lds";
		case SUPERH_LDTLB: return "ldtlb";
		case SUPERH_MAC: return "mac";
		case SUPERH_MOV: return "mov";
		case SUPERH_MOVA: return "mova";
		case SUPERH_MOVCA: return "movca";
		case SUPERH_MOVCO: return "movco";
		case SUPERH_MOVI20: return "movi20";
		case SUPERH_MOVI20S: return "movi20s";
		case SUPERH_MOVLI: return "movli";
		case SUPERH_MOVML: return "movml";
		case SUPERH_MOVMU: return "movmu";
		case SUPERH_MOVRT: return "movrt";
		case SUPERH_MOVS: return "movs";
		case SUPERH_MOVT: return "movt";
		case SUPERH_MOVU: return "movu";
		case SUPERH_MOVUA: return "movua";
		case SUPERH_MOVX: return "movx";
		case SUPERH_MOVY: return "movy";
		case SUPERH_MUL: return "mul";
		case SUPERH_MULR: return "mulr";
		case SUPERH_MULS: return "muls";
		case SUPERH_MULU: return "mulu";
		case SUPERH_NEG: return "neg";
		case SUPERH_NEGC: return "negc";
		case SUPERH_NOP: return "nop";
		case SUPERH_NOPX: return "nopx";
		case SUPERH_NOPY: return "nopy";
		case SUPERH_NOT: return "not";
		case SUPERH_NOTT: return "nott";
		case SUPERH_OCBI: return "ocbi";
		case SUPERH_OCBP: return "ocbp";
		case SUPERH_OCBWB: return "ocbwb";
		case SUPERH_OR: return "or";
		case SUPERH_PABS: return "pabs";
		case SUPERH_PADD: return "padd";
		case SUPERH_PADDC: return "paddc";
		case SUPERH_PAND: return "pand";
		case SUPERH_PCLR: return "pclr";
		case SUPERH_PCMP: return "pcmp";
		case SUPERH_PCOPY: return "pcopy";
		case SUPERH_PDEC: return "pdec";
		case SUPERH_PDMSB: return "pdmsb";
		case SUPERH_PINC: return "pinc";
		case SUPERH_PLDS: return "plds";
		case SUPERH_PMULS: return "pmuls";
		case SUPERH_PNEG: return "pneg";
		case SUPERH_POR: return "por";
		case SUPERH_PREF: return "pref";
		case SUPERH_PREFI: return "prefi";
		case SUPERH_PRND: return "prnd";
		case SUPERH_PSHA: return "psha";
		case SUPERH_PSHL: return "pshl";
		case SUPERH_PSTS: return "psts";
		case SUPERH_PSUB: return "psub";
		case SUPERH_PSUBC: return "psubc";
		case SUPERH_PXOR: return "pxor";
		case SUPERH_RESBANK: return "resbank";
		case SUPERH_ROTCL: return "rotcl";
		case SUPERH_ROTCR: return "rotcr";
		case SUPERH_ROTL: return "rotl";
		case SUPERH_ROTR: return "rotr";
		case SUPERH_RTE: return "rte";
		case SUPERH_RTS: return "rts";
		case SUPERH_RTV: return "rtv";
		case SUPERH_SETRC: return "setrc";
		case SUPERH_SETS: return "sets";
		case SUPERH_SETT: return "sett";
		case SUPERH_SHAD: return "shad";
		case SUPERH_SHAL: return "shal";
		case SUPERH_SHAR: return "shar";
		case SUPERH_SHLD: return "shld";
		case SUPERH_SHLL: return "shll";
		case SUPERH_SHLL16: return "shll16";
		case SUPERH_SHLL2: return "shll2";
		case SUPERH_SHLL8: return "shll8";
		case SUPERH_SHLR: return "shlr";
		case SUPERH_SHLR16: return "shlr16";
		case SUPERH_SHLR2: return "shlr2";
		case SUPERH_SHLR8: return "shlr8";
		case SUPERH_SLEEP: return "sleep";
		case SUPERH_STBANK: return "stbank";
		case SUPERH_STC: return "stc";
		case SUPERH_STS: return "sts";
		case SUPERH_SUB: return "sub";
		case SUPERH_SUBC: return "subc";
		case SUPERH_SUBV: return "subv";
		case SUPERH_SWAP: return "swap";
		case SUPERH_SYNCO: return "synco";
		case SUPERH_TAS: return "tas";
		case SUPERH_TRAPA: return "trapa";
		case SUPERH_TST: return "tst";
		case SUPERH_XOR: return "xor";
		case SUPERH_XTRCT: return "xtrct";
		default:
			return "error";
	}
}
