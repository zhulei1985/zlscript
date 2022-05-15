#include "ScriptGlobalVarMgr.h"
#include "EScriptVariableType.h"
#include "ScriptVarInfo.h"
#include "ScriptVarTypeMgr.h"
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

	stGlobalVar* CScriptGlobalVarMgr::Get(std::string name)
	{
		auto it = m_mapDicName2Index.find(name);
		if (it != m_mapDicName2Index.end())
		{
			return Get(it->second);
		}
		return nullptr;
	}

	stGlobalVar* CScriptGlobalVarMgr::Get(unsigned int index)
	{
		if (index >= m_vGlobalVar.size())
		{
			return nullptr;
		}
		stGlobalVar* pVar = &m_vGlobalVar[index];
		pVar->lock.lock();
		return pVar;
	}

	void CScriptGlobalVarMgr::Revert(stGlobalVar* pGlobalVar)
	{
		if (pGlobalVar)
		{
			pGlobalVar->lock.unlock();
		}
	}

}