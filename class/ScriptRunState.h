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
	class CScriptRunState;
	class CScriptCallState
	{
	public:
		CScriptCallState(CScriptRunState* pMaster)
		{
			m_pMaster = pMaster;
			m_varReturn = nullptr;
		}
		~CScriptCallState();
		__int64 GetMasterID();


		CBaseVar* GetVarFormStack(unsigned int index);

		virtual int GetParamNum();

		CBaseVar* GetResult();
		void SetResult(CBaseVar* Val);
	public:
		tagScriptVarStack m_stackRegister;
		std::string strBuffer;
		CScriptRunState* m_pMaster;

		CBaseVar* m_varReturn;
	};
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
		__int64 nCallEventIndex;//被调用的事件频道ID
		__int64 m_CallReturnId;//被调用的事件频道ID
		std::string FunName;

		unsigned int nRunCount;

		CBaseVar* m_varReturn{nullptr};
	public:
		void SetWatingTime(unsigned int nTime);

		unsigned int m_WatingTime;

	public:
		//最后一次运行的时间
		std::chrono::steady_clock::time_point m_timeLastRunTime;
		CBaseVar* m_pVarReturn;
		CScriptExecBlockStack m_BlockStack;

	public:
		
		virtual void CopyFromStack(tagScriptVarStack &pStack);

		//获取函数变量
		virtual void ClearFunParam();
		//void ClearFunParam(int nKeepNum);

		void ClearStack();
		void ClearExecBlock(bool bPrint = false);

		void PrintExecBlock();
		void ClearAll();

		int CallFun_CallBack(CScriptVirtualMachine* pMachine, int FunIndex, CScriptCallState* pCallState);
		int CallFun_Script(CScriptVirtualMachine* pMachine, int FunIndex, tagScriptVarStack& ParmStack, int nParmNum,  bool bIsBreak = false);
		int CallFun_Script(CScriptVirtualMachine* pMachine, int FunIndex, std::vector<const CBaseVar*>& vParams);

		int CallFun(CScriptVirtualMachine* pMachine, CScriptExecBlock* pCurBlock, int nType, int FunIndex, int nParmNum, bool bIsBreak = false);
		int CallFun(CScriptVirtualMachine* pMachine, int nType, int FunIndex, tagScriptVarStack& ParmStack, bool bIsBreak = false);
		int CallFun(CScriptVirtualMachine* pMachine, const char* pFunName, tagScriptVarStack& ParmStack, bool bIsBreak = false);

		int CallFunImmediately(CScriptVirtualMachine* pMachine, const char* pFunName, tagScriptVarStack& ParmStack);

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
	public:
		CBaseVar* NewVar(int type);
		void ReleaseVar(CBaseVar* pVar);
	protected:
		//变量缓存
		std::vector<tagScriptVarStack> m_mapVarCache;
		//std::unordered_map<int, tagScriptVarStack> m_mapVarCache;
	};

	//template<class T>
	//inline bool CScriptCallState::SetClassPointResult(T* pVal)
	//{
	//	if (pVal)
	//	{
	//		CScriptBasePointer* pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(pVal->GetScriptPointIndex());
	//		m_varReturn = pPoint;
	//		return true;
	//	}
	//	else
	//	{
	//		m_varReturn.Clear();
	//	}
	//	return false;
	//}
}