#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "binaryninjaapi.h"
#include "lowlevelilinstruction.h"
using namespace BinaryNinja; // for ::LogDebug, etc.
using namespace std;

//#define printf(...) while(0);

extern "C" {
#include "decode.h"
#include "format.h"
}

#define IL_FLAG_T 1

#define IL_FLAG_WRITE_NONE 0
#define IL_FLAG_WRITE_ALL  1
#define IL_FLAG_WRITE_ALL_FLOAT  2

#define IL_FLAG_CLASS_INT 1
#define IL_FLAG_CLASS_FLOAT 2

bool GetLowLevelILForInstruction(Architecture* arch, uint64_t addr, LowLevelILFunction& il, Instruction& instr, size_t addrSize)
{
	switch(instr.operation) {

		/* there are 3 "call subroutine" instructions in SH4 */
		/* (1/3) JumpSubRoutine <reg> */
		/* eg: 0B 41    jsr @r1 */
		case SUPERH_JSR:
			il.AddInstruction(il.Call(il.Register(4, instr.operands[0].regA)));
			break;

		/* (2/3) branch subroutine, PC = (PC+4) + 2*disp */
		case SUPERH_BSR:
			il.AddInstruction(il.Call(il.ConstPointer(4, instr.operands[0].address)));
			break;

		/* (3/3) branch subroutine far, PC = (PC+4) + Rn */
		case SUPERH_BSRF:
			il.AddInstruction(il.Call(
				il.Add(4,
					il.Add(4, il.Register(4, PC), il.Const(4, 4)),
					il.Register(4, instr.operands[0].regA)
				)
			));
			break;

		case SUPERH_JMP:
			il.AddInstruction(il.Jump(il.Register(4, instr.operands[0].regA)));
			break;

		case SUPERH_BT:
		{
			BNLowLevelILLabel* trueLabel = il.GetLabelForAddress(arch, instr.operands[0].address);
			BNLowLevelILLabel* falseLabel = il.GetLabelForAddress(arch, instr.delay_slot ? addr + 4 : addr + 2);
			il.AddInstruction(
				il.If(
					il.CompareNotEqual(1,
						il.Flag(IL_FLAG_T), il.Const(1, 0)),
					*trueLabel, *falseLabel)
			);
			break;
		}

		case SUPERH_BF:
		{
			BNLowLevelILLabel* trueLabel = il.GetLabelForAddress(arch, instr.operands[0].address);
			BNLowLevelILLabel* falseLabel = il.GetLabelForAddress(arch, instr.delay_slot ? addr + 4 : addr + 2);
			il.AddInstruction(
				il.If(
					il.CompareEqual(1,
						il.Flag(IL_FLAG_T), il.Const(1, 0)),
					*trueLabel, *falseLabel)
			);
			break;
		}

		case SUPERH_MOV:
			/* eg: 00 EE    mov #0, r14 */
			if(instr.operands_n == 2 && instr.operands[0].type == IMMEDIATE && instr.operands[1].type == GPREG)
				il.AddInstruction(il.SetRegister(4,
					instr.operands[1].regA,
					il.Const(4, instr.operands[0].immediate)
				));
			else
			/* eg: 04 D4    mov.l 0x004021f2, r4 */
			if(instr.operands_n == 2 && instr.operands[0].type == ADDRESS && instr.operands[1].type == GPREG)
				il.AddInstruction(il.SetRegister(4,
					instr.operands[1].regA,
					il.Load(4, il.ConstPointer(4, instr.operands[0].address))
				));
			else
			/* eg: 53 60   mov r5,r0 */
			if(instr.operands_n == 2 && instr.operands[0].type == GPREG && instr.operands[1].type == GPREG)
				il.AddInstruction(il.SetRegister(4,
					instr.operands[1].regA,
					il.Register(4, instr.operands[0].regA)
				));
			else
				il.AddInstruction(il.Unimplemented());

			break;

		/* return subroutine */
		case SUPERH_RTS:
			il.AddInstruction(il.Return(il.Register(4, PR)));
			break;

		case SUPERH_ADD:
			if(instr.operands_n == 2 && instr.operands[0].type == IMMEDIATE && instr.operands[1].type == GPREG)
				il.AddInstruction(il.SetRegister(4,
					instr.operands[1].regA,
					il.Add(4,
						il.Register(4, instr.operands[1].regA),
						il.SignExtend(4,
							il.Const(1, instr.operands[0].immediate)))
					// il.SignExtend(4,
					// 	il.Add(4, il.Register(4, instr.operands[1].regA),
					// 		il.Const(1, instr.operands[0].immediate)))
				));
			else
			/* eg: 04 D4    mov.l 0x004021f2, r4 */
			if(instr.operands_n == 2 && instr.operands[0].type == GPREG && instr.operands[1].type == GPREG)
				il.AddInstruction(il.SetRegister(4,
					instr.operands[1].regA,
					il.Add(4, il.Register(4, instr.operands[1].regA),
						il.Register(4, instr.operands[0].regA))
				));
			else
				il.AddInstruction(il.Unimplemented());
			break;

		case SUPERH_TST:
			if(instr.operands_n == 2 && instr.operands[0].type == IMMEDIATE && instr.operands[1].type == GPREG)
				il.AddInstruction(il.SetFlag(IL_FLAG_T,
					il.CompareEqual(4,
						il.Const(4, 0),
						il.And(4,
							il.Register(4, instr.operands[1].regA),
							il.ZeroExtend(4,
								il.Const(1, instr.operands[0].immediate))))
				));
			else
			/* eg: 04 D4    mov.l 0x004021f2, r4 */
			if(instr.operands_n == 2 && instr.operands[0].type == GPREG && instr.operands[1].type == GPREG)
				il.AddInstruction(il.SetFlag(IL_FLAG_T,
					il.CompareEqual(4,
						il.Const(4, 0),
						il.And(4,
							il.Register(4, instr.operands[0].regA),
							il.Register(4, instr.operands[1].regA)))
				));
			else
				il.AddInstruction(il.Unimplemented());
			break;

		case SUPERH_CMP:
			switch (instr.cond)
			{
			case CMP_COND_EQ:
				il.AddInstruction(il.SetFlag(IL_FLAG_T,
					il.CompareEqual(4,
						instr.operands[0].type == IMMEDIATE
							? il.Const(4, instr.operands[0].immediate)
							: il.Register(4, instr.operands[0].regA),
						il.Register(4, instr.operands[1].regA))
					));
				break;
			case CMP_COND_GE:
				il.AddInstruction(il.SetFlag(IL_FLAG_T,
					il.CompareSignedGreaterEqual(4,
						il.Register(4, instr.operands[0].regA),
						il.Register(4, instr.operands[1].regA))
					));
				break;
			case CMP_COND_GT:
				il.AddInstruction(il.SetFlag(IL_FLAG_T,
					il.CompareSignedGreaterThan(4,
						il.Register(4, instr.operands[0].regA),
						il.Register(4, instr.operands[1].regA))
					));
				break;
			case CMP_COND_HI:
				il.AddInstruction(il.SetFlag(IL_FLAG_T,
					il.CompareUnsignedGreaterThan(4,
						il.Register(4, instr.operands[0].regA),
						il.Register(4, instr.operands[1].regA))
					));
				break;
			case CMP_COND_HS:
				il.AddInstruction(il.SetFlag(IL_FLAG_T,
					il.CompareUnsignedGreaterEqual(4,
						il.Register(4, instr.operands[0].regA),
						il.Register(4, instr.operands[1].regA))
					));
				break;
			case CMP_COND_PL:
				il.AddInstruction(il.SetFlag(IL_FLAG_T,
					il.CompareSignedGreaterThan(4,
						il.Register(4, instr.operands[0].regA),
						il.Const(4, 0))
					));
				break;
			case CMP_COND_PZ:
				il.AddInstruction(il.SetFlag(IL_FLAG_T,
					il.CompareSignedGreaterEqual(4,
						il.Register(4, instr.operands[0].regA),
						il.Const(4, 0))
					));
				break;
			case CMP_COND_STR:
			default:
				il.AddInstruction(il.Unimplemented());
				break;
			}
			break;

		default:
			il.AddInstruction(il.Unimplemented());
	}

	return true;
}

