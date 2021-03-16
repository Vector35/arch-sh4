#include <stdio.h>
#include <string.h>

#include "binaryninjaapi.h"
using namespace BinaryNinja; // for ::LogDebug, etc.
using namespace std;

//#define printf(...) while(0);
#include "decode.h"
#include "format.h"

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
		char tmp[32];
		Instruction instr;
		memset(&instr, 0, sizeof(instr));
		uint16_t insword = (data[0]<<8) | data[1];
		if(sh4_decompose(insword, &instr, addr) != 0)
			return false;

		switch(instr.opcode) {
			case OPC_BSR:
			case OPC_BSRF:
			case OPC_JSR:
				if(instr.operands_n == 1)
					result.AddBranch(CallDestination, instr.operands[0].address);
				else
					result.AddBranch(UnresolvedBranch);
				break;

			case OPC_BRA:
			case OPC_JMP:
				if(instr.operands_n == 1)
					result.AddBranch(UnconditionalBranch, instr.operands[0].address);
				else
					result.AddBranch(UnresolvedBranch);
				break;

			case OPC_BT:
			case OPC_BF:
				//result.AddBranch(TrueBranch, instr.operands[0].address);
				//result.AddBranch(FalseBranch, addr+2);
				break;

			case OPC_RTS:
				result.AddBranch(FunctionReturn);
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
		uint16_t insword = (data[0]<<8) | data[1];
		if(sh4_decompose(insword, &instr, addr) != 0)
			return false;
		if(!(instr.opcode > OPC_NONE && instr.opcode < OPC_MAXIMUM))
			return false;

		/* opcode */
		strcpy(buf, sh4_opc_strs[instr.opcode]);
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
		if(instr.opcode == OPC_CMP || instr.opcode == OPC_FCMP) {
			if(instr.cond > CMP_COND_NONE && instr.cond < CMP_COND_MAXIMUM) {
				strcat(buf, "/");
				strcat(buf, sh4_cmp_cond_strs[instr.cond]);
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
					result.emplace_back(RegisterToken, sh4_reg_strs[instr.operands[i].regA]);
					break;

				case IMMEDIATE:
					result.emplace_back(TextToken, "#");
					sprintf(buf, "%d", instr.operands[i].immediate);
					result.emplace_back(IntegerToken, buf);
					break;

				case ADDRESS:
					sprintf(buf, "0x%016llx", instr.operands[i].address);
					result.emplace_back(PossibleAddressToken, buf);
					break;

				case DEREF_REG:
					result.emplace_back(BeginMemoryOperandToken, "@");

					if(instr.operands[i].flags & SH4_FLAG_PRE_INCREMENT)
						result.emplace_back(TextToken, "+");
					if(instr.operands[i].flags & SH4_FLAG_PRE_DECREMENT)
						result.emplace_back(TextToken, "-");

					result.emplace_back(RegisterToken, sh4_reg_strs[instr.operands[i].regA]);

					if(instr.operands[i].flags & SH4_FLAG_POST_INCREMENT)
						result.emplace_back(TextToken, "+");
					if(instr.operands[i].flags & SH4_FLAG_POST_DECREMENT)
						result.emplace_back(TextToken, "-");
					break;

				case DEREF_REG_REG:
					result.emplace_back(BeginMemoryOperandToken, "@(");
					result.emplace_back(RegisterToken, sh4_reg_strs[instr.operands[i].regA]);
					result.emplace_back(OperandSeparatorToken, ",");
					result.emplace_back(RegisterToken, sh4_reg_strs[instr.operands[i].regB]);
					result.emplace_back(EndMemoryOperandToken, ")");
					break;

				case DEREF_REG_IMM:
					result.emplace_back(BeginMemoryOperandToken, "@(");
					sprintf(buf, "%d", instr.operands[i].displacement);
					result.emplace_back(IntegerToken, buf);
					result.emplace_back(OperandSeparatorToken, ",");
					result.emplace_back(RegisterToken, sh4_reg_strs[instr.operands[i].regA]);
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
		uint16_t insword = (data[0]<<8) | data[1];
		if(sh4_decompose(insword, &instr, addr) != 0)
			return false;
		if(!(instr.opcode > OPC_NONE && instr.opcode < OPC_MAXIMUM))
			return false;

		switch(instr.opcode) {
			/* there are 3 "call subroutine" instrucitons in SH4 */
			/* (1/3) JumpSubRoutine <reg> */
			/* eg: 0B 41    jsr @r1 */
			case OPC_JSR:
				il.AddInstruction(il.Call(il.Register(4, instr.operands[0].regA)));
				break;

			/* (2/3) branch subroutine, PC = (PC+4) + 2*disp */
			case OPC_BSR:
				il.AddInstruction(il.Call(il.ConstPointer(4, instr.operands[0].address)));
				break;

			/* (3/3) branch subroutine far, PC = (PC+4) + Rn */
			case OPC_BSRF:
				il.AddInstruction(il.Call(
					il.Add(4,
						il.Add(4, il.Register(4, PC), il.Const(4, 4)),
						il.Register(4, instr.operands[0].regA)
					)
				));
				break;

			case OPC_JMP:
				il.AddInstruction(il.Jump(il.Register(4, instr.operands[0].regA)));
				break;

			case OPC_MOV:
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
					il.AddInstruction(il.Nop());

				break;

			/* return subroutine */
			case OPC_RTS:
				il.AddInstruction(il.Return(il.Register(4, PR)));
				break;

			default:
				il.AddInstruction(il.Nop());
		}

		len = 2;
		return true;
	}

	virtual size_t GetFlagWriteLowLevelIL(BNLowLevelILOperation op, size_t size, uint32_t flagWriteType,
		uint32_t flag, BNRegisterOrConstant* operands, size_t operandCount, LowLevelILFunction& il) override
	{
		printf("%s()\n", __func__);
		return il.Unimplemented();
	}

	virtual string GetRegisterName(uint32_t regId) override
	{
		//printf("%s()\n", __func__);
		if(regId > SH4_REGISTER_NONE && regId < SH4_REGISTER_MAXIMUM)
			return sh4_reg_strs[regId];

		return "unknown";
	}

	virtual vector<uint32_t> GetAllFlags() override
	{
		//printf("%s()\n", __func__);
		return vector<uint32_t>();
	}

	virtual string GetFlagName(uint32_t flag) override
	{
		printf("%s(%d)\n", __func__, flag);
		return "ERR_FLAG_NAME";
	}

	virtual vector<uint32_t> GetAllFlagWriteTypes() override
	{
		printf("%s()\n", __func__);
		return vector<uint32_t>();
	}

	virtual string GetFlagWriteTypeName(uint32_t writeType) override
	{
		printf("%s(%d)\n", __func__, writeType);
		return "none";
	}

	virtual vector<uint32_t> GetFlagsWrittenByFlagWriteType(uint32_t writeType) override
	{
		printf("%s(%d)\n", __func__, writeType);
		return vector<uint32_t>();
	}

	virtual BNFlagRole GetFlagRole(uint32_t flag, uint32_t semClass = 0) override
	{
		printf("%s(%d)\n", __func__, flag);
		return SpecialFlagRole;
	}

	virtual vector<uint32_t> GetFlagsRequiredForFlagCondition(BNLowLevelILFlagCondition cond, uint32_t semClass = 0) override
	{
		//printf("%s(%d)\n", __func__, cond);
		return vector<uint32_t>{};
	}

	virtual vector<uint32_t> GetFullWidthRegisters() override
	{
		printf("%s()\n", __func__);
		return vector<uint32_t> {
			R0, R1, R2, R3, R4, R5, R6, R7,
			R8, R9, R10, R11, R12, R13, R14, R15
		};
	}

	virtual vector<uint32_t> GetAllRegisters() override
	{
		vector<uint32_t> result = {0};
		for(int i=1; i<(int)SH4_REGISTER_MAXIMUM; i++)
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
		printf("%s()\n", __func__);
		return true;
	}

	/*************************************************************************/

	virtual bool IsNeverBranchPatchAvailable(const uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		printf("%s()\n", __func__);
		return false;
	}

	virtual bool IsAlwaysBranchPatchAvailable(const uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		printf("%s()\n", __func__);
		return false;
	}

	virtual bool IsInvertBranchPatchAvailable(const uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		printf("%s()\n", __func__);
		return false;
	}

	virtual bool IsSkipAndReturnZeroPatchAvailable(const uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		printf("%s()\n", __func__);
		return false;
	}

	virtual bool IsSkipAndReturnValuePatchAvailable(const uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		printf("%s()\n", __func__);
		return false;
	}

	/*************************************************************************/

	virtual bool ConvertToNop(uint8_t* data, uint64_t, size_t len) override
	{
		(void)data;
		(void)len;
		printf("%s()\n", __func__);
		return false;
	}

	virtual bool AlwaysBranch(uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		printf("%s()\n", __func__);
		return false;
	}

	virtual bool InvertBranch(uint8_t* data, uint64_t addr, size_t len) override
	{
		(void)data;
		(void)addr;
		(void)len;
		printf("%s()\n", __func__);
		return false;
	}

	virtual bool SkipAndReturnValue(uint8_t* data, uint64_t addr, size_t len, uint64_t value) override
	{
		(void)data;
		(void)addr;
		(void)len;
		(void)value;
		printf("%s()\n", __func__);
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


class LinuxSH4Platform: public Platform
{
public:
	LinuxSH4Platform(Architecture* arch): Platform(arch, "linux-sh4")
	{
		Ref<CallingConvention> cc;

		cc = arch->GetCallingConventionByName("sh4-standard");
		if(cc)
			RegisterDefaultCallingConvention(cc);

		cc = arch->GetCallingConventionByName("linux-syscall");
		if(cc)
			SetSystemCallConvention(cc);
	}
};

extern "C"
{
	BN_DECLARE_CORE_ABI_VERSION

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
		Ref<Platform> platform;
		platform = new LinuxSH4Platform(archSh4);
		Platform::Register("linux", platform);
		BinaryViewType::RegisterPlatform("ELF", 0, archSh4, platform);
		BinaryViewType::RegisterPlatform("ELF", EM_SUPERH, archSh4, platform);

		/* 4) binary view */
		BinaryViewType::RegisterArchitecture("ELF", EM_SUPERH, LittleEndian, archSh4);

		return true;
	}
}

