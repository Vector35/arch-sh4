#include <stdio.h>
#include <string.h>

#include "binaryninjaapi.h"
using namespace BinaryNinja; // for ::LogDebug, etc.
using namespace std;

//#define printf(...) while(0);
#include "disasm.h"

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

	virtual bool GetInstructionInfo(const uint8_t* data, uint64_t addr,
		size_t maxLen, InstructionInfo& result) override
	{
		char tmp[32];

		//printf("%s(data, addr=%llX, maxLen=%zu, result) parses ", __func__, addr, maxLen);
		result.length = 1;
		return true;
	}

	virtual bool GetInstructionText(const uint8_t* data, uint64_t addr,
	  size_t& len, vector<InstructionTextToken>& result) override
	{
		//printf("%s(%02X %02X, 0x%llX, 0x%zX, il)\n", __func__, data[0], data[1], addr, len);
		char buf[32];

		/* decompose instruction */
		struct decomp_result dr;
		memset(&dr, 0, sizeof(dr));
		if(decompose(addr, *(uint16_t *)data, &dr) != 0)
			return false;
		if(!(dr.opcode > OPC_NONE && dr.opcode < OPC_MAXIMUM))
			return false;

		/* opcode */
		strcpy(buf, sh4_opc_strs[dr.opcode]);
		/* size modifier suffixes {.b, .w, .l} */
		if(dr.length_suffix == LEN_SUFFIX_B)
			strcat(buf, ".b");
		if(dr.length_suffix == LEN_SUFFIX_W)
			strcat(buf, ".w");
		if(dr.length_suffix == LEN_SUFFIX_L)
			strcat(buf, ".l");
		/* delay slot suffix .s or /s from doc */
		if(dr.delay_slot)
			strcat(buf, ".s");
		/* conditional "/xxx" if this is a cmp */
		if(dr.opcode == OPC_CMP || dr.opcode == OPC_FCMP) {
			if(dr.cond > CMP_COND_NONE && dr.cond < CMP_COND_MAXIMUM) {
				strcat(buf, "/");
				strcat(buf, sh4_cmp_cond_strs[dr.cond]);
			}
		}
		/* done */
		result.emplace_back(InstructionToken, buf);

		/* operands */
		if(dr.operands_n)
			result.emplace_back(TextToken, " ");

		for(int i=0; i < (dr.operands_n); ++i) {
			char buf[32];

			switch(dr.operands[i].type) {
				case GPREG:
				case BANKREG:
				case CTRLREG:
				case SYSREG:
				case FPUREG:
					result.emplace_back(RegisterToken, sh4_reg_strs[dr.operands[i].regA]);
					break;

				case IMMEDIATE:
					result.emplace_back(TextToken, "#");
					sprintf(buf, "%d", dr.operands[i].immediate);
					result.emplace_back(IntegerToken, buf);
					break;

				case ADDRESS:
					sprintf(buf, "0x%016llx", dr.operands[i].address);
					result.emplace_back(IntegerToken, buf);
					break;

				case DEREF_REG:
					result.emplace_back(BeginMemoryOperandToken, "@");

					if(dr.operands[i].flags & SH4_FLAG_PRE_INCREMENT)
						result.emplace_back(TextToken, "+");
					if(dr.operands[i].flags & SH4_FLAG_PRE_DECREMENT)
						result.emplace_back(TextToken, "-");

					result.emplace_back(RegisterToken, sh4_reg_strs[dr.operands[i].regA]);

					if(dr.operands[i].flags & SH4_FLAG_POST_INCREMENT)
						result.emplace_back(TextToken, "+");
					if(dr.operands[i].flags & SH4_FLAG_POST_DECREMENT)
						result.emplace_back(TextToken, "-");
					break;

				case DEREF_REG_REG:
					result.emplace_back(BeginMemoryOperandToken, "@(");
					result.emplace_back(RegisterToken, sh4_reg_strs[dr.operands[i].regA]);
					result.emplace_back(OperandSeparatorToken, ",");
					result.emplace_back(RegisterToken, sh4_reg_strs[dr.operands[i].regB]);
					result.emplace_back(EndMemoryOperandToken, ")");
					break;

				case DEREF_REG_IMM:
					result.emplace_back(BeginMemoryOperandToken, "@(");
					sprintf(buf, "%d", dr.operands[i].displacement);
					result.emplace_back(IntegerToken, buf);
					result.emplace_back(OperandSeparatorToken, ",");
					result.emplace_back(RegisterToken, sh4_reg_strs[dr.operands[i].regA]);
					result.emplace_back(EndMemoryOperandToken, ")");
					break;

				default:
					LogError("unknown operand type\n");
					break;
			}

			if(i != dr.operands_n-1)
				result.emplace_back(OperandSeparatorToken, ",");
		}

		return true;
	}

	virtual bool GetInstructionLowLevelIL(const uint8_t* data, uint64_t addr, size_t& len, LowLevelILFunction& il) override
	{
		printf("%s()\n", __func__);
		return false;
	}

	virtual size_t GetFlagWriteLowLevelIL(BNLowLevelILOperation op, size_t size, uint32_t flagWriteType,
		uint32_t flag, BNRegisterOrConstant* operands, size_t operandCount, LowLevelILFunction& il) override
	{
		printf("%s()\n", __func__);
		return il.Unimplemented();
	}

	virtual string GetRegisterName(uint32_t regId) override
	{
		printf("%s()\n", __func__);
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
		return vector<uint32_t>();
	}

	virtual vector<uint32_t> GetFullWidthRegisters() override
	{
		printf("%s()\n", __func__);
		return vector<uint32_t>{0};
	}

	virtual vector<uint32_t> GetAllRegisters() override
	{
		vector<uint32_t> result = {0};
		return result;
	}

	virtual BNRegisterInfo GetRegisterInfo(uint32_t regId) override
	{
		//printf("%s()\n", __func__);
		return RegisterInfo(0,0,0);
	}

	virtual uint32_t GetStackPointerRegister() override
	{
		printf("%s()\n", __func__);
		return 0;
	}

	virtual uint32_t GetLinkRegister() override
	{
		printf("%s()\n", __func__);
		return 0;
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

extern "C"
{
	BINARYNINJAPLUGIN bool CorePluginInit()
	{
		printf("ARCH SH4 compiled at %s %s\n", __DATE__, __TIME__);
		LogInfo("ARCH SH4 compiled at %s %s\n", __DATE__, __TIME__);

		/* create, register arch in global list of available architectures */
		Architecture* archSh4 = new SH4Architecture("sh4", LittleEndian);
		Architecture::Register(archSh4);

		#define EM_SUPERH 0x2a
		BinaryViewType::RegisterArchitecture("ELF", EM_SUPERH, LittleEndian, archSh4);

		return true;
	}
}

