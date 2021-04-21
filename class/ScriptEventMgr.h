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
//#include <functional>

#include "ScriptStack.h"


namespace zlscript
{
	enum E_SCRIPT_GLOBAL_EVENT_CHANNEL
	{
		E_SCRIPT_EVENT_CHANNEL_ASSIGN,
	};
	enum E_SCRIPT_EVENT_TYPE
	{
		E_SCRIPT_EVENT_NONE,
		E_SCRIPT_EVENT_RETURN,//本地返回值
		//E_SCRIPT_EVENT_NEWTWORK_RETURN,//网络返回值
		E_SCRIPT_EVENT_RUNSCRIPT,//本地运行脚本
		//E_SCRIPT_EVENT_NETWORK_RUNSCRIPT,//网络运行脚本
	};

	class tagScriptEvent
	{
	public:
		__int64 nSendID;//发送ID
		int nEventType;//事件类型

		//传递参数
		CScriptStack m_Parm;
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
		__int64 AssignID();

		void RegisterChannel(__int64 nChannel);
		void RemoveChannel(__int64 nChannel);

		//压入事件，如果事件频道无人处理，返回失败
		bool SendEvent(int nEventType, __int64 nSendID, CScriptStack& vIn, __int64 nRecvID = 0);

		void GetEventByType(int nEventType, std::vector<tagScriptEvent*> &vOut, int nSize = 10);
		void GetEventByChannel(__int64 nChannelID, std::vector<tagScriptEvent*>& vOut);

		void Lock();
		void Unlock();

		//TODO 日后加上缓存
		tagScriptEvent* NewEvent();
		void ReleaseEvent(tagScriptEvent *pPoint);
	protected:
		struct tagListEvents
		{
			tagListEvents()
			{
				lastTime = std::chrono::steady_clock::now();
			}
			std::list<tagScriptEvent*> list;
			std::chrono::steady_clock::time_point lastTime;//最后获取事件的时间
		};
		//typedef std::list<tagScriptEvent*> tagListEvents;

		std::map<__int64, tagListEvents> m_mapEventByChannel;
		__int64 m_nEventListCount;
		std::mutex m_LockEventChannel;

		std::map<int, tagListEvents> m_mapEventsByType;
		std::mutex m_LockEventType;

		unsigned int m_MaxDurationTime;
	public:
		static CScriptEventMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptEventMgr s_Instance;
	};
}