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
#include <stack>
#include <vector>
#include "ScriptCodeLoader.h"
#include "ScriptStack.h"

#include <chrono>
//////////////////////////////////////////////////////////////////////////////
//                              脚本执行块                                  //
//////////////////////////////////////////////////////////////////////////////
namespace zlscript
{
	class CScriptRunState;
	class CScriptVirtualMachine;
	class CFunICode;

	class CScriptExecBlock
	{
	public:
		CScriptExecBlock(CFunICode* pCode, CScriptRunState* pMaster);
		~CScriptExecBlock(void);

		int GetDefaultReturnType();
		int GetFunType();


	private:

		CFunICode* m_pCurCode;

		//状态堆栈
		std::vector<int> m_vRunState;
		int nOtherState{0};
		//变量寄存器
		tagScriptVarStack loaclVarStack;
	public:
		tagScriptVarStack registerStack;
	public:
		CScriptRunState* m_pMaster;

		void RegisterLoaclVar(int index, int type);
		CBaseVar* GetLoaclVar(int index);

		void RegisterRunState(int size);
		int& GetRunState(int index);
		void RevertRunState(int index);
	public:
		enum ERESULT_TYPE
		{
			ERESULT_END,//结束运行
			ERESULT_CONTINUE,//继续运行
			ERESULT_NEXTCONTINUE,//暂停当前运行，下次继续
			ERESULT_WAITING,//暂停，等返回
			ERESULT_LOOP_HEAD,
			ERESULT_LOOP_END,
			ERESULT_ERROR,
		};
		int GetCurCodeSoureIndex();
		unsigned int ExecBlock(CScriptVirtualMachine* pMachine);



	//性能监控

		std::chrono::milliseconds m_msRunningTime;//运行时间
	public:
		friend class CScriptVirtualMachine;
		friend class CScriptRunState;
	};

	class CScriptExecBlockStack
	{
	public:
		CScriptExecBlockStack();
		~CScriptExecBlockStack();
	public:
		unsigned int size();
		bool empty();

		void push(CScriptExecBlock* pBlock);
		CScriptExecBlock* top();
		void pop();

		CScriptExecBlock* get(unsigned int i);
	protected:
		unsigned int unIndex;
		std::vector<CScriptExecBlock*> m_Vec;
	};
}
