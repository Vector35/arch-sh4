#!/usr/bin/env python

# run disassembly tests in test_cases.txt
# ensure you built gofer.so (see Makefile-local)

import os, sys, struct, ctypes, re

RED = '\x1B[31m'
GREEN = '\x1B[32m'
NORMAL = '\x1B[0m'

#------------------------------------------------------------------------------
# disassemble
#------------------------------------------------------------------------------

(dll, pfunc_disasm, cbuf) = (None, None, None)

def disassemble(addr, insnum):
	global cbuf, pfunc_disasm

	if not pfunc_disasm:
		cbuf = ctypes.create_string_buffer(1024)
		dll = ctypes.CDLL(os.path.join(os.path.dirname(__file__), 'gofer.so'))
		pfunc_disasm = dll.disassemble
		pfunc_disasm.restype = ctypes.c_int
		pfunc_disasm.argtypes = [ctypes.c_uint64, ctypes.c_char_p, ctypes.c_uint, ctypes.c_void_p]

	insword = struct.pack('<I', insnum)
	cbuf.value = b'(uninitialized)'
	pfunc_disasm(addr, insword, 4, ctypes.byref(cbuf), False)
	return cbuf.value.decode('utf-8').strip()

#------------------------------------------------------------------------------
# main
#------------------------------------------------------------------------------

def main():
	if sys.argv[1:]:
		insnum = int(sys.argv[1], 16)
		print(disassemble(0, insnum))

	else:
		with open('test_cases.txt') as fp:
			lines = fp.readlines()

		for (i,line) in enumerate(lines):
			if line.startswith('//') or line.startswith('#'):
				print(line, end='')
				continue

			m = re.match(r'^(........): (..) (..) (.*)', line)
			(addr, b1, b0, expected) = m.group(1,2,3,4)
			addr = int(addr, 16)
			insnum = (int(b1,16)<<8) | int(b0,16)

			actual = disassemble(addr, insnum)
			print('%08X: %04X -%s- vs -%s-' % (addr, insnum, actual, expected))
			if actual != expected:
				print('0x%08X' % insnum)
				print('  actual:', RED, actual, NORMAL)
				print('expected:', GREEN, expected, NORMAL)
				print('line %d/%d (%.2f%%)' % (i, len(lines), i/len(lines)*100))
				sys.exit(-1)

		print('PASS')

if __name__ == '__main__':
	main()