/*****************************************************************************/
/* the architecture class */
/*****************************************************************************/

class SH4Architecture: public Architecture
{
	private:
	BNEndianness endian;

	BNRegisterInfo RegisterInfo(uint32_t fullWidthReg, size_t offset, size_t size, bool zeroExtend = false)
	{
		BNRegisterInfo result;
		result.fullWidthRegister = fullWidthReg;
		result.offset = offset;
		result.size = size;
		result.extend = zeroExtend ? ZeroExtendToFullWidth : NoExtend;
		return result;
	}

	public:

	/* initialization list */
	SH4Architecture(const char* name, BNEndianness endian_): Architecture(name)
	{
		endian = endian_;
	}

	/*************************************************************************/

	virtual BNEndianness GetEndianness() const override
	{
		//printf("%s()\n", __func__);
		return endian;
	}

	virtual size_t GetAddressSize() const override
	{
		//printf("%s()\n", __func__);
		return 4; // 4 bytes, 32-bits
	}

	virtual size_t GetDefaultIntegerSize() const override
	{
		return 4; // 4 bytes, 32-bits
	}

	virtual size_t GetInstructionAlignment() const override
	{
		return 2;
	}

	/* the maximum length of an instruction
		(how much for binja to buffer before calling GetInstructionInfo()) */
	virtual size_t GetMaxInstructionLength() const override
	{
		return 2;
	}

