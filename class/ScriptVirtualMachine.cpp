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

#include "scriptcommon.h"

#include <stdarg.h>
#include <chrono>

#include "EScriptSentenceType.h"
#include "EScriptVariableType.h"


#include "ScriptVirtualMachine.h"
#include "ScriptEventMgr.h"
#include "ScriptDebugPrint.h"
namespace zlscript
{

	//CScriptVirtualMachine  *CScriptVirtualMachine::m_pInstance = nullptr;
	CScriptVirtualMachine::CScriptVirtualMachine()
	{
		nMaxTimeRes = 9999;
		init(0);
	}
	CScriptVirtualMachine::CScriptVirtualMachine(int nThread)
	{
		nMaxTimeRes = 9999;
		init(nThread);
	}
	;

	CScriptVirtualMachine::~CScriptVirtualMachine()
	{
		clear();
		//if (m_xCtrl)
		//{
		//	delete m_xCtrl;
		//}
	};

	CScriptVirtualMachine* CScriptVirtualMachine::CreateNew()
	{
		return new CScriptVirtualMachine();
	}
	//bool CScriptVirtualMachine::NewInstance()
	//{
	//	m_pInstance = new CScriptVirtualMachine;
	//	return true;
	//}
	//CScriptVirtualMachine *CScriptVirtualMachine::GetInstance()
	//{
	//	return m_pInstance;
	//}
	//bool CScriptVirtualMachine::DelInstance()
	//{
	//	SAFE_DELETE(m_pInstance);
	//	return true;
	//}

	void CScriptVirtualMachine::init(int nThread)
	{
		clear();

		CScriptCodeLoader::GetInstance()->GetGlobalVar(vGlobalNumVar);

		CScriptExecFrame::OnInit();
		m_nThreadRunState = 1;
		for (int i = 0; i < nThread; i++)
		{
			std::thread tbg(&CScriptVirtualMachine::RunningThreadFun, this);
			tbg.detach();
		}
		if (nThread <= 0)
		{
			bNoThread = true;
		}
		else
		{
			bNoThread = false;
		}
	}

	void CScriptVirtualMachine::clear()
	{
		//CSingleLock xLock(m_xCtrl, true);

		listRunState::iterator it = m_RunStateList.begin();
		for (;it != m_RunStateList.end(); it++)
		{
			CScriptRunState *pState = *it;
			SAFE_DELETE(pState);
		}
		m_RunStateList.clear();
		//vGlobalNumVar.clear();
	}

	void CScriptVirtualMachine::SetEventIndex(int val)
	{

		m_nEventListIndex = val;

	}

