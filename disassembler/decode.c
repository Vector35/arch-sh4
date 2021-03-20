#include "decode.h"

int decode_spec(context *, Instruction *);

int superh_decompose(uint16_t insword, struct Instruction *instr, uint64_t addr)
{
	context ctx = {insword, addr, false, false, SH4};
	return decode_spec(&ctx, instr);
}
