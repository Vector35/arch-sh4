/* exhaustive test of the sh4 disassembler */

/* */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* our stuff */
#include "disasm.h"

/* libopcodes stuff */
#define PACKAGE "bfd"
#define PACKAGE_VERSION "2.30"
#include <bfd.h> // for bfd_arch_arm, etc.
#include <dis-asm.h>


int cb_fprintf(void *stream, const char *fmt, ...)
{
	va_list args;
    va_start(args, fmt);
    char *built_str = (char *)stream;
	char buf[1024];
    int rc = vsnprintf(buf, sizeof(buf)-1, fmt, args);
    va_end(args);
    strcat((char *)built_str, buf);
    return rc;
}

int get_disasm_libopcodes(uint32_t addr, uint8_t *data, int len, char *result)
{
	/* initialize disassembler, if needed */
	static bool loc_init = false;
	static disassemble_info dinfo = {0};
	static disassembler_ftype disasm;
	if(!loc_init) {
		/* create disassemble info */
		init_disassemble_info(&dinfo, NULL, cb_fprintf);
		dinfo.flavour = bfd_target_unknown_flavour;
		dinfo.arch = bfd_arch_sh;
		dinfo.mach = bfd_mach_sh4;
		dinfo.endian = BFD_ENDIAN_LITTLE;
		disassemble_init_for_target(&dinfo); // reads dinfo.arch and populate extra stuff

		/* create disassembler */
		disasm = disassembler(bfd_arch_sh, TRUE, bfd_mach_sh4, NULL);
		if(!disasm) {
			printf("ERROR: disassembler() returned no function\n");
			return -1;
		}

		loc_init = true;
	}

	/* use the stream pointer as our private data
		(the buffer that fprintf() should append to */
	dinfo.stream = (void *)result;

	/* call disassembler
		will use callbacks in dinfo (like .read_memory_func, .print_addr_func, etc.)
		and the defaults are fine for this use case, see the defaults in a debugger
		or look at especially buffer_read_memory() in dis-buf.c for details */
	dinfo.octets_per_byte = 1;
	dinfo.buffer_vma = addr;
	dinfo.stop_vma = addr+2;
	/* source data */
	dinfo.buffer = data;
	dinfo.buffer_length = len;

	result[0] = '\0';
	disasm((bfd_vma)addr, &dinfo);

	return 0;
}

int main(int ac, char **av)
{
	char resultA[128], resultB[128];
	uint64_t addr = 0;
	//uint64_t addr = 0xFFFFFFFC;
	int maxlen = 0;

	uint16_t start=0, end=0xFFFF;

	if(ac > 1 && !strcmp(av[1], "speed")) {
		double delta;
		struct timespec t0,t1;
		uint16_t insword = 0;
		struct decomp_result dr;

		clock_gettime(CLOCK_MONOTONIC, &t0);
		#define NUM_TRIALS (10*1000*1000)
		for(int i=0; i<NUM_TRIALS; i++) {
			decompose(0, insword, &dr);
		}
		clock_gettime(CLOCK_MONOTONIC, &t1);

		delta = (double)(t1.tv_nsec - t0.tv_nsec) / 1000000000.0;
		delta += (double)t1.tv_sec - t0.tv_sec;
		printf("computed %d decompositions in %f wallclock seconds\n", NUM_TRIALS, delta);
		printf("that's %f trials/second\n", NUM_TRIALS/delta);

		return 0;
	}

	if(ac > 1) {
		start = strtoul(av[1], NULL, 16);
		end = strtoul(av[1], NULL, 16);
	}	

	for(uint16_t insword=start; 1; insword++) {
		/* result a is us */
		int rc = disassemble(addr, insword, resultA);
		/* result b is them */
		get_disasm_libopcodes(addr, (uint8_t *)&insword, 2, resultB);

		/* normalize */
		/* libopcodes: tabs to spaces */
		int l = strlen(resultB);
		for(int i=0; i<l; ++i)
			if(resultB[i]=='\t')
				resultB[i] = ' ';
		/* libopcodes: remove trailing space */
		if(resultB[l-1]==' ')
			resultB[l-1] = '\x0';

		/* print */
		printf("%016llx: %04X [%s] vs. [%s]\n", addr, insword, resultA, resultB);

		/* compare */
		bool match = false;
		if(rc == -1 && strncmp(resultB, ".word", 5) == 0)
			match = true;
		if(strcmp(resultA, resultB) == 0) {
			int tmp = strlen(resultA);
			if(tmp > maxlen) maxlen = tmp;
			match = true;
		}

		/* bail */
		if(!match) {
			printf("MISMATCH!\n");
			break;
		}

		if(insword == end) break;
	}

	printf("maximum length string is: %d\n", maxlen);
}
