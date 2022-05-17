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

#include "ScriptCallBackFunion.h"
#include"ScriptCodeLoader.h"
#include "ScriptVirtualMachine.h"
#include "ScriptDataMgr.h"
#include "ScriptEventMgr.h"
#include "ScriptDebugPrint.h"
#include "ScriptClassMgr.h"
#include "ScriptClassAttributes.h"
#include "ScriptPointInterface.h"
namespace zlscript
{
	inline void InitScript()
	{
		RegisterVarType("int", CIntVar);
		RegisterVarType("float", CFloatVar);
		RegisterVarType("string", CStringVar);
		CScriptCallBackFunion::GetInstance()->init();
		//CScriptSuperPointerMgr::GetInstance()->Init();
		//CScriptData::Init2Script();
		//CScriptHashMap::Init2Script();
		//CScriptArray::Init2Script();
	}
	inline void LoadFile(const char * filename)
	{
		CScriptCodeLoader::GetInstance()->LoadFile(filename);
	}
	#define RUNSCRIPT(pMachine,scriptname,...) { \
		if (pMachine) \
		{ \
			zlscript::CScriptRunState* m_pScriptState = new zlscript::CScriptRunState; \
			if (m_pScriptState) \
			{ \
				pMachine->RunFun(m_pScriptState, scriptname, __VA_ARGS__); \
			} \
		} \
	}
	inline void RunScript(const char* pName, ...)
	{
		if (pName == nullptr)
		{
			return;
		}
		tagScriptVarStack vRetrunVars;
		CIntVar var;
		var.Set((__int64)0);
		CStringVar var2;
		var2.Set(pName);
		CBaseVar* pVar = &var;
		CBaseVar* pVar2 = &var2;
		CBaseVar* pTemp = nullptr;

		STACK_PUSH_COPY(vRetrunVars, pVar);
		STACK_PUSH_COPY(vRetrunVars, pVar2);
		CScriptEventMgr::GetInstance()->SendEvent(E_SCRIPT_EVENT_RUNSCRIPT,0, vRetrunVars);
	}


}