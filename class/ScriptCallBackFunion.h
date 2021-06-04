#pragma once
#include <vector>
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

#include <string>
#include<unordered_map>
#include "scriptcommon.h"
namespace zlscript
{

	class CScriptCallState;
	class CScriptVirtualMachine;
	typedef int (*C_CallBackScriptFunion)(CScriptVirtualMachine*, CScriptCallState*);


	class CScriptCallBackFunion
	{
	public:
		CScriptCallBackFunion(void);
		~CScriptCallBackFunion(void);

		void init();

	public:
		static CScriptCallBackFunion* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptCallBackFunion s_Instance;
	public:
		static C_CallBackScriptFunion GetFun(unsigned int index);
		static C_CallBackScriptFunion GetFun(const char* pFunName);
		static int GetFunIndex(std::string str);

		void RegisterFun(std::string str, C_CallBackScriptFunion pFun);
	public:
		std::vector<C_CallBackScriptFunion> m_vecFun;
		std::unordered_map<std::string, int> m_mapFunIndex;

	public:
		static int SetProcessID(CScriptVirtualMachine* pMachine, CScriptCallState* pState);
		static int RunScriptToChannel(CScriptVirtualMachine* pMachine, CScriptCallState* pState);

		static int GetMsTime(CScriptVirtualMachine* pMachine, CScriptCallState* pState);

		static int print(CScriptVirtualMachine* pMachine, CScriptCallState* pState);
		static int Printf(CScriptVirtualMachine* pMachine, CScriptCallState* pState);

		static int getrand(CScriptVirtualMachine* pMachine, CScriptCallState* pState);
		static int wait(CScriptVirtualMachine* pMachine, CScriptCallState* pState);

		static int TransRadian(CScriptVirtualMachine* pMachine, CScriptCallState* pState);
		static int CheckClassPoint(CScriptVirtualMachine* pMachine, CScriptCallState* pState);

		static int SetEventTrigger(CScriptVirtualMachine* pMachine, CScriptCallState* pState);
		static int TriggerEvent(CScriptVirtualMachine* pMachine, CScriptCallState* pState);
		static int RemoveEventTrigger(CScriptVirtualMachine* pMachine, CScriptCallState* pState);

		static int InitData(CScriptVirtualMachine* pMachine, CScriptCallState* pState);
		static int ReleaseData(CScriptVirtualMachine* pMachine, CScriptCallState* pState);
		static int GetVal4Data(CScriptVirtualMachine* pMachine, CScriptCallState* pState);
		static int SetVal2Data(CScriptVirtualMachine* pMachine, CScriptCallState* pState);


	};
}