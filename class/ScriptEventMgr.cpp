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
namespace zlscript
{
	CScriptEventMgr CScriptEventMgr::s_Instance;
	CScriptEventMgr::CScriptEventMgr()
	{
		m_nEventListCount = E_SCRIPT_EVENT_INDEX_ASSIGN;
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

	void CScriptEventMgr::Lock()
	{
		m_Lock.lock();
	}

	void CScriptEventMgr::Unlock()
	{
		m_Lock.unlock();
	}
}