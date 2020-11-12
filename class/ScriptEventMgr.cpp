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
		m_MaxDurationTime = 1000 * 60 * 10;
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

	__int64 CScriptEventMgr::AssignID()
	{
		__int64 nResult = 0;
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


	bool CScriptEventMgr::SendEvent(int nEventType, __int64 nSendID, CScriptStack& vIn, __int64 nRecvID)
	{
		auto nowTime = std::chrono::steady_clock::now();
		//查找一个合适的频道
		if (nRecvID == 0)
		{
			std::lock_guard<std::mutex> Lock(m_LockEventType);

			auto& eList = m_mapEventsByType[nEventType];
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - eList.lastTime);
			if (duration.count() >= m_MaxDurationTime)
			{
				return false;
			}
			tagScriptEvent* pEvent = NewEvent();
			pEvent->nSendID = nSendID;
			pEvent->nEventType = nEventType;
			while (vIn.size() > 0)
			{
				pEvent->m_Parm.push(vIn.top());
				vIn.pop();
			}
			eList.list.push_back(pEvent);
		}
		else
		{
			std::lock_guard<std::mutex> Lock(m_LockEventChannel);

			auto& channel = m_mapEventByChannel[nRecvID];
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - channel.lastTime);
			if (duration.count() >= m_MaxDurationTime)
			{
				return false;
			}
			tagScriptEvent* pEvent = NewEvent();
			pEvent->nSendID = nSendID;
			pEvent->nEventType = nEventType;
			while (vIn.size() > 0)
			{
				pEvent->m_Parm.push(vIn.top());
				vIn.pop();
			}
			channel.list.push_back(pEvent);
		}

		return true;
	}

	void CScriptEventMgr::SendEventForce(int nEventType, __int64 nSendID, CScriptStack& vIn, __int64 nRecvID)
	{
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
			eList.list.push_back(pEvent);
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
			channel.list.push_back(pEvent);
		}
	}

	void CScriptEventMgr::GetEventByType(int nEventType, std::vector<tagScriptEvent*>& vOut, int nSize)
	{
		std::lock_guard<std::mutex> Lock(m_LockEventType);

		auto itChannel = m_mapEventsByType.find(nEventType);
		if (itChannel != m_mapEventsByType.end())
		{
			tagListEvents& eventChannel = itChannel->second;
			eventChannel.lastTime = std::chrono::steady_clock::now();
			for (int i = 0; i < nSize; i++)
			{
				if (eventChannel.list.empty())
				{
					break;
				}
				tagScriptEvent* pEvent = eventChannel.list.front();
				if (pEvent)
				{
					vOut.push_back(pEvent);
				}
				eventChannel.list.pop_front();
			}
		}
	}

	void CScriptEventMgr::GetEventByChannel(__int64 nChannelID, std::vector<tagScriptEvent*>& vOut)
	{
		std::lock_guard<std::mutex> Lock(m_LockEventChannel);

		auto itChannel = m_mapEventByChannel.find(nChannelID);
		if (itChannel != m_mapEventByChannel.end())
		{
			tagListEvents& eventChannel = itChannel->second;
			eventChannel.lastTime = std::chrono::steady_clock::now();
			for (auto it = eventChannel.list.begin(); it != eventChannel.list.end(); it++)
			{
				tagScriptEvent* pEvent = *it;
				if (pEvent)
				{
					vOut.push_back(pEvent);
				}
			}
			eventChannel.list.clear();
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