	void CScriptVirtualMachine::RunningThreadFun()
	{
		while (m_nThreadRunState > 0)
		{
			if (m_nThreadRunState == 2)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}
			//从队列中获取一个等待执行的运行状态
			CScriptRunState* pState = PopStateFormRunList();

			if (pState == nullptr)
			{
				//没有可运行的状态，休眠一下
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			if (pState->GetEnd())
			{
				//直接删除
			}

			auto nowTime = std::chrono::steady_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - pState->m_timeLastRunTime);
			pState->m_timeLastRunTime = nowTime;
			int nResult = pState->Exec(nMaxTimeRes, this);
			switch (nResult)
			{
			case ERunTime_Error:
			case ERunTime_Complete:
				if (pState->m_CallReturnId != 0)
				{
					CScriptStack vRetrunVars;
					ScriptVector_PushVar(vRetrunVars, &pState->PopVarFormStack());
					ResultTo(vRetrunVars, pState->m_CallReturnId, pState->nCallEventIndex);

				}
				break;
			case ERunTime_Waiting:
				if (PushStateToWaitingReturnMap(pState))
				{
					pState = nullptr;
				}
				break;
			case ERunTime_Continue:
				if (pState->m_WatingTime > 0)
				{
					PushStateToWaitingTimeList(pState);
					pState = nullptr;
				}
				else
				{
					PushStateToRunList(pState);
					pState = nullptr;
				}
				break;
			}

			if (pState)
			{
				//释放掉
				//TODO 加入缓存
				SAFE_DELETE(pState);
			}
		}

	}

	void CScriptVirtualMachine::RunningThreadFun2()
	{
		while (m_nThreadRunState > 0)
		{
			if (m_nThreadRunState == 2)
			{
				return;
			}
			//从队列中获取一个等待执行的运行状态
			CScriptRunState* pState = PopStateFormRunList();

			if (pState == nullptr)
			{
				//没有可运行的状态
				return;
			}

			if (pState->GetEnd())
			{
				//直接删除
			}

			auto nowTime = std::chrono::steady_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - pState->m_timeLastRunTime);
			pState->m_timeLastRunTime = nowTime;
			int nResult = pState->Exec(nMaxTimeRes, this);
			switch (nResult)
			{
			case ERunTime_Error:
			case ERunTime_Complete:
				if (pState->m_CallReturnId != 0)
				{
					CScriptStack vRetrunVars;
					ScriptVector_PushVar(vRetrunVars, &pState->PopVarFormStack());
					ResultTo(vRetrunVars, pState->m_CallReturnId, pState->nCallEventIndex);

				}
				break;
			case ERunTime_Waiting:
				if (PushStateToWaitingReturnMap(pState))
				{
					pState = nullptr;
				}
				break;
			case ERunTime_Continue:
				if (pState->m_WatingTime > 0)
				{
					PushStateToWaitingTimeList(pState);
					pState = nullptr;
				}
				else
				{
					PushStateToRunList(pState);
					pState = nullptr;
				}
				break;
			}

			if (pState)
			{
				//释放掉
				//TODO 加入缓存
				SAFE_DELETE(pState);
			}
		}
	}

	bool CScriptVirtualMachine::RunFunImmediately(std::string name, CScriptStack& ParmStack)
	{
		//TODO 日后添加缓存
		CScriptRunState* pState = new CScriptRunState();
		pState->CallFunImmediately(this, name.c_str(), ParmStack);
		delete pState;
		return false;
	}

	//bool CScriptVirtualMachine::RunFun(string funname)
	//{
	//	CScriptRunState *pState = new CScriptRunState();
	//
	//	map<string,int>::iterator it = m_CodeLoader.m_mapString2CodeIndex.find(funname);
	//	if (it == m_CodeLoader.m_mapString2CodeIndex.end())
	//	{
	//		return false;
	//	}
	//	pState->vGlobalNumVar = m_CodeLoader.vGlobalNumVar;
	//	CallFun(pState,1,it->second,0);
	//	m_listRunState.push_back(pState);
	//	
	//	return true;
	//}
	bool CScriptVirtualMachine::RunFun(CScriptRunState *pState, std::string funname,const char *sFormat,...)
	{
		if (pState == nullptr)
		{
			return false;
		}
		if (funname.empty())
		{
			return false;
		}
		CScriptStack varRegister;
		if (sFormat)
		{
			int nlen = strlen(sFormat);
			va_list argptr;

			va_start( argptr, sFormat);   

			char ch = 0;
			for (int i = 0; i < nlen; i++)
			{
				ch = *(sFormat + i);
				if (ch == '%')
				{
					i++;
					if (i < nlen)
					{
						void *pPoint = nullptr;
						pPoint = va_arg(argptr, void*); 
						switch (*(sFormat + i))
						{
						case 'd':
							ScriptVector_PushVar(varRegister, *(__int64*)pPoint);
							break;
						case 's':
							ScriptVector_PushVar(varRegister, (const char*)pPoint);
							break;
						}
					}
				}
			}
			va_end(argptr);
		}

		pState->m_pMachine = this;
		pState->FunName = funname;

		if (pState->CallFun(this,funname.c_str(), varRegister) == ECALLBACK_ERROR)
		{
			SCRIPT_PRINT("script","Cannot Process runscript: %s",funname.c_str());
		}
		SCRIPT_PRINT("script","Process runscript: %s",funname.c_str());

		if (funname.empty())
		{
			//assert(0);
		}

		PushStateToRunList(pState);
		//m_RunStateList.push_back(pState);

		//char cbuff[1024];
		//sprintf(cbuff, "Script State Add 1: %d:", pState);
		//zlscript::CScriptDebugPrintMgr::GetInstance()->Print(cbuff);
		return true;
	}

	bool CScriptVirtualMachine::RunFun(CScriptRunState* pState, std::string funname, CScriptStack& ParmStack, bool bIsBreak)
	{
		pState->m_pMachine = this;
		pState->FunName = funname;

		if (pState->CallFun(this, funname.c_str(), ParmStack, bIsBreak) == ECALLBACK_ERROR)
		{
			SCRIPT_PRINT("script", "Cannot Process runscript: %s", funname.c_str());
		}
		SCRIPT_PRINT("script", "Process runscript: %s", funname.c_str());

		if (funname.empty())
		{
			//assert(0);
		}

		PushStateToRunList(pState);
		//char cbuff[1024];
		//sprintf(cbuff, "Script State Add 2: %d:", pState);
		//zlscript::CScriptDebugPrintMgr::GetInstance()->Print(cbuff);
		return true;
	}

	unsigned int CScriptVirtualMachine::Exec(unsigned int nDelay,unsigned int unTimeRes)
	{
		CScriptExecFrame::OnUpdate();

		auto nowTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
		unsigned int nNowTime = nowTime.time_since_epoch().count();
		auto pState = PopStateFormWaitingTimeList(nNowTime);
		while (pState)
		{
			pState->m_WatingTime = 0;
			PushStateToRunList(pState);
			pState = PopStateFormWaitingTimeList(nNowTime);
		}

		if (bNoThread)
		{
			RunningThreadFun2();
		}
		////删除
		//for (auto itDel = m_listDel.begin(); itDel != m_listDel.end(); itDel++)
		//{
		//	std::map<unsigned long, CScriptRunState*>::iterator itWait = m_mapWaitingForReturn.find(*itDel);
		//	if (itWait != m_mapWaitingForReturn.end())
		//	{
		//		CScriptRunState* pState = itWait->second;
		//		if (pState && pState->GetId() == *itDel)
		//		{
		//			SAFE_DELETE(pState);
		//			m_mapWaitingForReturn.erase(itWait);
		//			break;
		//		}
		//	}
		//	listRunState::iterator itRun = m_RunStateList.begin();
		//	for (; itRun != m_RunStateList.end(); itRun++)
		//	{
		//		CScriptRunState* pState = *itRun;
		//		if (pState && pState->GetId() == *itDel)
		//		{
		//			SAFE_DELETE(pState);
		//			itRun = m_RunStateList.erase(itRun);
		//			break;
		//		}
		//	}

		//}
		//m_listDel.clear();
		return 0;
	}
	bool CScriptVirtualMachine::HasWaitingScript(unsigned long id)
	{
		std::lock_guard<std::mutex> Lock(m_WaitReturnLock);
		std::map<unsigned long,CScriptRunState *>::iterator itWait = m_mapWaitingForReturn.find(id);
		if (itWait != m_mapWaitingForReturn.end())
		{
			return true;
		}
		return false;
	}

	//bool CScriptVirtualMachine::RemoveRunState(unsigned long id)
	//{
	//	listRunState::iterator it = m_RunStateList.begin();
	//	for (; it != m_RunStateList.end(); it++)
	//	{
	//		CScriptRunState *pState = *it;
	//		if (pState->GetId() == id)
	//		{
	//			pState->SetEnd(true);
	//			break;
	//		}
	//	}
	//	//return false;
	//	m_listDel.push_back(id);
	//	return true;
	//}
	//bool CScriptVirtualMachine::RemoveRunStateByShape(int id)
	//{
	//	//listRunState::iterator it = m_RunStateList.begin();
	//	//for (; it != m_RunStateList.end(); )
	//	//{
	//	//	CScriptRunState *pState = *it;
	//	//	if (pState && pState->GetShape() && pState->GetShape()->GetID() == id)
	//	//	{
	//	//		delete pState;
	//	//		it = m_RunStateList.erase(it);
	//	//	}
	//	//	else
	//	//	{
	//	//		it++;
	//	//	}
	//	//}
	//	return true;
	//}
	//bool CScriptVirtualMachine::CheckRun(__int64 id)
	//{
	//	listRunState::iterator it = m_RunStateList.begin();
	//	for (; it != m_RunStateList.end(); it++)
	//	{
	//		CScriptRunState *pState = *it;
	//		if (pState->GetId() == id)
	//		{
	//			return true;
	//		}
	//	}
	//	std::map<unsigned long, CScriptRunState *>::iterator itWait = m_mapWaitingForReturn.find(id);
	//	if (itWait != m_mapWaitingForReturn.end())
	//	{
	//		return true;
	//	}
	//	return false;
	//}
	//CScriptRunState *CScriptVirtualMachine::GetRunState(unsigned long id)
	//{
	//	listRunState::iterator it = m_RunStateList.begin();
	//	for (; it != m_RunStateList.end(); it++)
	//	{
	//		CScriptRunState *pState = *it;
	//		if (pState->GetId() == id)
	//		{
	//			return pState;
	//		}
	//	}
	//	return nullptr;
	//}
	//CScriptRunState *CScriptVirtualMachine::PopRunState(unsigned long id)
	//{
	//	listRunState::iterator it = m_RunStateList.begin();
	//	for (; it != m_RunStateList.end(); it++)
	//	{
	//		CScriptRunState *pState = *it;
	//		if (pState->GetId() == id)
	//		{
	//			m_RunStateList.erase(it);
	//			return pState;
	//		}
	//	}
	//	return nullptr;
	//}
	StackVarInfo CScriptVirtualMachine::GetGlobalVar(unsigned int pos)
	{
		if (pos < vGlobalNumVar.size())
		{
			return vGlobalNumVar[pos];
		}
		return StackVarInfo();
	}

	void CScriptVirtualMachine::SetGlobalVar(unsigned int pos, StackVarInfo& val)
	{
		if (pos < vGlobalNumVar.size())
		{
			vGlobalNumVar[pos] = val;
		}
	}

	CScriptRunState* CScriptVirtualMachine::PopStateFormRunList()
	{
		std::lock_guard<std::mutex> Lock(m_RunStateLock);
		CScriptRunState* pState = nullptr;

		if (!m_RunStateList.empty())
		{
			pState = m_RunStateList.front();
			m_RunStateList.pop_front();
		}

		return pState;
	}

	void CScriptVirtualMachine::PushStateToRunList(CScriptRunState* pState)
	{
		std::lock_guard<std::mutex> Lock(m_RunStateLock);
		if (pState)
		{
			m_RunStateList.push_back(pState);
		}
	}

	CScriptRunState* CScriptVirtualMachine::PopStateFormWaitingReturnMap(__int64 nID)
	{
		std::lock_guard<std::mutex> Lock(m_WaitReturnLock);
		CScriptRunState* pState = nullptr;
		auto it = m_mapWaitingForReturn.find(nID);
		if (it != m_mapWaitingForReturn.end())
		{
			pState = it->second;
			m_mapWaitingForReturn.erase(it);
		}
		return pState;
	}

	bool CScriptVirtualMachine::PushStateToWaitingReturnMap(CScriptRunState* pState)
	{
		std::lock_guard<std::mutex> Lock(m_WaitReturnLock);
		if (m_mapWaitingForReturn.find(pState->GetId()) != m_mapWaitingForReturn.end())
		{
			return false;
		}
		else
		{
			m_mapWaitingForReturn[pState->GetId()] = pState;
			return true;
		}
		return false;
	}

	CScriptRunState* CScriptVirtualMachine::PopStateFormWaitingTimeList(unsigned int nowTime)
	{
		std::lock_guard<std::mutex> Lock(m_WaitTimeLock);
		CScriptRunState* pState = nullptr;

		if (!m_ListWaitingForTime.empty())
		{
			pState = m_ListWaitingForTime.front();
			//需要检测一下等待时间是否到达
			if (pState->m_WatingTime > nowTime)
			{
				return nullptr;
			}
			m_ListWaitingForTime.pop_front();
		}

		return pState;
	}

	void CScriptVirtualMachine::PushStateToWaitingTimeList(CScriptRunState* pState)
	{
		std::lock_guard<std::mutex> Lock(m_WaitTimeLock);
		if (pState)
		{
			bool bInsert = false;
			for (auto it = m_ListWaitingForTime.begin(); it != m_ListWaitingForTime.end(); it++)
			{
				if ((*it)->m_WatingTime > pState->m_WatingTime)
				{
					m_ListWaitingForTime.insert(it, pState);
				}
			}
			if (!bInsert)
				m_ListWaitingForTime.push_back(pState);
		}
	}

	void CScriptVirtualMachine::EventReturnFun(int nSendID, CScriptStack& ParmInfo)
	{
		__int64 nScriptStateID = ScriptStack_GetInt(ParmInfo);

		auto pState = PopStateFormWaitingReturnMap(nScriptStateID);
		if (pState)
		{
			pState->CopyFromStack(&ParmInfo);
			PushStateToRunList(pState);
		}
	}
	void CScriptVirtualMachine::EventRunScriptFun(int nSendID, CScriptStack& ParmInfo)
	{
		__int64 nScriptStateID = ScriptStack_GetInt(ParmInfo);

		CScriptRunState* m_pScriptState = new CScriptRunState;
		if (m_pScriptState)
		{
			//__int64 nCallStateID = ScriptStack_GetInt(ParmInfo);
			std::string strScript = ScriptStack_GetString(ParmInfo);
			if (nScriptStateID != 0)
			{
				m_pScriptState->nCallEventIndex = nSendID;
				m_pScriptState->m_CallReturnId = nScriptStateID;
			}
			RunFun(m_pScriptState, strScript, ParmInfo);
		}
	}

	void CScriptVirtualMachine::RunTo(std::string funName, CScriptStack& pram, __int64 nReturnID, __int64 nEventIndex)
	{
		CScriptStack m_scriptParm;

		//ScriptVector_PushVar(m_scriptParm, GetEventIndex());
		ScriptVector_PushVar(m_scriptParm, nReturnID);


		ScriptVector_PushVar(m_scriptParm, funName.c_str());
		for (unsigned int i = 0; i < pram.size(); i++)
		{

			ScriptVector_PushVar(m_scriptParm, pram.GetVal(i));
		}
		CScriptEventMgr::GetInstance()->SendEvent(E_SCRIPT_EVENT_RUNSCRIPT, GetEventIndex(), m_scriptParm, nEventIndex);
	}
	void CScriptVirtualMachine::ResultTo(CScriptStack& pram, __int64 nReturnID, __int64 nEventIndex)
	{
		CScriptStack vRetrunVars;

		ScriptVector_PushVar(vRetrunVars, nReturnID);
		for (unsigned int i = 0; i < pram.size(); i++)
		{
			ScriptVector_PushVar(vRetrunVars, pram.GetVal(i));
		}
		CScriptEventMgr::GetInstance()->SendEvent(E_SCRIPT_EVENT_RETURN, GetEventIndex(), vRetrunVars, nEventIndex);
	}
}