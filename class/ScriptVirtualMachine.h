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
#include "ScriptEventMgr.h"
#include "ScriptExecFrame.h"
#include "ScriptRunState.h"
#include <vector>
#include <string>
#include <map>
#include <list>
#include <mutex>
namespace zlscript
{
	class CScriptStack;
	class CScriptVarialbe;
	class CCriticalSection;
	class CScriptVirtualMachine;



	enum EScript_Channel
	{
		EScript_Channel_NoLimit,//无限制
		EScript_Channel_User_Alone,//玩家一次只能运行一个,后请求的脚本将不能执行
		Escript_Channel_User_NoOverlap//玩家一次只能运行一个同名脚本
	};



	class CScriptVirtualMachine : public CScriptExecFrame
	{
	public:
		CScriptVirtualMachine();
		CScriptVirtualMachine(int nThread);

		virtual ~CScriptVirtualMachine(void);

		static CScriptVirtualMachine* CreateNew();

		virtual void init(int nThread);

		virtual void clear();

		void SetEventIndex(int val);



		//这个函数会一直尝试从执行队列里取出状态执行，即使队列为空
		void RunningThreadFun();
		//这个函数也会尝试从执行队列里取出状态执行，但队列为空时返回
		void RunningThreadFun2();
	public:
		//nDelay, 与上一次执行的间隔时间 毫秒
		//unTimeRes, 本次一共执行多少步脚本
		virtual unsigned int Exec(unsigned int nDelay, unsigned int unTimeRes = 1);

		//主要用于初始化等需要立即执行的脚本，不能异步执行，不要有异步调用的函数什么的，也不要有wait什么的
		bool RunFunImmediately(std::string name, CScriptStack& ParmStack);

		bool RunFun(CScriptRunState* pState, std::string funname, const char* sFormat, ...);
		bool RunFun(CScriptRunState* pState, std::string funname, CScriptStack& ParmStack, bool bIsBreak = false);

		bool HasWaitingScript(unsigned long id);

		//bool RemoveRunState(unsigned long id);
		//bool RemoveRunStateByShape(int id);
		//bool CheckRun(__int64 id);
		//CScriptRunState* GetRunState(unsigned long id);
		//CScriptRunState* PopRunState(unsigned long id);
		StackVarInfo GetGlobalVar(unsigned int pos);
		void SetGlobalVar(unsigned int pos, StackVarInfo& val);
	protected:
		std::mutex m_GlobalVarLock;
		std::vector<StackVarInfo> vGlobalNumVar;

		//****************运行管理******************//
		bool bNoThread;
		std::atomic_int m_nThreadRunState = 0;//0 退出 1 运行 2 暂停
		typedef std::list<CScriptRunState*> listRunState;
	protected:
		CScriptRunState* PopStateFormRunList();
		void PushStateToRunList(CScriptRunState* pState);


		CScriptRunState* PopStateFormWaitingReturnMap(__int64 nID);
		bool PushStateToWaitingReturnMap(CScriptRunState* pState);

		//弹出一个已经到期的等待状态
		CScriptRunState* PopStateFormWaitingTimeList(unsigned int nowTime);
		void PushStateToWaitingTimeList(CScriptRunState* pState);
	protected:
		int nMaxTimeRes;
		//等待执行的脚本
		listRunState m_RunStateList;
		std::mutex m_RunStateLock;

		//等待返回的脚本
		std::map<unsigned long, CScriptRunState*> m_mapWaitingForReturn;
		std::mutex m_WaitReturnLock;
		//需要延时执行的脚本
		listRunState m_ListWaitingForTime;
		std::mutex m_WaitTimeLock;

		std::list<unsigned long> m_listDel;
	public:
		friend class CScriptExecBlock;


		void EventReturnFun(int nSendID, CScriptStack& ParmInfo);
		void EventRunScriptFun(int nSendID, CScriptStack& ParmInfo);

		//"我"要求"别人"执行脚本
		virtual void RunTo(std::string funName, CScriptStack& pram, __int64 nReturnID, __int64 nEventIndex);
		//"我"向"别人"返回执行脚本的结果
		virtual void ResultTo(CScriptStack& pram, __int64 nReturnID, __int64 nEventIndex);
		////"别人"要求"我"执行脚本
		//virtual void RunFrom(std::string funName, CScriptStack& pram, __int64 nReturnID, __int64 nEventIndex);
		////"别人"向"我"返回执行脚本的结果
		//virtual void ResultFrom(CScriptStack& pram, __int64 nReturnID);
	};

}