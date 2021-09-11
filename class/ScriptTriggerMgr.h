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

namespace zlscript
{


	class CScriptTriggerMgr
	{
	public:
		CScriptTriggerMgr();
		~CScriptTriggerMgr();

		//触发器功能
	public:
		struct tagTrigger
		{
			int nEventnChannel;
			std::string strScriptName;
			tagScriptVarStack parm;
		};
		struct tagEventTriggers
		{
			int nEventID;
			std::map<std::string, tagTrigger> vTriggers;
		};
		struct tagClassTriggers
		{
			__int64 nClassPoint;
			std::map<std::string, tagEventTriggers> vTriggers;
		};

		void SetEventTrigger(std::string strEvent, __int64 nClassPoint, std::string flag, int nChannel, std::string strScriptName, tagScriptVarStack& parm);
		void TriggerEvent(std::string strEvent, __int64 nClassPoint);
		void RemoveTrigger(std::string strEvent, __int64 nClassPoint, std::string flag);
	protected:
		std::map<__int64, tagClassTriggers> m_mapTriggers;
		std::mutex m_TriggerLock;
	public:
		static CScriptTriggerMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptTriggerMgr s_Instance;
	};
}