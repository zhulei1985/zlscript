/****************************************************************************
	Copyright (c) 2019 ZhuLei
	Email:zhulei1985@foxmail.com

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
 ****************************************************************************/

#include "ScriptEventMgr.h"
#include "ScriptVirtualMachine.h"
namespace zlscript
{
	CScriptEventMgr CScriptEventMgr::s_Instance;
	CScriptEventMgr::CScriptEventMgr()
	{
		m_nEventListCount = E_SCRIPT_EVENT_CHANNEL_ASSIGN;
	}

	CScriptEventMgr::~CScriptEventMgr()
	{
		Lock();
		std::map<int, listEvent>::iterator itList = m_mapEvent.begin();
		for (; itList != m_mapEvent.end(); itList++)
		{
			listEvent& le = itList->second;
			while (le.size() > 0)
			{
				tagScriptEvent* pEvent = le.front();
				le.pop_front();
				if (pEvent)
				{
					delete pEvent;
				}
			}
		}

		Unlock();
	}

	int CScriptEventMgr::AssignID()
	{
		int nResult = 0;
		Lock();
		m_nEventListCount++;
		nResult = m_nEventListCount;
		Unlock();
		return nResult;
	}

	bool CScriptEventMgr::SendEvent(int nSendID, int nRecvID, CScriptStack& vIn)
	{
		Lock();
		listEvent& le = m_mapEvent[nRecvID];
		tagScriptEvent* pEvent = new tagScriptEvent;
		pEvent->nRecvID = nRecvID;
		pEvent->nSendID = nSendID;

		while (vIn.size() > 0)
		{
			pEvent->m_Parm.push(vIn.top());
			vIn.pop();
		}
		le.push_back(pEvent);
		Unlock();
		return true;
	}

	tagScriptEvent* CScriptEventMgr::GetEvent(int nID)
	{
		listEvent& le = m_mapEvent[nID];
		if (le.size() > 0)
		{
			return le.front();
		}
		return nullptr;
	}

	void CScriptEventMgr::PopEvent(int nID)
	{
		listEvent& le = m_mapEvent[nID];
		if (le.size() > 0)
		{
			tagScriptEvent* pEvent = le.front();
			le.pop_front();
			if (pEvent)
			{
				delete pEvent;
			}
		}
	}

	int CScriptEventMgr::GetEventSize(int nID)
	{
		Lock();
		int nResult = 0;
		listEvent& le = m_mapEvent[nID];
		nResult = le.size();
		Unlock();
		return nResult;
	}

	void CScriptEventMgr::ProcessEvent(int nID, std::function<void(int , CScriptStack&, CScriptStack&)> const& fun)
	{
		int nSize = GetEventSize(nID);

		for (int i = 0; i < nSize; i++)
		{
			int nSendID = 0;
			CScriptStack ParmInfo;
			Lock();
			auto pEvent = GetEvent(nID);
			if (pEvent)
			{
				nSendID = pEvent->nSendID;
				ParmInfo = pEvent->m_Parm;
				PopEvent(nID);
			}
			else
			{
				Unlock();
				break;
			}
			Unlock();

			CScriptStack vRetrunVars;

			fun(nSendID, ParmInfo, vRetrunVars);
		}
	}

	void CScriptEventMgr::Lock()
	{
		m_Lock.lock();
	}

	void CScriptEventMgr::Unlock()
	{
		m_Lock.unlock();
	}
	void CScriptEventMgr::SetEventTrigger(std::string strEvent, __int64 nClassPoint, std::string flag, int nChannel, std::string strScriptName, CScriptStack& parm)
	{
		m_TriggerLock.lock();
		auto & classTrigger = m_mapTriggers[nClassPoint];
		auto& eventTrigger = classTrigger.vTriggers[strEvent];
		tagTrigger trigger;
		trigger.nEventnChannel = nChannel;
		trigger.strScriptName = strScriptName;
		trigger.parm = parm;
		eventTrigger.vTriggers[flag] = trigger;
		m_TriggerLock.unlock();
	}
	void CScriptEventMgr::TriggerEvent(std::string strEvent, __int64 nClassPoint)
	{
		m_TriggerLock.lock();
		auto& classTrigger = m_mapTriggers[nClassPoint];
		auto& eventTrigger = classTrigger.vTriggers[strEvent];
		for (auto it = eventTrigger.vTriggers.begin(); it != eventTrigger.vTriggers.end(); it++)
		{
			CScriptStack vRetrunVars;

			ScriptVector_PushVar(vRetrunVars, (__int64)E_SCRIPT_EVENT_RUNSCRIPT);
			ScriptVector_PushVar(vRetrunVars, (__int64)0);
			ScriptVector_PushVar(vRetrunVars, it->second.strScriptName.c_str());
			for (int i = 0; i < it->second.parm.size(); i++)
			{
				ScriptVector_PushVar(vRetrunVars, it->second.parm.GetVal(i));
			}

			CScriptEventMgr::GetInstance()->SendEvent(E_SCRIPT_EVENT_CHANNEL_SCRIPT_DRAWING, it->second.nEventnChannel, vRetrunVars);
		}
		m_TriggerLock.unlock();
	}
	void CScriptEventMgr::RemoveTrigger(std::string strEvent, __int64 nClassPoint, std::string flag)
	{
		m_TriggerLock.lock();
		auto& classTrigger = m_mapTriggers[nClassPoint];
		if (strEvent == "")
		{
			classTrigger.vTriggers.clear();
		}
		else
		{
			auto& eventTrigger = classTrigger.vTriggers[strEvent];
			if (flag == "")
			{
				eventTrigger.vTriggers.clear();
			}
			else
			{
				eventTrigger.vTriggers.erase(flag);
			}
		}
		m_TriggerLock.unlock();
	}
}