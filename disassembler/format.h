#pragma once

#include <stddef.h>

int sh4_disassemble(Instruction *instruction, char *buf, size_t buf_sz);
