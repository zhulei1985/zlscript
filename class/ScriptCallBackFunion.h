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

	class CScriptRunState;
	class CScriptVirtualMachine;
	typedef int (*C_CallBackScriptFunion)(CScriptVirtualMachine*, CScriptRunState*);


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
		static int RunScript(CScriptVirtualMachine* pMachine, CScriptRunState* pState);
		static int RunScriptToChannel(CScriptVirtualMachine* pMachine, CScriptRunState* pState);
		static int StopScript(CScriptVirtualMachine* pMachine, CScriptRunState* pState);
		static int CheckScriptRun(CScriptVirtualMachine* pMachine, CScriptRunState* pState);

		static int print(CScriptVirtualMachine* pMachine, CScriptRunState* pState);
		static int Printf(CScriptVirtualMachine* pMachine, CScriptRunState* pState);

		static int getrand(CScriptVirtualMachine* pMachine, CScriptRunState* pState);
		static int wait(CScriptVirtualMachine* pMachine, CScriptRunState* pState);

		static int TransRadian(CScriptVirtualMachine* pMachine, CScriptRunState* pState);
		static int CheckClassPoint(CScriptVirtualMachine* pMachine, CScriptRunState* pState);

		static int SetEventTrigger(CScriptVirtualMachine* pMachine, CScriptRunState* pState);
		static int TriggerEvent(CScriptVirtualMachine* pMachine, CScriptRunState* pState);
		static int RemoveEventTrigger(CScriptVirtualMachine* pMachine, CScriptRunState* pState);

		static int NewArray(CScriptVirtualMachine* pMachine, CScriptRunState* pState);
		static int ReleaseArray(CScriptVirtualMachine* pMachine, CScriptRunState* pState);

		static int GetData(CScriptVirtualMachine* pMachine, CScriptRunState* pState);
		static int ReleaseData(CScriptVirtualMachine* pMachine, CScriptRunState* pState);
		static int GetVal4Data(CScriptVirtualMachine* pMachine, CScriptRunState* pState);
		static int SetVal2Data(CScriptVirtualMachine* pMachine, CScriptRunState* pState);


	};
}