	/* think "GetInstructionBranchBehavior()"

	   populates InstructionInfo (api/binaryninjaapi.h)
	   which extends struct BNInstructionInfo (core/binaryninjacore.h)

	   tasks:
		1) set the length
		2) invoke AddBranch() for every non-sequential execution possibility

	   */
	virtual bool GetInstructionInfo(const uint8_t* data, uint64_t addr,
		size_t maxLen, InstructionInfo& result) override
	{
		// char tmp[32];
		Instruction instr;
		memset(&instr, 0, sizeof(instr));
		uint16_t insword = *(uint16_t *)data;
		if(endian == BigEndian)
			insword = (data[0]<<8) | data[1];
		if(superh_decompose(insword, &instr, addr) != 0)
			return false;

		// result.branchDelay = instr.delay_slot;

		switch(instr.operation) {
			case SUPERH_BSR:
			case SUPERH_BSRF:
			case SUPERH_JSR:
				if(instr.operands_n == 1)
					result.AddBranch(CallDestination, instr.operands[0].address, nullptr, instr.delay_slot);
				else
					result.AddBranch(UnresolvedBranch, 0, nullptr, instr.delay_slot);
				break;

			case SUPERH_BRA:
			case SUPERH_JMP:
				if(instr.operands_n == 1)
					result.AddBranch(UnconditionalBranch, instr.operands[0].address, nullptr, instr.delay_slot);
				else
					result.AddBranch(UnresolvedBranch, 0, nullptr, instr.delay_slot);
				break;

			case SUPERH_BT:
			case SUPERH_BF:
				result.AddBranch(TrueBranch, instr.operands[0].address, nullptr, instr.delay_slot);
				// result.AddBranch(FalseBranch, addr+2, nullptr, instr.delay_slot);
				if (instr.delay_slot)
					result.AddBranch(FalseBranch, addr+4, nullptr, instr.delay_slot);
				else
					result.AddBranch(FalseBranch, addr+2, nullptr, instr.delay_slot);
				break;

			case SUPERH_RTS:
				result.AddBranch(FunctionReturn, 0, nullptr, instr.delay_slot);
				break;

			default:
				break;
		}

		//printf("%s(data, addr=%llX, maxLen=%zu, result) parses ", __func__, addr, maxLen);
		result.length = 2;
		return true;
	}

