#include "ScriptVarTypeMgr.h"
#include "EScriptVariableType.h"
#include "ScriptVarInfo.h"
#include "ScriptClassMgr.h"
namespace zlscript
{
	CScriptVarTypeMgr CScriptVarTypeMgr::s_Instance;

	CScriptVarTypeMgr::CScriptVarTypeMgr()
	{
	}

	CScriptVarTypeMgr::~CScriptVarTypeMgr()
	{
	}

	unsigned int CScriptVarTypeMgr::GetVarType(std::string name)
	{
		auto it = mapDicString2Type.find(name);
		if (it != mapDicString2Type.end())
		{
			return it->second;
		}
		return -1;
	}

	CBaseVar* CScriptVarTypeMgr::GetVar(unsigned int type)
	{
		if (type < m_vVarTypeMgr.size())
		{
			return m_vVarTypeMgr[type]->New(1);
		}
		return nullptr;
	}

	void CScriptVarTypeMgr::ReleaseVar(CBaseVar* pVar)
	{
		if (pVar == nullptr)
		{
			return;
		}
		if (pVar->GetType() >= m_vVarTypeMgr.size())
		{
			return;
		}
		m_vVarTypeMgr[pVar->GetType()]->Release(pVar);
	}

	CScriptPointInterface* CScriptVarTypeMgr::NewObject(unsigned int type)
	{
		if (type < m_vVarTypeMgr.size())
		{
			CBaseScriptClassMgr* pMsg = dynamic_cast<CBaseScriptClassMgr*>(m_vVarTypeMgr[type]);
			return pMsg->NewObject();
		}
		return nullptr;
	}

	void CScriptVarTypeMgr::ReleaseObject(CScriptPointInterface* pVar)
	{
		int type = pVar->getClassInfo()->nClassType;
		if (type < m_vVarTypeMgr.size())
		{
			CBaseScriptClassMgr* pMsg = dynamic_cast<CBaseScriptClassMgr*>(m_vVarTypeMgr[type]);
			return pMsg->Release(pVar);
		}
	}
	CBaseScriptClassInfo* CScriptVarTypeMgr::GetTypeInfo(unsigned int type)
	{
		if (type < m_vVarTypeInfo.size())
		{
			return m_vVarTypeInfo[type];
		}
		return nullptr;
	}

}