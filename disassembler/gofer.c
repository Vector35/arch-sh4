#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "decode.h"
#include "format.h"

void disassemble(uint64_t addr, uint8_t *data, int len, char *result, bool verbose)
{
	Instruction instr;
	memset(&instr, 0, sizeof(instr));

	sh4_decompose(*(uint16_t *)data, &instr, addr);

	sh4_disassemble(&instr, result, 1024);
}
