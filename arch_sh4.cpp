#include <stdio.h>
#include <string.h>

#include <vector>
using namespace std;

#include <binaryninjaapi.h>
using namespace BinaryNinja; // for ::LogDebug, etc.

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
		printf("%s() returns 2\n", __func__);
		return 4; // 4 bytes, 32-bits
	}

	virtual size_t GetInstructionAlignment() const override
	{
		printf("%s() returns 1\n", __func__);
		return 2;
	}

	/* the maximum length of an instruction
		(how much for binja to buffer before calling GetInstructionInfo()) */
	virtual size_t GetMaxInstructionLength() const override
	{
		printf("%s() returns 4\n", __func__);
		return 2;
	}

	virtual bool GetInstructionInfo(const uint8_t* data, uint64_t addr,
		size_t maxLen, InstructionInfo& result) override
	{
		char tmp[32];

		printf("%s(data, addr=%llX, maxLen=%zu, result) parses ", __func__, addr, maxLen);
		result.length = 1;
		return true;
	}

	virtual bool GetInstructionText(const uint8_t* data, uint64_t addr, 
	  size_t& len, vector<InstructionTextToken>& result) override
	{
		//printf("%s(data, 0x%llX, 0x%zX, il)\n", __func__, addr, len);
		char output[64];
		strcpy(output, "HELLO");
		result.emplace_back(TextToken, output);
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
		printf("%s()\n", __func__);
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
		printf("%s(%d)\n", __func__, cond);
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
		printf("%s()\n", __func__);
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
