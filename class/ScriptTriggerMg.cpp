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

#include "ScriptTriggerMgr.h"
#include "ScriptVirtualMachine.h"
#include"ScriptEventMgr.h"
namespace zlscript
{
	CScriptTriggerMgr CScriptTriggerMgr::s_Instance;
	CScriptTriggerMgr::CScriptTriggerMgr()
	{

	}

	CScriptTriggerMgr::~CScriptTriggerMgr()
	{

	}


	void CScriptTriggerMgr::SetEventTrigger(std::string strEvent, __int64 nClassPoint, std::string flag, int nChannel, std::string strScriptName, CScriptStack& parm)
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
	void CScriptTriggerMgr::TriggerEvent(std::string strEvent, __int64 nClassPoint)
	{
		m_TriggerLock.lock();
		auto& classTrigger = m_mapTriggers[nClassPoint];
		auto& eventTrigger = classTrigger.vTriggers[strEvent];
		for (auto it = eventTrigger.vTriggers.begin(); it != eventTrigger.vTriggers.end(); it++)
		{
			CScriptStack vRetrunVars;

			//ScriptVector_PushVar(vRetrunVars, (__int64)E_SCRIPT_EVENT_RUNSCRIPT);
			ScriptVector_PushVar(vRetrunVars, (__int64)0);
			ScriptVector_PushVar(vRetrunVars, it->second.strScriptName.c_str());
			for (int i = 0; i < it->second.parm.size(); i++)
			{
				ScriptVector_PushVar(vRetrunVars, it->second.parm.GetVal(i));
			}

			CScriptEventMgr::GetInstance()->SendEvent(E_SCRIPT_EVENT_RUNSCRIPT,it->second.nEventnChannel, vRetrunVars);
		}
		m_TriggerLock.unlock();
	}
	void CScriptTriggerMgr::RemoveTrigger(std::string strEvent, __int64 nClassPoint, std::string flag)
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