	virtual bool GetInstructionText(const uint8_t* data, uint64_t addr,
	  size_t& len, vector<InstructionTextToken>& result) override
	{
		//printf("%s(%02X %02X, 0x%llX, 0x%zX, il)\n", __func__, data[0], data[1], addr, len);
		char buf[32];

		/* decompose instruction */
		Instruction instr;
		memset(&instr, 0, sizeof(instr));
		uint16_t insword = *(uint16_t *)data;
		if(endian == BigEndian)
			insword = (data[0]<<8) | data[1];
		if(superh_decompose(insword, &instr, addr) != 0)
			return false;

		/* opcode */
		strcpy(buf, operation_to_str(instr.operation));
		/* size modifier suffixes {.b, .w, .l} */
		if(instr.length_suffix == LEN_SUFFIX_B)
			strcat(buf, ".b");
		if(instr.length_suffix == LEN_SUFFIX_W)
			strcat(buf, ".w");
		if(instr.length_suffix == LEN_SUFFIX_L)
			strcat(buf, ".l");
		/* delay slot suffix .s or /s from doc */
		if(instr.delay_slot)
			strcat(buf, ".s");
		/* conditional "/xxx" if this is a cmp */
		if(instr.operation == SUPERH_CMP || instr.operation == SUPERH_FCMP) {
			if(instr.cond > CMP_COND_NONE && instr.cond < CMP_COND_MAXIMUM) {
				strcat(buf, "/");
				strcat(buf, superh_cmp_cond_strs[instr.cond]);
			}
		}
		/* done */
		result.emplace_back(InstructionToken, buf);

		/* operands */
		if(instr.operands_n)
			result.emplace_back(TextToken, " ");

		for(int i=0; i < (instr.operands_n); ++i) {
			char buf[32];

			switch(instr.operands[i].type) {
				case GPREG:
				case BANKREG:
				case CTRLREG:
				case SYSREG:
				case FPUREG:
					result.emplace_back(RegisterToken, superh_reg_strs[instr.operands[i].regA]);
					break;

				case IMMEDIATE:
					result.emplace_back(TextToken, "#");
					snprintf(buf, sizeof(buf), "%d", instr.operands[i].immediate);
					result.emplace_back(IntegerToken, buf);
					break;

				case ADDRESS:
					snprintf(buf, sizeof(buf), "0x%016llx", instr.operands[i].address);
					result.emplace_back(PossibleAddressToken, buf);
					break;

				case DEREF_REG:
					result.emplace_back(BeginMemoryOperandToken, "@");

					if(instr.operands[i].flags & SUPERH_FLAG_PRE_INCREMENT)
						result.emplace_back(TextToken, "+");
					if(instr.operands[i].flags & SUPERH_FLAG_PRE_DECREMENT)
						result.emplace_back(TextToken, "-");

					result.emplace_back(RegisterToken, superh_reg_strs[instr.operands[i].regA]);

					if(instr.operands[i].flags & SUPERH_FLAG_POST_INCREMENT)
						result.emplace_back(TextToken, "+");
					if(instr.operands[i].flags & SUPERH_FLAG_POST_DECREMENT)
						result.emplace_back(TextToken, "-");
					break;

				case DEREF_REG_REG:
					result.emplace_back(BeginMemoryOperandToken, "@(");
					result.emplace_back(RegisterToken, superh_reg_strs[instr.operands[i].regA]);
					result.emplace_back(OperandSeparatorToken, ",");
					result.emplace_back(RegisterToken, superh_reg_strs[instr.operands[i].regB]);
					result.emplace_back(EndMemoryOperandToken, ")");
					break;

				case DEREF_REG_IMM:
					result.emplace_back(BeginMemoryOperandToken, "@(");
					snprintf(buf, sizeof(buf), "%d", instr.operands[i].displacement);
					result.emplace_back(IntegerToken, buf);
					result.emplace_back(OperandSeparatorToken, ",");
					result.emplace_back(RegisterToken, superh_reg_strs[instr.operands[i].regA]);
					result.emplace_back(EndMemoryOperandToken, ")");
					break;

				default:
					LogError("unknown operand type\n");
					break;
			}

			if(i != instr.operands_n-1)
				result.emplace_back(OperandSeparatorToken, ",");
		}

		return true;
	}

