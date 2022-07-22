#pragma once
#include "EMicroCodeType.h"
#include "ScriptCompileInfo.h"
#include "ScriptCodeStyle.h"

#include "ScriptExeCodeData.h"

namespace zlscript
{
	CExeCodeData::CExeCodeData()
	{
		nType = EICODE_FUN_DEFAULT;
		nFunParamNums = 0;
		nDefaultReturnType = EScriptVal_None;
		pBeginCode = nullptr;
		pEndCode = nullptr;
	}
	CExeCodeData::~CExeCodeData()
	{
		while (pBeginCode)
		{
			auto pNext = pBeginCode->m_pNext;
			delete pBeginCode;
			pBeginCode = pNext;
		}
	}
	void CExeCodeData::AddCode(CBaseExeCode* pCode)
	{
		if (pCode == nullptr)
		{
			return;
		}
		if (pBeginCode == nullptr)
		{
			pBeginCode = pCode;
			pEndCode = pCode;
		}
		else
		{
			pCode->nCodeIndex = pEndCode->nCodeIndex + 1;
			pEndCode->m_pNext = pCode;
			pEndCode = pCode;
		}
	}
	int CExeCodeData::GetConstVarIndex(CBaseVar* pVar)
	{
		if (pVar == nullptr)
		{
			_ASSERT(0);
		}
		for (unsigned int i = 0; i < vConstVar.size(); i++)
		{
			if (vConstVar[i] == pVar)
			{
				return i;
			}
		}
		int result = vConstVar.size();
		vConstVar.push_back(pVar);
		return result;
	}
}