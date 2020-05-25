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

#pragma once
#include <vector>
#include <list>
#include <map>
#include <mutex>
#include <functional>

#include "ScriptStack.h"

typedef std::function<void(int, zlscript::CScriptStack&)> EventProcessFun;

namespace zlscript
{
	enum E_SCRIPT_GLOBAL_EVENT_CHANNEL
	{
		E_SCRIPT_EVENT_CHANNEL_ASSIGN,
	};


	class tagScriptEvent
	{
	public:
		int nSendID;//发送ID
		int nEventType;//事件类型

		//传递参数
		CScriptStack m_Parm;
	};
	class tagEventChannel
	{
	public:
		tagEventChannel()
		{
			nChannelID = 0;
			isBlocking = false;
		}
		~tagEventChannel()
		{
			clear();
		}
		void clear();
	public:
		int nChannelID;
		bool isBlocking;
		std::list<tagScriptEvent*> listEvent;
	};
	class tagMapEventChannel
	{
	public:
		tagMapEventChannel()
		{

		}
		~tagMapEventChannel()
		{

		}
	public:
		std::map<int, tagEventChannel> m_mapEventChannel;
	};
	//2020.5.24 新的修改计划：
	//频道ID依然需要各个部分自己注册，但是不限事件类型了
	//事件分为指定频道和不指定两种，指定频道会直接发给对应频道，不指定的话会根据事件分别保存，等待频道自己取
	class CScriptEventMgr
	{
	public:
		CScriptEventMgr();
		~CScriptEventMgr();
	public:
		int AssignID();

		//void RegisterEvent(int nEventType, int nChannelID);
		//void RemoveEvent(int nEventType, int nChannelID);

		//void SetEventBlock(int nEventType, int nID, bool IsBlock);

		bool SendEvent(int nEventType, int nSendID, CScriptStack& vIn, int nRecvID = 0);
		void GetEventByType(int nEventType, std::vector<tagScriptEvent*> &vOut, int nSize = 10);
		void GetEventByChannel(int nChannelID, std::vector<tagScriptEvent*>& vOut);


		//void ProcessEvent(int nEventType, int nID, EventProcessFun const& fun);

		void Lock();
		void Unlock();

		//TODO 日后加上缓存
		tagScriptEvent* NewEvent();
		void ReleaseEvent(tagScriptEvent *pPoint);
	protected:
		typedef std::list<tagScriptEvent*> tagListEvents;

		std::map<int, tagListEvents> m_mapEventByChannel;
		int m_nEventListCount;
		std::mutex m_LockEventChannel;

		std::map<int, tagListEvents> m_mapEventsByType;
		std::mutex m_LockEventType;
	public:
		static CScriptEventMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptEventMgr s_Instance;
	};
}