	virtual bool GetInstructionLowLevelIL(const uint8_t* data, uint64_t addr, size_t& len, LowLevelILFunction& il) override
	{
		//printf("%s()\n", __func__);

		/* decompose instruction */
		Instruction instr;
		memset(&instr, 0, sizeof(instr));
		uint16_t insword = *(uint16_t *)data;
		if(endian == BigEndian)
			insword = (data[0]<<8) | data[1];
		if(superh_decompose(insword, &instr, addr) != 0) {
			return false;
		}
		if(!(instr.operation > SUPERH_ERROR && instr.operation <= SUPERH_XTRCT)) {
			return false;
		}

		if (instr.delay_slot)
		{
			if (len < 4)
			{
				LogWarn("Can not lift instruction with delay slot @ 0x%08" PRIx64, addr);
				return false;
			}

			Instruction secondInstr;
			memset(&secondInstr, 0, sizeof(secondInstr));
			uint16_t insword2 = *(uint16_t *)(data + 2);
			if(endian == BigEndian)
				insword2 = (data[2]<<8) | data[3];
			if(superh_decompose(insword2, &secondInstr, addr + 2) != 0) {
				il.AddInstruction(il.Undefined());
				return false;
			}

			bool status = true;
			if (false && (instr.operation == SUPERH_BT || instr.operation == SUPERH_BF))
			{
				InstructionInfo instrInfo;
				LowLevelILLabel trueCode, falseCode;
				GetInstructionInfo(data, addr, len, instrInfo);
				il.AddInstruction(
					il.If(instr.operation == SUPERH_BT
						? il.CompareNotEqual(1, il.Flag(IL_FLAG_T), il.Const(1, 0))
						: il.CompareEqual(1, il.Flag(IL_FLAG_T), il.Const(1, 0)),
					trueCode, falseCode)
				);
				il.MarkLabel(trueCode);
				il.SetCurrentAddress(this, addr + instr.size);
				GetLowLevelILForInstruction(this, addr + instr.size, il, secondInstr, GetAddressSize());
				for (size_t i = 0; i < instrInfo.branchCount; i++)
				{
					// if (instrInfo.branchType[i] == TrueBranch)
					{
						BNLowLevelILLabel* trueLabel = il.GetLabelForAddress(this, instrInfo.branchTarget[i]);
						if (trueLabel)
							il.AddInstruction(il.Goto(*trueLabel));
						else
							il.AddInstruction(il.Jump(il.ConstPointer(GetAddressSize(), instrInfo.branchTarget[i])));
						break;
					}
				}
				il.MarkLabel(falseCode);
			}
			else
			{
				size_t nop;

				// ensure we have space to preserve one register in case the delay slot
				// clobbers a value needed by the branch. this will be eliminated when
				// normal LLIL is generated from Lifted IL if we don't need it
				il.SetCurrentAddress(this, addr + instr.size);
				nop = il.Nop();
				il.AddInstruction(nop);

				GetLowLevelILForInstruction(this, addr + instr.size, il, secondInstr, GetAddressSize());

				LowLevelILInstruction delayed;
				uint32_t clobbered = BN_INVALID_REGISTER;
				size_t instrIdx = il.GetInstructionCount();
				if (instrIdx != 0)
				{
					// FIXME: this assumes that the instruction in the delay slot
					// only changed registers in the last IL instruction that it
					// added -- strictly speaking we should be starting from the
					// first instruction that could have been added and follow all
					// paths to the end of that instruction.
					delayed = il.GetInstruction(instrIdx - 1);
					if ((delayed.operation == LLIL_SET_REG) && (delayed.address == (addr + instr.size)))
						clobbered = delayed.GetDestRegister<LLIL_SET_REG>();
					else if ((delayed.operation == LLIL_SET_FLAG) && (delayed.address == (addr + instr.size)))
						clobbered = delayed.GetDestFlag<LLIL_SET_FLAG>();
				}

				il.SetCurrentAddress(this, addr);

				// if ((instr.operation == MIPS_JR) && (instr.operands[0].reg == REG_T9) &&
				// 		(secondInstr.operation == MIPS_ADDIU) && (secondInstr.operands[0].reg == REG_SP) &&
				// 		(secondInstr.operands[1].reg == REG_SP) && (secondInstr.operands[2].immediate < 0x80000000))
				// {
				// 	il.AddInstruction(il.TailCall(il.Register(4, REG_T9)));
				// }
				if (false) {}
				else
				{
					status = GetLowLevelILForInstruction(this, addr, il, instr, GetAddressSize());
				}

				if (clobbered != BN_INVALID_REGISTER)
				{
					// FIXME: this approach will break with any of the REG_SPLIT operations as well
					// any use of partial registers -- this approach needs to be expanded substantially
					// to be correct in the general case. also, it uses LLIL_TEMP(1) for the simple reason
					// that the mips lifter only uses LLIL_TEMP(0) at the moment.
					LowLevelILInstruction lifted = il.GetInstruction(instrIdx);
					if ((lifted.operation == LLIL_IF || lifted.operation == LLIL_CALL) && (lifted.address == addr))
					{
						int replace = 0;

						lifted.VisitExprs([&](const LowLevelILInstruction& expr) -> bool {
							if (expr.operation == LLIL_REG && expr.GetSourceRegister<LLIL_REG>() == clobbered)
							{
								// Replace all reads from the clobbered register to a temp register
								// that we're going to set (by replacing the earlier nop we added)
								il.ReplaceExpr(expr.exprIndex, il.Register(expr.size, LLIL_TEMP(1)));
								replace = LLIL_REG;
							}
							else if (expr.operation == LLIL_FLAG && expr.GetSourceFlag<LLIL_FLAG>() == clobbered)
							{
								// Replace all reads from the clobbered register to a temp register
								// that we're going to set (by replacing the earlier nop we added)
								il.ReplaceExpr(expr.exprIndex, il.Register(expr.size, LLIL_TEMP(1)));
								replace = LLIL_FLAG;
							}
							return true;
						});

						if (replace)
						{
							// Preserve the value of the clobbered register by replacing the LLIL_NOP
							// instruction we added at the beginning with an assignment to the temp
							// register we rewrote in the LLIL_IF condition expression
							il.SetCurrentAddress(this, addr + instr.size);
							if (replace == LLIL_REG)
								il.ReplaceExpr(nop, il.SetRegister(delayed.size, LLIL_TEMP(1), il.Register(delayed.size, delayed.GetDestRegister<LLIL_SET_REG>())));
							else
								il.ReplaceExpr(nop, il.SetRegister(delayed.size, LLIL_TEMP(1), il.Flag(delayed.GetDestFlag<LLIL_SET_FLAG>())));
							il.SetCurrentAddress(this, addr);
						}
					}
				}
			}

			len = instr.size + secondInstr.size;
			return status;
		}

		len = instr.size;
		return GetLowLevelILForInstruction(this, addr, il, instr, GetAddressSize());
	}


