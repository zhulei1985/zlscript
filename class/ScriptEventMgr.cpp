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
	void tagEventChannel::clear()
	{
		{
			auto it = listEvent.begin();
			for (; it != listEvent.end(); it++)
			{
				tagScriptEvent* pEvent = *it;
				if (pEvent)
				{
					delete pEvent;
				}
			}
			listEvent.clear();
		}

		isBlocking = false;
	}


	CScriptEventMgr CScriptEventMgr::s_Instance;
	CScriptEventMgr::CScriptEventMgr()
	{
		m_nEventListCount = E_SCRIPT_EVENT_CHANNEL_ASSIGN;
	}

	CScriptEventMgr::~CScriptEventMgr()
	{
		Lock();
		auto itList = m_mapEvent.begin();
		for (; itList != m_mapEvent.end(); itList++)
		{
			auto& ch = itList->second;
			ch.m_mapEventChannel.clear();
		}
		m_mapEvent.clear();
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

	void CScriptEventMgr::RegisterEvent(int nEventType, int nChannelID)
	{
		Lock();
		auto& MapChannel = m_mapEvent[nEventType];
		auto& channel = MapChannel.m_mapEventChannel[nChannelID];
		channel.nChannelID = nChannelID;
		Unlock();
	}

	void CScriptEventMgr::RemoveEvent(int nEventType, int nChannelID)
	{
		auto& MapChannel = m_mapEvent[nEventType];

		auto it = MapChannel.m_mapEventChannel.find(nChannelID);
		if (it != MapChannel.m_mapEventChannel.end())
		{
			MapChannel.m_mapEventChannel.erase(it);
		}
	}

	void CScriptEventMgr::SetEventBlock(int nEventType, int nID, bool IsBlock)
	{
		auto& MapChannel = m_mapEvent[nEventType];

		auto it = MapChannel.m_mapEventChannel.find(nID);
		if (it != MapChannel.m_mapEventChannel.end())
		{
			it->second.isBlocking = IsBlock;
		}
	}


	bool CScriptEventMgr::SendEvent(int nEventType, int nSendID, CScriptStack& vIn, int nRecvID)
	{
		Lock();
		auto& mapChannel = m_mapEvent[nEventType];
		tagEventChannel* pChannel = nullptr;
		//查找一个合适的频道
		if (nRecvID == 0)
		{
			unsigned int nAmend = 0xffffffff;
			for (auto it = mapChannel.m_mapEventChannel.begin(); it != mapChannel.m_mapEventChannel.end(); it++)
			{
				auto pCur = &it->second;
				if (!pCur->isBlocking && pCur->listEvent.size() < nAmend)
				{
					nAmend = pCur->listEvent.size();
					pChannel = pCur;
				}
				if (pChannel == nullptr)
				{
					pChannel = pCur;
				}
			}
		}
		else
		{
			auto it = mapChannel.m_mapEventChannel.find(nRecvID);
			if (it != mapChannel.m_mapEventChannel.end())
			{
				pChannel = &it->second;
			}
		}

		if (pChannel)
		{
			tagScriptEvent* pEvent = new tagScriptEvent;
			pEvent->nSendID = nSendID;

			while (vIn.size() > 0)
			{
				pEvent->m_Parm.push(vIn.top());
				vIn.pop();
			}
			pChannel->listEvent.push_back(pEvent);
		}

		Unlock();
		return true;
	}

	void CScriptEventMgr::GetEvent(int nEventType, int nChannelID, std::vector<tagScriptEvent>& vOut)
	{
		Lock();
		auto& mapChannel = m_mapEvent[nEventType];
		auto itChannel = mapChannel.m_mapEventChannel.find(nChannelID);
		if (itChannel != mapChannel.m_mapEventChannel.end())
		{
			tagEventChannel& eventChannel = itChannel->second;
			for (auto it = eventChannel.listEvent.begin(); it != eventChannel.listEvent.end(); it++)
			{
				tagScriptEvent* pEvent = *it;
				if (pEvent)
				{
					vOut.push_back(*pEvent);
				}
			}
			eventChannel.clear();
		}
		Unlock();
	}


	void CScriptEventMgr::ProcessEvent(int nEventType, int nID, EventProcessFun const& fun)
	{
		std::vector<tagScriptEvent> vEvent;

		//获取所有对应事件
		GetEvent(nEventType, nID, vEvent);


		for (int i = 0; i < vEvent.size(); i++)
		{
			fun(vEvent[i].nSendID, vEvent[i].m_Parm);
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


}