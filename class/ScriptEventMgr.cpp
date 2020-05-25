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
		//Lock();
		//auto itList = m_mapEventByChannel.begin();
		//for (; itList != m_mapEventByChannel.end(); itList++)
		//{
		//	auto& ch = itList->second;
		//	ch.m_mapEventChannel.clear();
		//}
		//m_mapEventByChannel.clear();
		//Unlock();
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

	//void CScriptEventMgr::RegisterEvent(int nEventType, int nChannelID)
	//{
	//	Lock();
	//	auto& MapChannel = m_mapEventByChannel[nEventType];
	//	auto& channel = MapChannel.m_mapEventChannel[nChannelID];
	//	channel.nChannelID = nChannelID;
	//	Unlock();
	//}

	//void CScriptEventMgr::RemoveEvent(int nEventType, int nChannelID)
	//{
	//	auto& MapChannel = m_mapEventByChannel[nEventType];

	//	auto it = MapChannel.m_mapEventChannel.find(nChannelID);
	//	if (it != MapChannel.m_mapEventChannel.end())
	//	{
	//		MapChannel.m_mapEventChannel.erase(it);
	//	}
	//}

	//void CScriptEventMgr::SetEventBlock(int nEventType, int nID, bool IsBlock)
	//{
	//	auto& MapChannel = m_mapEventByChannel[nEventType];

	//	auto it = MapChannel.m_mapEventChannel.find(nID);
	//	if (it != MapChannel.m_mapEventChannel.end())
	//	{
	//		it->second.isBlocking = IsBlock;
	//	}
	//}


	bool CScriptEventMgr::SendEvent(int nEventType, int nSendID, CScriptStack& vIn, int nRecvID)
	{
		//查找一个合适的频道
		if (nRecvID == 0)
		{
			std::lock_guard<std::mutex> Lock(m_LockEventType);

			auto& eList = m_mapEventsByType[nEventType];

			tagScriptEvent* pEvent = NewEvent();
			pEvent->nSendID = nSendID;
			pEvent->nEventType = nEventType;
			while (vIn.size() > 0)
			{
				pEvent->m_Parm.push(vIn.top());
				vIn.pop();
			}
			eList.push_back(pEvent);
		}
		else
		{
			std::lock_guard<std::mutex> Lock(m_LockEventChannel);

			auto& channel = m_mapEventByChannel[nRecvID];

			tagScriptEvent* pEvent = NewEvent();
			pEvent->nSendID = nSendID;
			pEvent->nEventType = nEventType;
			while (vIn.size() > 0)
			{
				pEvent->m_Parm.push(vIn.top());
				vIn.pop();
			}
			channel.push_back(pEvent);
		}

		return true;
	}

	void CScriptEventMgr::GetEventByType(int nEventType, std::vector<tagScriptEvent*>& vOut, int nSize)
	{
		std::lock_guard<std::mutex> Lock(m_LockEventType);

		auto itChannel = m_mapEventsByType.find(nEventType);
		if (itChannel != m_mapEventsByType.end())
		{
			tagListEvents& eventChannel = itChannel->second;
			for (int i = 0; i < nSize; i++)
			{
				if (eventChannel.empty())
				{
					break;
				}
				tagScriptEvent* pEvent = eventChannel.front();
				if (pEvent)
				{
					vOut.push_back(pEvent);
				}
				eventChannel.pop_front();
			}
		}
	}

	void CScriptEventMgr::GetEventByChannel(int nChannelID, std::vector<tagScriptEvent*>& vOut)
	{
		std::lock_guard<std::mutex> Lock(m_LockEventChannel);

		auto itChannel = m_mapEventByChannel.find(nChannelID);
		if (itChannel != m_mapEventByChannel.end())
		{
			tagListEvents& eventChannel = itChannel->second;
			for (auto it = eventChannel.begin(); it != eventChannel.end(); it++)
			{
				tagScriptEvent* pEvent = *it;
				if (pEvent)
				{
					vOut.push_back(pEvent);
				}
			}
			eventChannel.clear();
		}
	}



	//void CScriptEventMgr::ProcessEvent(int nEventType, int nID, EventProcessFun const& fun)
	//{
	//	std::vector<tagScriptEvent> vEvent;

	//	//获取所有对应事件
	//	GetEvent(nEventType, nID, vEvent);


	//	for (int i = 0; i < vEvent.size(); i++)
	//	{
	//		fun(vEvent[i].nSendID, vEvent[i].m_Parm);
	//	}
	//}

	void CScriptEventMgr::Lock()
	{
		m_LockEventChannel.lock();
	}

	void CScriptEventMgr::Unlock()
	{
		m_LockEventChannel.unlock();
	}

	tagScriptEvent* CScriptEventMgr::NewEvent()
	{
		return new tagScriptEvent;;
	}

	void CScriptEventMgr::ReleaseEvent(tagScriptEvent* pPoint)
	{
		if (pPoint)
		{
			delete pPoint;
		}
	}


}