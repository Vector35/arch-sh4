#!/usr/bin/env python

# this tests disassembly through the binja API, ensuring binja <- arch <- disassembler
#
# that's an important distinction versus testing the disassembler in isolation

import re, struct, os, sys

import binaryninja
print("binaryninja.__file__:", binaryninja.__file__)

sys.path.append('./disassembler')
import disasm_test

arch = None
def disassemble(addr, insnum):
	global arch
	if not arch:
		arch = binaryninja.Architecture['sh4']
		print(arch)
	data = struct.pack('<H', insnum)
	(tokens, length) = arch.get_instruction_text(data, addr)
	if not tokens or length==0:
		return None
	return ''.join([x.text for x in tokens])

def main():
	if sys.argv[1:]:
		if sys.argv[1] in ['strain', 'strainer', 'stress', 'stresser']:
			for insnum in range(0, 2**32):
				result = disassemble(0, insnum)
				if insnum & 0xFFFFFF == 0:
					print('%08X: %s' % (insnum, result))
		else:
			insnum = int(sys.argv[1], 16)
			print(disassemble(0, insnum))

	else:
		with open('./disassembler/test_cases.txt') as fp:
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
			print('0x%08X -%s- vs. -%s-' % (insnum, actual, expected))
			if actual != expected:
				print('line %d/%d (%.2f%%)' % (i, len(lines), i/len(lines)*100))
				sys.exit(-1)

		print('PASS')

if __name__ == '__main__':
	main()

