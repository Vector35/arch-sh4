#pragma once

#include <stddef.h>

extern const char *superh_reg_strs[];
extern const char *superh_cmp_cond_strs[];
extern const char *superh_len_suffix_strs[];

#ifdef __cplusplus
extern "C" {
#endif
int superh_disassemble(Instruction *instruction, char *buf, size_t buf_sz);
#ifdef __cplusplus
}
#endif
