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

#include "scriptcommon.h"

//*************************************************************************************//
//									脚本的虚拟机（执行器）							   //
//*************************************************************************************//
#include "ScriptStack.h"
#include "ScriptExecBlock.h"
#include "ScriptCodeLoader.h"
#include "ScriptSuperPointer.h"
#include <vector>
#include <string>
#include <map>
#include <list>

namespace zlscript
{
	class CScriptStack;
	class CScriptVarialbe;
	class CCriticalSection;
	class CScriptVirtualMachine;

	enum E_SCRIPT_EVENT_TYPE
	{
		E_SCRIPT_EVENT_NONE,
		E_SCRIPT_EVENT_RETURN,
		E_SCRIPT_EVENT_RUNSCRIPT,
	};

	enum EScript_Channel
	{
		EScript_Channel_NoLimit,//无限制
		EScript_Channel_User_Alone,//玩家一次只能运行一个,后请求的脚本将不能执行
		Escript_Channel_User_NoOverlap//玩家一次只能运行一个同名脚本
	};
	class CScriptVirtualMachine;
	class CScriptRunState
	{
	public:
		CScriptRunState();
		virtual ~CScriptRunState();
	protected:
		unsigned long m_id;

		static unsigned long s_nIDSum;

	public:

		CScriptVirtualMachine* m_pMachine;
	public:
		int nCallEventIndex;//被调用的事件频道ID
		unsigned long m_CallStateId;//被调用的事件频道ID
		std::string FunName;

		unsigned int nRunCount;

	public:
		virtual void WatingTime(unsigned int nTime);
		unsigned int m_WatingTime;

	public:
		//CScriptStack m_varRegister;
		std::stack<CScriptExecBlock*> m_BlockStack;

		//int CurCallFunParamNum;//当前调用函数的参数数量
		//int CurStackSizeWithoutFunParam;//除了函数参数，堆栈的大小
	public:
		bool PushEmptyVarToStack();
		bool PushVarToStack(int nVal);
		bool PushVarToStack(__int64 nVal);
		bool PushVarToStack(double Double);
		bool PushVarToStack(const char* pstr);
		bool PushClassPointToStack(__int64 nIndex);

		bool PushVarToStack(StackVarInfo& Val);

		template<class T>
		bool PushClassPointToStack(T* pVal);

		__int64 PopIntVarFormStack();
		double PopDoubleVarFormStack();
		char* PopCharVarFormStack();
		__int64 PopClassPointFormStack();
		StackVarInfo PopVarFormStack();

		int GetParamNum();

		//void SetParamNum(int val)
		//{
		//	CurCallFunParamNum = val;

		//}
		//void CopyToRegister(CScriptRunState* pState, int nNum);

		void CopyFromStack(CScriptStack* pStack);

		//获取函数变量
		void ClearFunParam();
		//void ClearFunParam(int nKeepNum);

		void ClearStack();
		void ClearExecBlock(bool bPrint = false);

		void ClearAll();

		int CallFun(CScriptVirtualMachine* pMachine, CScriptExecBlock* pCurBlock, int nType, int FunIndex, int nParmNum,bool bIsBreak=false);
		int CallFun(CScriptVirtualMachine* pMachine, int nType, int FunIndex, CScriptStack& ParmStack, bool bIsBreak = false);
		int CallFun(CScriptVirtualMachine* pMachine, const char* pFunName, CScriptStack& ParmStack, bool bIsBreak = false);
		unsigned int Exec(unsigned int unTimeRes, CScriptVirtualMachine* pMachine);

		unsigned long GetId() const
		{
			return m_id;
		}
		void SetId(unsigned long val)
		{
			m_id = val;
		}

	public:
		void SetEnd(bool val)
		{
			IsEnd = val;
		}
		bool GetEnd()
		{
			return IsEnd;
		}
	protected:
		bool IsEnd;

		char strbuff[2048];
	};
	template<class T>
	inline bool CScriptRunState::PushClassPointToStack(T* pVal)
	{
		if (pVal)
		{
			PushClassPointToStack(pVal->GetScriptPointIndex());
			return true;
		}
		else
		{
			PushClassPointToStack(-1);
		}
		return false;
	}

	enum
	{
		ERunTime_Error,
		ERunTime_Continue,
		ERunTime_Complete,
		ERunTime_Waiting,
		ERunTime_CallScriptFun,
	};
	class CScriptVirtualMachine
	{
	public:
		CScriptVirtualMachine();

		virtual ~CScriptVirtualMachine(void);

		static CScriptVirtualMachine* CreateNew();

		virtual void init();

		virtual void clear();

		void SetEventIndex(int val);
	public:
		//nDelay, 与上一次执行的间隔时间 毫秒
		//unTimeRes, 本次一共执行多少步脚本
		virtual unsigned int Exec(unsigned int nDelay, unsigned int unTimeRes = 1);

		bool RunFun(CScriptRunState* pState, std::string funname, const char* sFormat, ...);
		bool RunFun(CScriptRunState* pState, std::string funname, CScriptStack& ParmStack, bool bIsBreak = false);

		bool HasWaitingScript(unsigned long id);

		bool RemoveRunState(unsigned long id);
		bool RemoveRunStateByShape(int id);
		bool CheckRun(__int64 id);
		CScriptRunState* GetRunState(unsigned long id);
		CScriptRunState* PopRunState(unsigned long id);
		CScriptCodeLoader::VarPoint* GetGlobalVar(unsigned int pos);
	protected:

		std::vector<CScriptCodeLoader::VarPoint> vGlobalNumVar;

		//****************运行管理******************//
		typedef std::list<CScriptRunState*> listRunState;
		//vector<ListRunState> m_vRunStateList;
		listRunState m_RunStateList;

		std::map<unsigned long, CScriptRunState*> m_mapWaiting;

		std::list<unsigned long> m_listDel;
	public:
		friend class CScriptExecBlock;

		int m_nEventListIndex;
	};

}