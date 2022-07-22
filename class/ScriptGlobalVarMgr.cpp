#include "ScriptGlobalVarMgr.h"
#include "EScriptVariableType.h"
#include "ScriptVarInfo.h"
#include "ScriptVarTypeMgr.h"
#include "ScriptVarAssignmentMgr.h"
namespace zlscript
{
	CScriptGlobalVarMgr CScriptGlobalVarMgr::s_Instance;

	CScriptGlobalVarMgr::CScriptGlobalVarMgr()
	{
	}

	CScriptGlobalVarMgr::~CScriptGlobalVarMgr()
	{
	}

	bool CScriptGlobalVarMgr::New(std::string name, int type)
	{
		std::lock_guard<std::mutex> Lock(m_lock);

		stGlobalVar var;
		var.index = m_vGlobalVar.size();
		var.pVar = CScriptVarTypeMgr::GetInstance()->GetVar(type);

		m_mapDicName2Index[name] = var.index;
		m_vGlobalVar.push_back(var);
		return true;
	}

	int CScriptGlobalVarMgr::GetIndex(std::string name)
	{
		auto it = m_mapDicName2Index.find(name);
		if (it != m_mapDicName2Index.end())
		{
			return it->second;
		}
		return -1;
	}

	void CScriptGlobalVarMgr::Revert(stGlobalVar* pGlobalVar)
	{
		if (pGlobalVar)
		{
			pGlobalVar->lock.unlock();
		}
	}

	void CScriptGlobalVarMgr::Revert(unsigned int index)
	{
		if (index >= m_vGlobalVar.size())
		{
			return;
		}
		stGlobalVar* pVar = &m_vGlobalVar[index];
		Revert(pVar);
	}

	const CBaseVar* CScriptGlobalVarMgr::Get(unsigned int index)
	{
		if (index >= m_vGlobalVar.size())
		{
			return nullptr;
		}
		stGlobalVar* pVar = &m_vGlobalVar[index];
		pVar->lock.lock();
		return pVar->pVar;
	}
	bool CScriptGlobalVarMgr::Set(unsigned int index,const CBaseVar* pVar)
	{
		if (index >= m_vGlobalVar.size())
		{
			return false;
		}
		stGlobalVar* pGVar = &m_vGlobalVar[index];
		if (pGVar)
		{
			pGVar->lock.lock();
			AssignVar(pGVar->pVar,pVar);
			pGVar->lock.unlock();
			return true;
		}
		return false;
	}
}