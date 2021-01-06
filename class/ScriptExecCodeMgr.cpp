#include "ScriptExecCodeMgr.h"
#include "ScriptEventMgr.h"
#include "ScriptVirtualMachine.h"
#include "ScriptStack.h"
namespace zlscript
{
	CScriptExecCodeMgr CScriptExecCodeMgr::s_Instance;
	CScriptExecCodeMgr::CScriptExecCodeMgr()
	{
	}
	CScriptExecCodeMgr::~CScriptExecCodeMgr()
	{
	}
	bool CScriptExecCodeMgr::RemoteFunctionCall(std::string name, CScriptRunState* pState, int nParmNum)
	{
		auto it = m_mapRemoteCallInfo.find(name);
		if (it == m_mapRemoteCallInfo.end())
		{
			return false;
		}
		if (it->second.vScriptEventIndexs.empty())
		{
			return false;
		}
		if (it->second.unIndex >= it->second.vScriptEventIndexs.size())
		{
			it->second.unIndex = 0;
		}

		__int64 nEventIndex = it->second.vScriptEventIndexs[it->second.unIndex];
		CScriptStack scriptParm;
	
		//ScriptVector_PushVar(scriptParm, name.c_str());
		for (int i = 0; i < nParmNum; i++)
		{
			auto var = pState->PopVarFormStack();
			ScriptVector_PushVar(scriptParm, &var);
		}
		if (pState->m_pMachine)
		{
			pState->m_pMachine->RunTo(name, scriptParm, pState->GetId(), nEventIndex);
		}
		//CScriptEventMgr::GetInstance()->SendEvent(E_SCRIPT_EVENT_NETWORK_RUNSCRIPT, 0, m_scriptParm, nEventIndex);

		it->second.unIndex++;
		return true;
	}
	bool CScriptExecCodeMgr::SetRemoteFunction(std::string name, __int64 nEventIndex)
	{
		auto &info = m_mapRemoteCallInfo[name];
		for (unsigned int i = 0; i < info.vScriptEventIndexs.size(); i++)
		{
			if (info.vScriptEventIndexs[i] == nEventIndex)
			{
				return false;
			}
		}
		info.vScriptEventIndexs.push_back(nEventIndex);
		return true;
	}
	void CScriptExecCodeMgr::RemoveRemoteFunction(std::string name, __int64 nEventIndex)
	{
		auto it = m_mapRemoteCallInfo.find(name);
		if (it != m_mapRemoteCallInfo.end())
		{
			auto itIndex = it->second.vScriptEventIndexs.begin();
			for (; itIndex != it->second.vScriptEventIndexs.end(); itIndex++)
			{
				if (*itIndex == nEventIndex)
				{
					it->second.vScriptEventIndexs.erase(itIndex);
					break;
				}
			}
		}
	}
}