	virtual size_t GetFlagWriteLowLevelIL(BNLowLevelILOperation op, size_t size, uint32_t flagWriteType,
		uint32_t flag, BNRegisterOrConstant* operands, size_t operandCount, LowLevelILFunction& il) override
	{
		//printf("%s()\n", __func__);
		return il.Unimplemented();
	}

	virtual string GetRegisterName(uint32_t regId) override
	{
		//printf("%s()\n", __func__);
		if(regId > SUPERH_REGISTER_NONE && regId < SUPERH_REGISTER_MAXIMUM)
			return superh_reg_strs[regId];

		return "unknown";
	}

	virtual vector<uint32_t> GetAllFlags() override
	{
		//printf("%s()\n", __func__);
		return vector<uint32_t> {IL_FLAG_T};
	}

	virtual string GetFlagName(uint32_t flag) override
	{
		//printf("%s(%d)\n", __func__, flag);
		switch (flag)
		{
		case IL_FLAG_T:
			return "T";
		}
		return "ERR_FLAG_NAME";
	}

	/* flag write types */

	virtual vector<uint32_t> GetAllFlagWriteTypes() override
	{
		return vector<uint32_t> {IL_FLAG_WRITE_ALL, IL_FLAG_WRITE_ALL_FLOAT};
	}


