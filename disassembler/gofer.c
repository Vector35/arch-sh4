#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "decode.h"
#include "format.h"

void disassemble(uint64_t addr, uint8_t *data, int len, char *result)
{
	Instruction instr;
	memset(&instr, 0, sizeof(instr));

	superh_decompose(*(uint16_t *)data, &instr, addr);

	superh_disassemble(&instr, result, 1024);
}

