#pragma once
#include "EMicroCodeType.h"
#include "ScriptCompileInfo.h"
namespace zlscript
{
	class CExcCodeData
	{
	public:
		CExcCodeData()
		{
			nType = EICODE_FUN_DEFAULT;
			nDefaultReturnType = EScriptVal_None;
			pBeginCode = nullptr;
			pEndCode = nullptr;
		}
		~CExcCodeData()
		{
			while (pBeginCode)
			{
				auto pNext = pBeginCode->m_pNext;
				delete pBeginCode;
				pBeginCode = pNext;
			}
		}
		void AddCode(CBaseExeCode* pCode)
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
		CBaseExeCode* pBeginCode;
		CBaseExeCode* pEndCode;
		int nType;


		int nDefaultReturnType;//默认返回值类型
	};
}