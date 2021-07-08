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
#include "EMicroCodeType.h"
#include <chrono>
//////////////////////////////////////////////////////////////////////////////
//                              脚本执行块                                  //
//////////////////////////////////////////////////////////////////////////////
namespace zlscript
{
	class CScriptRunState;
	class CScriptVirtualMachine;


	class CScriptExecBlock
	{
	public:
		CScriptExecBlock(CScriptCodeLoader::tagCodeData* pData, CScriptRunState* pMaster);
		~CScriptExecBlock(void);

		int GetDefaultReturnType();
		int GetFunType();

		inline __int64 GetVal_Int64(char cType, unsigned int pos);
		StackVarInfo GetVal(char cType, unsigned int pos);
	private:

		CScriptRunState* m_pMaster;
		//指向的代码块
		CScriptCodeLoader::tagCodeData* m_pCodeData;
		//执行位置
		unsigned int m_nCodePoint;

	private:

		//寄存器
		StackVarInfo m_register[R_SIZE];
		//堆栈
		CScriptStack m_stackRegister;

		char m_cReturnRegisterIndex;//返回值放入的寄存器索引
		StackVarInfo m_varReturnVar;//返回值
	public:
		enum ERESULT_TYPE
		{
			ERESULT_END,
			ERESULT_CONTINUE,
			ERESULT_ERROR,
			ERESULT_RETURN,
			ERESULT_WAITING,
			ERESULT_CALLSCRIPTFUN,
			ERESULT_NEXTCONTINUE
		};
		CodeStyle GetCurCode();
		unsigned int ExecBlock(CScriptVirtualMachine* pMachine);

		inline bool CheckRegisterTrue(char index);

		void PushVar(StackVarInfo& var);
		StackVarInfo PopVar();
		StackVarInfo* GetVar(unsigned int index);
		unsigned int GetVarSize();

		StackVarInfo& GetReturnVar()
		{
			return m_varReturnVar;
		}
		char GetReturnRegisterIndex()
		{
			return m_cReturnRegisterIndex;
		}
		void ClearStack();

		std::string GetCurSourceWords();
	private:
		StackVarInfo* m_pTempVar;//临时变量
		unsigned int m_nTempVarSize;
		//std::vector<StackVarInfo> vNumVar;//临时变量


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
