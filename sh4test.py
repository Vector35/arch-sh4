#!/usr/bin/env python

# this tests some select LIFTING

RET = b'\x0b\x00' # rts (little endian)

# these test cases must be little-endian encoded, since we're using the linux-sh4 platform
test_cases = [
	# (b'', '?'),

	# 00000000: DE 73 mov.l 0x00000000000001d0,r14
	(b'\x73\xde', 'LLIL_SET_REG.d(r14,LLIL_LOAD.d(LLIL_CONST.d(0x1D0)))'),
	# 00000000: D6 5F mov.l 0x0000000000000180,r6
	(b'\x5f\xd6', 'LLIL_SET_REG.d(r6,LLIL_LOAD.d(LLIL_CONST.d(0x180)))'),
	# 00000000: D0 AF mov.l 0x00000000000002c0,r0
	(b'\xaf\xd0', 'LLIL_SET_REG.d(r0,LLIL_LOAD.d(LLIL_CONST.d(0x2C0)))'),
	# 00000000: D0 D8 mov.l 0x0000000000000364,r0
	(b'\xd8\xd0', 'LLIL_SET_REG.d(r0,LLIL_LOAD.d(LLIL_CONST.d(0x364)))'),

	# 00000000: E2 B5 mov #-75,r2
	(b'\xB5\xE2', 'LLIL_SET_REG.d(r2,LLIL_CONST.d(0xFFFFFFB5))'),
	# 00000000: EE 61 mov #97,r14
	(b'\x61\xEE', 'LLIL_SET_REG.d(r14,LLIL_CONST.d(0x61))'),
	# 00000000: E7 7B mov #123,r7
	(b'\x7B\xE7', 'LLIL_SET_REG.d(r7,LLIL_CONST.d(0x7B))'),
	# 00000000: EE 77 mov #119,r14
	(b'\x77\xEE', 'LLIL_SET_REG.d(r14,LLIL_CONST.d(0x77))'),

	# 00000000: 4A 2B jmp @r10
	(b'\x2b\x4a', 'LLIL_JUMP(LLIL_REG.d(r10))'),	
	# 00000000: 4F 2B jmp @r15
	(b'\x2b\x4f', 'LLIL_JUMP(LLIL_REG.d(r15))'),	
	# 00000000: 43 2B jmp @r3
	(b'\x2b\x43', 'LLIL_JUMP(LLIL_REG.d(r3))'),	
	# 00000000: 41 2B jmp @r1
	(b'\x2b\x41', 'LLIL_JUMP(LLIL_REG.d(r1))'),	
	# 00000000: 06 03 bsrf r6
	(b'\x03\x06', 'LLIL_CALL(LLIL_ADD.d(LLIL_ADD.d(LLIL_REG.d(pc),LLIL_CONST.d(0x4)),LLIL_REG.d(r6)))'),	
	# 00000000: 01 03 bsrf r1
	(b'\x03\x01', 'LLIL_CALL(LLIL_ADD.d(LLIL_ADD.d(LLIL_REG.d(pc),LLIL_CONST.d(0x4)),LLIL_REG.d(r1)))'),	
	# 00000000: 0F 03 bsrf r15
	(b'\x03\x0F', 'LLIL_CALL(LLIL_ADD.d(LLIL_ADD.d(LLIL_REG.d(pc),LLIL_CONST.d(0x4)),LLIL_REG.d(r15)))'),	
	# 00000000: 00 03 bsrf r0
	(b'\x03\x00', 'LLIL_CALL(LLIL_ADD.d(LLIL_ADD.d(LLIL_REG.d(pc),LLIL_CONST.d(0x4)),LLIL_REG.d(r0)))'),	
	# 00000000: B9 1D bsr 0xfffffffffffff23e
	(b'\x1d\xb9', 'LLIL_CALL(LLIL_CONST.d(0xFFFFF23E))'),
	# 00000000: BE F1 bsr 0xfffffffffffffde6
	(b'\xf1\xbe', 'LLIL_CALL(LLIL_CONST.d(0xFFFFFDE6))'),
	# 00000000: B6 13 bsr 0x0000000000000c2a
	(b'\x13\xb6', 'LLIL_CALL(LLIL_CONST.d(0xC2A))'),
	# 00000000: B8 65 bsr 0xfffffffffffff0ce
	(b'\x65\xb8', 'LLIL_CALL(LLIL_CONST.d(0xFFFFF0CE))'),
	# jsr @r4
	(b'\x0b\x44', 'LLIL_CALL(LLIL_REG.d(r4))'),
	# jsr @r10
	(b'\x0b\x4A', 'LLIL_CALL(LLIL_REG.d(r10))'),
	# jsr @r12
	(b'\x0b\x4C', 'LLIL_CALL(LLIL_REG.d(r12))'),
	# jsr @r5
	(b'\x0b\x45', 'LLIL_CALL(LLIL_REG.d(r5))'),
	# rts
	(b'\x0b\x00', '') # expected empty, since RTS function should be blank
]