	virtual string GetFlagWriteTypeName(uint32_t flags) override
	{
		switch (flags)
		{
		case IL_FLAG_WRITE_ALL:
			return "*";
		case IL_FLAG_WRITE_ALL_FLOAT:
			return "f*";
		default:
			return "";
		}
	}

	virtual vector<uint32_t> GetFlagsWrittenByFlagWriteType(uint32_t writeType) override
	{
		switch (writeType)
		{
		case IL_FLAG_WRITE_ALL:
		case IL_FLAG_WRITE_ALL_FLOAT:
			return vector<uint32_t> {IL_FLAG_T};
		default:
			return vector<uint32_t> {};
		}
	}

	virtual BNFlagRole GetFlagRole(uint32_t flag, uint32_t semClass = 0) override
	{
		//printf("%s(%d)\n", __func__, flag);
		return SpecialFlagRole;
	}

	virtual vector<uint32_t> GetFlagsRequiredForFlagCondition(BNLowLevelILFlagCondition cond, uint32_t semClass = 0) override
	{
		//printf("%s(%d)\n", __func__, cond);
		return vector<uint32_t>{};
	}

	virtual vector<uint32_t> GetFullWidthRegisters() override
	{
		//printf("%s()\n", __func__);
		return vector<uint32_t> {
			R0, R1, R2, R3, R4, R5, R6, R7,
			R8, R9, R10, R11, R12, R13, R14, R15
		};
	}

	virtual vector<uint32_t> GetAllRegisters() override
	{
		vector<uint32_t> result = {0};
		for(int i=1; i<(int)SUPERH_REGISTER_MAXIMUM; i++)
			result.push_back(i);
		return result;
	}

	virtual BNRegisterInfo GetRegisterInfo(uint32_t regId) override
	{
		//printf("%s()\n", __func__);
		return RegisterInfo(0,0,0);
	}

	virtual uint32_t GetStackPointerRegister() override
	{
		return R15;
	}

	virtual uint32_t GetLinkRegister() override
	{
		return PR;
	}

	/*************************************************************************/

	bool Assemble(const string& code, uint64_t addr, DataBuffer& result, string& errors) override
	{
		(void)code;
		(void)addr;
		(void)result;
		(void)errors;
		//printf("%s()\n", __func__);
		return true;
	}

	/*************************************************************************/

	virtual bool IsNeverBranchPatchAvailable(const uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		//printf("%s()\n", __func__);
		return false;
	}

	virtual bool IsAlwaysBranchPatchAvailable(const uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		//printf("%s()\n", __func__);
		return false;
	}

