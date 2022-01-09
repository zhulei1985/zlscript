#include "ScriptBaseICode.h"
#include "ScriptCompiler.h"
namespace zlscript
{
	CBaseICode* CBaseICode::GetFather()
	{
		return m_pFather;
	}
	void CBaseICode::SetFather(CBaseICode* pCode)
	{
		m_pFather = pCode;
	}
	CScriptCompiler* CBaseICode::GetCompiler()
	{
		return m_pCompiler;
	}
	void CBaseICode::SetCompiler(CScriptCompiler* pCompiler)
	{
		m_pCompiler = pCompiler;
	}
	bool CBaseICode::DefineTempVar(std::string VarType, std::string VarName)
	{
		if (m_pFather)
		{
			return m_pFather->DefineTempVar(VarType, VarName);
		}
		return false;
	}
	bool CBaseICode::CheckTempVar(const char* pVarName)
	{
		if (GetTempVarIndex(pVarName) != g_nTempVarIndexError)
		{
			return true;
		}

		return false;
	}
	unsigned int CBaseICode::GetTempVarIndex(const char* pVarName)
	{
		if (GetFather())
		{
			return GetFather()->GetTempVarIndex(pVarName);
		}
		return g_nTempVarIndexError;
	}

	VarInfo* CBaseICode::GetTempVarInfo(const char* pVarName)
	{
		if (GetFather())
		{
			return GetFather()->GetTempVarInfo(pVarName);
		}
		return nullptr;
	}

	void CBaseICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (pCode)
		{
			pCode->SetFather(this);
		}
	}

	CBaseICode* CBaseICode::GetICode(int nType, int index)
	{
		return nullptr;
	}

	void CBaseICode::AddErrorInfo(unsigned int pos, std::string error)
	{
		if (m_pCompiler)
			m_pCompiler->AddErrorInfo(pos, error);
	}
}