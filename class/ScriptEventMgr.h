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

#include "ScriptStack.h"

namespace zlscript
{
	enum E_SCRIPT_GLOBAL_EVENT_INDEX
	{
		E_SCRIPT_EVENT_INDEX_UI,
		E_SCRIPT_EVENT_INDEX_GAME_PROCESS,

		E_SCRIPT_EVENT_INDEX_SCRIPT_DRAWING,
		E_SCRIPT_EVENT_INDEX_ASSIGN,
	};
	enum E_SCRIPT_EVENT_TYPE
	{
		E_SCRIPT_EVENT_NONE,
		E_SCRIPT_EVENT_RETURN,
		E_SCRIPT_EVENT_RUNSCRIPT,

		E_SCRIPT_EVENT_BEGIN_PROCESS,
		E_SCRIPT_EVENT_RUN_PROCESS,
		E_SCRIPT_EVENT_END_PROCESS,

		E_SCRIPT_EVENT_CREATE_UI,
		E_SCRIPT_EVENT_REMOVE_UI,
		E_SCRIPT_EVENT_CREATE_SCENE,
		E_SCRIPT_EVENT_REMOVE_SCENE,
		E_SCRIPT_EVENT_GET_CUR_SCENE,

		E_SCRIPT_EVENT_BASE_UI_OPER,
	};
	class tagScriptEvent
	{
	public:
		int nSendID;//发送ID
		int nRecvID;//接收ID

		//传递参数
		CScriptStack m_Parm;
	};
	class CScriptEventMgr
	{
	public:
		CScriptEventMgr();
		~CScriptEventMgr();
	public:
		int AssignID();

		bool SendEvent(int nSendID, int nRecvID, CScriptStack& vIn);
		tagScriptEvent* GetEvent(int nID);
		void PopEvent(int nID);
		int GetEventSize(int nID);

		void Lock();
		void Unlock();
	protected:
		typedef std::list<tagScriptEvent*> listEvent;
		std::map<int, listEvent> m_mapEvent;
		int m_nEventListCount;
		std::mutex m_Lock;

	public:
		static CScriptEventMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptEventMgr s_Instance;
	};
}