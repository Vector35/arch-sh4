int int8(uint16_t i);
int int12(uint16_t i);
uint64_t displ2ea(int scale, int disp, uint64_t base, uint64_t mask);
void debug_pattern_matched(void);

#define SUPPORTED(X) (ctx->feature_flags & (X))
