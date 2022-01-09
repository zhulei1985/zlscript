#pragma once
#include "EMicroCodeType.h"
#include "ScriptCompileInfo.h"
namespace zlscript
{
	struct VarInfo
	{
		VarInfo()
		{
			cType = 0;
			cGlobal = 0;
			wExtend = 0;
			dwPos = 0;
		}
		//__int64 nVarInfo;

		unsigned char cType; // 1,整数,2 浮点 3,字符 4 类指针
		unsigned char cGlobal;// 1 表示全局变量
		unsigned short wExtend; // 大于1表示是数组下标,不再使用
		unsigned int dwPos;//位置ID

	};

	const unsigned int g_nTempVarIndexError = -1;
	class CScriptCompiler;

	//编译的中间状态
	class CBaseICode
	{
	public:
		CBaseICode() {
			m_pFather = nullptr;
			m_pCompiler = nullptr;
			cRegisterIndex = R_A;
		}
		virtual int GetType()
		{
			return 0;
		}
	public:
		virtual CBaseICode* GetFather();
		virtual void SetFather(CBaseICode* pCode);

		CScriptCompiler* GetCompiler();
		void SetCompiler(CScriptCompiler* pCompiler);

		virtual bool DefineTempVar(std::string VarType, std::string VarName);
		virtual bool CheckTempVar(const char* pVarName);
		//virtual void SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex) {
		//	return;
		//}
		virtual unsigned int GetTempVarIndex(const char* pVarName);
		virtual VarInfo* GetTempVarInfo(const char* pVarName);

		virtual void SetRegisterIndex(char val) { cRegisterIndex = val; }
		virtual bool MakeExeCode(tagCodeData& vOut) = 0;
		virtual bool Compile(SentenceSourceCode& vIn) = 0;

		virtual void AddICode(int nType, CBaseICode* pCode);
		virtual CBaseICode* GetICode(int nType, int index);
	private:
		CBaseICode* m_pFather;
	protected:
		CScriptCompiler* m_pCompiler;
		void AddErrorInfo(unsigned int pos, std::string error);
	public:
		unsigned int m_unBeginSoureIndex;

		unsigned char cRegisterIndex;
	};
}