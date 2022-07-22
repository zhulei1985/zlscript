#pragma once
#include <string>
namespace zlscript
{
	class CBaseVar;
	class CBaseExeCode;
	class CExeCodeData
	{
	public:
		CExeCodeData();
		~CExeCodeData();
		void AddCode(CBaseExeCode* pCode);
		CBaseExeCode* pBeginCode;
		CBaseExeCode* pEndCode;
		int nType;

		//临时变量信息
		std::vector<int> vLocalVarType;
		//常量表
		std::vector<CBaseVar*> vConstVar;
		int nFunParamNums;
		int nDefaultReturnType;//默认返回值类型

		std::string funname;
	public:
		int GetConstVarIndex(CBaseVar* pVar);
	};
}