import re
import sys
import binaryninja
from binaryninja import binaryview
from binaryninja import lowlevelil
from binaryninja.enums import LowLevelILOperation

def il2str(il):
	sz_lookup = {1:'.b', 2:'.w', 4:'.d', 8:'.q', 16:'.o'}
	if isinstance(il, lowlevelil.LowLevelILInstruction):
		size_suffix = sz_lookup.get(il.size, '?') if il.size else ''
		# print size-specified IL constants in hex
		if il.operation in [LowLevelILOperation.LLIL_CONST, LowLevelILOperation.LLIL_CONST_PTR] and il.size:
			tmp = il.operands[0]
			if tmp < 0: tmp = (1<<(il.size*8))+tmp
			tmp = '0x%X' % tmp if il.size else '%d' % il.size
			return 'LLIL_CONST%s(%s)' % (size_suffix, tmp)
		else:
			return '%s%s(%s)' % (il.operation.name, size_suffix, ','.join([il2str(o) for o in il.operands]))
	elif isinstance(il, list):
		return '[' + ','.join([il2str(x) for x in il]) + ']'
	else:
		return str(il)

# TODO: make this less hacky
def instr_to_il(data):
	platform = binaryninja.Platform['linux-sh4']
	# make a pretend function that returns
	bv = binaryview.BinaryView.new(data + RET)
	bv.add_function(0, plat=platform)
	assert len(bv.functions) == 1

	result = []
	for block in bv.functions[0].low_level_il:
		for il in block:
			result.append(il2str(il))
	result = '; '.join(result)
	if result.endswith('LLIL_RET(LLIL_REG.d(pr))'):
		result = result[0:result.index('LLIL_RET(LLIL_REG.d(pr))')]
	if result.endswith('; '):
		result = result[0:-2]

	return result

def il_str_to_tree(ilstr):
	result = ''
	depth = 0
	for c in ilstr:
		if c == '(':
			result += '\n'
			depth += 1
			result += '    '*depth
		elif c == ')':
			depth -= 1
		elif c == ',':
			result += '\n'
			result += '    '*depth
			pass
		else:
			result += c
	return result

def test_all():
	for (test_i, (data, expected)) in enumerate(test_cases):
		actual = instr_to_il(data)
		if actual != expected:
			print('MISMATCH AT TEST %d!' % test_i)
			print('\t   input: %s' % data.hex())
			print('\texpected: %s' % expected)
			print('\t  actual: %s' % actual)
			print('\t    tree:')
			print(il_str_to_tree(actual))

			return False

	return True

if __name__ == '__main__':
	if test_all():
		print('success!')
		sys.exit(0)
	else:
		sys.exit(-1)

if __name__ == 'arm64test':
	if test_all():
		print('success!')