	virtual bool IsInvertBranchPatchAvailable(const uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		//printf("%s()\n", __func__);
		return false;
	}

	virtual bool IsSkipAndReturnZeroPatchAvailable(const uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		//printf("%s()\n", __func__);
		return false;
	}

	virtual bool IsSkipAndReturnValuePatchAvailable(const uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		//printf("%s()\n", __func__);
		return false;
	}

	/*************************************************************************/

	virtual bool ConvertToNop(uint8_t* data, uint64_t, size_t len) override
	{
		(void)data;
		(void)len;
		//printf("%s()\n", __func__);
		return false;
	}

	virtual bool AlwaysBranch(uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		//printf("%s()\n", __func__);
		return false;
	}

	virtual bool InvertBranch(uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		//printf("%s()\n", __func__);
		return false;
	}

	virtual bool SkipAndReturnValue(uint8_t* data, uint64_t addr, size_t len, uint64_t value) override
	{
		(void)data;
		(void)addr;
		(void)len;
		(void)value;
		//printf("%s()\n", __func__);
		return false;
	}

	/*************************************************************************/

};

class Sh4LinuxCallingConvention: public CallingConvention
{
public:
	Sh4LinuxCallingConvention(Architecture* arch): CallingConvention(arch, "sh4-standard")
	{
	}

	virtual vector<uint32_t> GetIntegerArgumentRegisters() override
	{
		return vector<uint32_t>{
			R4, R6, R6, R7
		};
	}

	virtual vector<uint32_t> GetCallerSavedRegisters() override
	{
		return vector<uint32_t>{
		};
	}

	virtual vector<uint32_t> GetCalleeSavedRegisters() override
	{
		return vector<uint32_t>{
		};
	}

	virtual uint32_t GetIntegerReturnValueRegister() override
	{
		return R0;
	}
};


// class LinuxSH4Platform: public Platform
// {
// public:
// 	LinuxSH4Platform(Architecture* arch): Platform(arch, "linux-sh4")
// 	{
// 		Ref<CallingConvention> cc;

// 		cc = arch->GetCallingConventionByName("sh4-standard");
// 		if(cc)
// 			RegisterDefaultCallingConvention(cc);

// 		cc = arch->GetCallingConventionByName("linux-syscall");
// 		if(cc)
// 			SetSystemCallConvention(cc);
// 	}
// };

extern "C"
{
	BN_DECLARE_CORE_ABI_VERSION

#ifndef DEMO_VERSION
	BINARYNINJAPLUGIN void CorePluginDependencies()
	{
		AddOptionalPluginDependency("view_pe");
	}
#endif

#ifdef DEMO_VERSION
	bool Sh4PluginInit()
#else
	BINARYNINJAPLUGIN bool CorePluginInit()
#endif
	{
		#define EM_SUPERH 0x2a

		//printf("ARCH SH4 compiled at %s %s\n", __DATE__, __TIME__);
		//LogInfo("ARCH SH4 compiled at %s %s\n", __DATE__, __TIME__);

		/* 1) architecture */
		Architecture* archSh4 = new SH4Architecture("sh4", LittleEndian);
		Architecture::Register(archSh4);

		/* 2) calling conventions */
		Ref<CallingConvention> conv;
		conv = new Sh4LinuxCallingConvention(archSh4);

		archSh4->RegisterCallingConvention(conv);
		archSh4->SetDefaultCallingConvention(conv);

		/* 3) platform */
		// Ref<Platform> platform;
		// platform = new LinuxSH4Platform(archSh4);
		// Platform::Register("linux", platform);
		// BinaryViewType::RegisterPlatform("ELF", 0, archSh4, platform);
		// BinaryViewType::RegisterPlatform("ELF", EM_SUPERH, archSh4, platform);

		/* 4) binary view */
		BinaryViewType::RegisterArchitecture("ELF", EM_SUPERH, LittleEndian, archSh4);

		return true;
	}
}

