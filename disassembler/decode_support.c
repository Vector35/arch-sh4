#include <stdint.h>

// get 8-bit signed int
int int8(uint16_t i)
{
	uint32_t result = i;
	if(i & 0x80)
		result = -(128 - (i&0x7F));
	return result;
}

// get 12-bit signed int
int int12(uint16_t i)
{
	int result = i;
	if(i & 0x800)
		result = -(2048 - (i&0x7FF));
	return result;
}

// displacement to effective address
uint64_t displ2ea(int scale, int disp, uint64_t base, int64_t mask)
{
	//printf("%s(%d, %d, 0x%016llx)\n", __func__, scale, disp, base);
	int64_t dest = (base & (~mask)) + 4 + (scale*disp);
	return dest;
}

void debug_pattern_matched(void)
{
	while(0);
}
