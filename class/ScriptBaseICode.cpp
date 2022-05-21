#include "ScriptBaseICode.h"
#include "ScriptCompiler.h"
#include "ScriptExecBlock.h"
namespace zlscript
{
	CBaseICode* CBaseICode::GetFather()
	{
		return m_pFather;
	}
	void CBaseICode::SetFather(CBaseICode* pCode)
	{
		m_pFather = pCode;
		if (m_pFather)
			m_nRunStateIndex = m_pFather->m_nRunStateIndex + 1;
		else
			m_nRunStateIndex = 0;
		SetMaxRunState(m_nRunStateIndex);
	}
	//CScriptCompiler* CBaseICode::GetCompiler()
	//{
	//	return m_pCompiler;
	//}
	//void CBaseICode::SetCompiler(CScriptCompiler* pCompiler)
	//{
	//	m_pCompiler = pCompiler;
	//}
	bool CBaseICode::DefineTempVar(int type, std::string VarName, CScriptCompiler* pCompiler)
	{
		if (m_pFather)
		{
			return m_pFather->DefineTempVar(type, VarName, pCompiler);
		}
		return false;
	}
	bool CBaseICode::CheckTempVar(const char* pVarName)
	{
		if (GetTempVarInfo(pVarName) != nullptr)
		{
			return true;
		}

		return false;
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
		//if (m_pCompiler)
		//	m_pCompiler->AddErrorInfo(pos, error);
	}
	int CBaseICode::Run(CScriptExecBlock* pBlock)
	{
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	void CBaseICode::SetMaxRunState(int val)
	{
		if (m_pFather)
		{
			m_pFather->SetMaxRunState(val);
		}
	}
}