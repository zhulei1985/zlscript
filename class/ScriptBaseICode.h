#pragma once
#include "EMicroCodeType.h"
#include "ScriptCompileInfo.h"
namespace zlscript
{
	struct VarInfo
	{
		VarInfo()
		{
			nType = 0;
			index = 0;
		}

		unsigned int nType; //类型
		unsigned int index;//位置ID

	};

	const unsigned int g_nTempVarIndexError = -1;
	class CScriptCompiler;
	class CScriptExecBlock;
	//编译的中间状态
	class CBaseICode
	{
	public:
		CBaseICode() {
			m_pFather = nullptr;
		}
		virtual int GetType()
		{
			return 0;
		}
	public:
		virtual CBaseICode* GetFather();
		virtual void SetFather(CBaseICode* pCode);

		//CScriptCompiler* GetCompiler();
		//void SetCompiler(CScriptCompiler* pCompiler);

		virtual bool DefineTempVar(int type, std::string VarName);
		virtual bool CheckTempVar(const char* pVarName);

		virtual VarInfo* GetTempVarInfo(const char* pVarName);
		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler) = 0;

		virtual void AddICode(int nType, CBaseICode* pCode);
		virtual CBaseICode* GetICode(int nType, int index);
	private:
		CBaseICode* m_pFather;
	protected:
		//CScriptCompiler* m_pCompiler;
		void AddErrorInfo(unsigned int pos, std::string error);
	public:
		unsigned int m_unBeginSoureIndex;

	public:
		//直接基于中间代码来执行
		virtual int Run(CScriptExecBlock* pBlock) { return 0; };

	protected:
		int m_nRunStateIndex;//运行状态索引
	};
}