#pragma once
/****************************************************************************
	Copyright (c) 2020 ZhuLei
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
#include "scriptcommon.h"
#include "ScriptStack.h"
#include "ScriptExecBlock.h"

 //*************************************************************************************//
 //									脚本的运行状态							   //
 //*************************************************************************************//

namespace zlscript
{
	enum
	{
		ERunTime_Error,
		ERunTime_Continue,
		ERunTime_Complete,
		ERunTime_Waiting,
		ERunTime_CallScriptFun,
	};

	class CScriptVirtualMachine;
	class CScriptRunState
	{
	public:
		CScriptRunState();
		virtual ~CScriptRunState();
	protected:
		__int64 m_id;

		static __int64 s_nIDSum;

	public:

		CScriptVirtualMachine* m_pMachine;
	public:
		//__int64 nNetworkID;//如果是网络调用，网络连接的ID
		__int64 nCallEventIndex;//被调用的事件频道ID
		__int64 m_CallReturnId;//被调用的事件频道ID
		std::string FunName;

		unsigned int nRunCount;

	public:
		void SetWatingTime(unsigned int nTime);

		unsigned int m_WatingTime;

	public:
		//最后一次运行的时间
		std::chrono::steady_clock::time_point m_timeLastRunTime;
		StackVarInfo m_varReturn;
		CScriptExecBlockStack m_BlockStack;

		//int CurCallFunParamNum;//当前调用函数的参数数量
		//int CurStackSizeWithoutFunParam;//除了函数参数，堆栈的大小
	public:
		virtual bool PushEmptyVarToStack();
		virtual bool PushVarToStack(int nVal);
		virtual bool PushVarToStack(__int64 nVal);
		virtual bool PushVarToStack(double Double);
		virtual bool PushVarToStack(const char* pstr);
		virtual bool PushClassPointToStack(__int64 nIndex);
		virtual bool PushClassPointToStack(CScriptBasePointer* pPoint);
		virtual bool PushVarToStack(StackVarInfo& Val);

		template<class T>
		bool PushClassPointToStack(T* pVal);

		virtual __int64 PopIntVarFormStack();
		virtual double PopDoubleVarFormStack();
		virtual char* PopCharVarFormStack();
		virtual PointVarInfo PopClassPointFormStack();
		virtual StackVarInfo PopVarFormStack();

		virtual int GetParamNum();


		virtual void CopyToStack(CScriptStack* pStack, int nNum);
		virtual void CopyFromStack(CScriptStack* pStack);

		//获取函数变量
		virtual void ClearFunParam();
		//void ClearFunParam(int nKeepNum);

		void ClearStack();
		void ClearExecBlock(bool bPrint = false);

		void PrintExecBlock();
		void ClearAll();

		int CallFun(CScriptVirtualMachine* pMachine, CScriptExecBlock* pCurBlock, int nType, int FunIndex, int nParmNum, bool bIsBreak = false);
		int CallFun(CScriptVirtualMachine* pMachine, int nType, int FunIndex, CScriptStack& ParmStack, bool bIsBreak = false);
		int CallFun(CScriptVirtualMachine* pMachine, const char* pFunName, CScriptStack& ParmStack, bool bIsBreak = false);

		int CallFunImmediately(CScriptVirtualMachine* pMachine, const char* pFunName, CScriptStack& ParmStack);

		unsigned int Exec(unsigned int unTimeRes, CScriptVirtualMachine* pMachine);

		__int64 GetId() const
		{
			return m_id;
		}
		void SetId(__int64 val)
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
			CScriptBasePointer* pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(pVal->GetScriptPointIndex());
			PushClassPointToStack(pPoint);
			return true;
		}
		else
		{
			PushClassPointToStack((CScriptBasePointer*)nullptr);
		}
		return false;
	}
}