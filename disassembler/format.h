#pragma once

#include <stddef.h>

extern const char *sh4_opc_strs[];
extern const char *sh4_reg_strs[];
extern const char *sh4_cmp_cond_strs[];
extern const char *sh4_len_suffix_strs[];

#ifdef __cplusplus
extern "C" {
#endif
int sh4_disassemble(Instruction *instruction, char *buf, size_t buf_sz);
#ifdef __cplusplus
}
#endif
