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

#include "ScriptExecBlock.h"
#include "ScriptVirtualMachine.h"

#include "ScriptEventMgr.h"
#include "ScriptTriggerMgr.h"
#include "ScriptDataMgr.h"

#include <math.h>
#include <chrono>

#include "ScriptCallBackFunion.h"
//#include "tagTime.h"
//std::vector<C_CallBackScriptFunion> CScriptCallBackFunion::m_vecFun;
//std::unordered_map<std::string,int> CScriptCallBackFunion::m_mapFunIndex;
namespace zlscript
{

	CScriptCallBackFunion CScriptCallBackFunion::s_Instance;
	CScriptCallBackFunion::CScriptCallBackFunion(void)
	{

	}

	CScriptCallBackFunion::~CScriptCallBackFunion(void)
	{
	}

	void CScriptCallBackFunion::init()
	{
		RegisterFun("SetProcessID", (C_CallBackScriptFunion)SetProcessID);
		RegisterFun("setprocessid", (C_CallBackScriptFunion)SetProcessID);

		RegisterFun("RunScript", (C_CallBackScriptFunion)RunScriptToChannel);
		RegisterFun("runscript", (C_CallBackScriptFunion)RunScriptToChannel);

		RegisterFun("RunScriptToChannel", (C_CallBackScriptFunion)RunScriptToChannel);
		RegisterFun("runscripttochannel", (C_CallBackScriptFunion)RunScriptToChannel);

		RegisterFun("StopScript", (C_CallBackScriptFunion)StopScript);
		RegisterFun("stopscript", (C_CallBackScriptFunion)StopScript);

		RegisterFun("CheckScriptRun", (C_CallBackScriptFunion)CheckScriptRun);
		RegisterFun("checkscriptrun", (C_CallBackScriptFunion)CheckScriptRun);

		RegisterFun("GetMsTime", (C_CallBackScriptFunion)GetMsTime);

		RegisterFun("print", (C_CallBackScriptFunion)print);
		RegisterFun("printf", (C_CallBackScriptFunion)Printf);
		RegisterFun("rand", (C_CallBackScriptFunion)getrand);
		RegisterFun("getrand", (C_CallBackScriptFunion)getrand);
		RegisterFun("wait", (C_CallBackScriptFunion)wait);

		RegisterFun("CheckClassPoint", (C_CallBackScriptFunion)CheckClassPoint);
		RegisterFun("checkclasspoint", (C_CallBackScriptFunion)CheckClassPoint);

		RegisterFun("TransRadian", (C_CallBackScriptFunion)TransRadian);

		RegisterFun("SetEventTrigger", (C_CallBackScriptFunion)SetEventTrigger);
		RegisterFun("TriggerEvent", (C_CallBackScriptFunion)TriggerEvent);
		RegisterFun("RemoveEventTrigger", (C_CallBackScriptFunion)RemoveEventTrigger);


		RegisterFun("NewArray", (C_CallBackScriptFunion)NewArray);
		RegisterFun("newarray", (C_CallBackScriptFunion)NewArray);

		RegisterFun("ReleaseArray", (C_CallBackScriptFunion)ReleaseArray);
		RegisterFun("releasearray", (C_CallBackScriptFunion)ReleaseArray);

		RegisterFun("GetData", (C_CallBackScriptFunion)GetData);
		RegisterFun("getdata", (C_CallBackScriptFunion)GetData);

		RegisterFun("ReleaseData", (C_CallBackScriptFunion)ReleaseData);
		RegisterFun("releasedata", (C_CallBackScriptFunion)ReleaseData);

		RegisterFun("GetVal4Data", (C_CallBackScriptFunion)GetVal4Data);
		RegisterFun("getval4data", (C_CallBackScriptFunion)GetVal4Data);

		RegisterFun("SetVal2Data", (C_CallBackScriptFunion)SetVal2Data);
		RegisterFun("setval2data", (C_CallBackScriptFunion)SetVal2Data);
	}
	C_CallBackScriptFunion CScriptCallBackFunion::GetFun(unsigned int index)
	{
		if (index < CScriptCallBackFunion::GetInstance()->m_vecFun.size())
		{
			return CScriptCallBackFunion::GetInstance()->m_vecFun[index];
		}
		return nullptr;
	}
	C_CallBackScriptFunion CScriptCallBackFunion::GetFun(const char* pFunName)
	{
		int index = GetFunIndex(pFunName);
		return GetFun(index);
	}
	int CScriptCallBackFunion::GetFunIndex(std::string str)
	{
		std::unordered_map<std::string, int>::iterator it = CScriptCallBackFunion::GetInstance()->m_mapFunIndex.find(str);
		if (it != CScriptCallBackFunion::GetInstance()->m_mapFunIndex.end())
		{
			return it->second;
		}
		return -1;
	}
	void CScriptCallBackFunion::RegisterFun(std::string str, C_CallBackScriptFunion pFun)
	{
		if (m_mapFunIndex.find(str) == m_mapFunIndex.end())
		{
			m_vecFun.push_back(pFun);
			m_mapFunIndex[str] = m_vecFun.size() - 1;
		}
		else
		{
			//打印警告

			int index = m_mapFunIndex[str];
			m_vecFun[index] = pFun;
		}
	}

	int CScriptCallBackFunion::SetProcessID(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}

		if (pMachine == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		short nVal = pState->PopIntVarFormStack();

		g_Process_ID = nVal;
		pState->ClearFunParam();
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::RunScript(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}

		if (pMachine == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		int nParmNum = pState->GetParamNum();

		int nHoldState = pState->PopIntVarFormStack();
		std::string scriptname = pState->PopCharVarFormStack();

		bool bResult = false;
		CScriptStack ParmStack;
		nParmNum -= 2;
		CScriptStack vTemp;
		for (int i = 0; i < nParmNum; i++)
		{
			ScriptVector_PushVar(vTemp, &pState->PopVarFormStack());
		}

		while (vTemp.size() > 0)
		{
			ScriptVector_PushVar(ParmStack, &vTemp.top());
			vTemp.pop();
		}

		if (nHoldState == 0)
		{
			CScriptRunState* m_pScriptState = new CScriptRunState;
			if (m_pScriptState)
			{
				bResult = pMachine->RunFun(m_pScriptState, scriptname, ParmStack);
			}
			pState->ClearFunParam();
			pState->PushVarToStack((int)m_pScriptState->GetId());
		}
		else
		{
			//TODO 此处有问题
			bResult = pMachine->RunFun(pState, scriptname, ParmStack,true);
			//pState->ClearFunParam();
			if (bResult)
				return ECALLBACK_NEXTCONTINUE;
			else
				return ECALLBACK_ERROR;
		}
		if (bResult)
			return ECALLBACK_FINISH;
		else
			return ECALLBACK_ERROR;
	}
	int CScriptCallBackFunion::RunScriptToChannel(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}

		if (pMachine == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		int nParmNum = pState->GetParamNum();
		CScriptStack vRetrunVars;
		int nIsWaiting = pState->PopIntVarFormStack();//是否等待调用函数完成
		//ScriptVector_PushVar(vRetrunVars, (__int64)E_SCRIPT_EVENT_RUNSCRIPT);
		if (nIsWaiting > 0)
			ScriptVector_PushVar(vRetrunVars, (__int64)pState->GetId());
		else
			ScriptVector_PushVar(vRetrunVars, (__int64)0);
		ScriptVector_PushVar(vRetrunVars, pState->PopCharVarFormStack());
		//ScriptVector_PushVar(vRetrunVars, this);
		nParmNum -= 2;
		//CScriptStack vTemp;
		for (int i = 0; i < nParmNum; i++)
		{
			//ScriptVector_PushVar(vTemp, &pState->PopVarFormStack());
			ScriptVector_PushVar(vRetrunVars, &pState->PopVarFormStack());
		}

		//while (vTemp.size() > 0)
		//{
		//	ScriptVector_PushVar(vRetrunVars, &vTemp.top());
		//	vTemp.pop();
		//}
		CScriptEventMgr::GetInstance()->SendEvent(E_SCRIPT_EVENT_RUNSCRIPT,pMachine->m_nEventListIndex, vRetrunVars);

		pState->ClearFunParam();
		if (nIsWaiting)
		{
			return ECALLBACK_WAITING;
		}
		else
			return ECALLBACK_FINISH;
	}
	int CScriptCallBackFunion::StopScript(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}

		if (pMachine == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		int nId = pState->PopIntVarFormStack();
		pMachine->RemoveRunState(nId);
		pState->ClearFunParam();
		return ECALLBACK_FINISH;
	}
	int CScriptCallBackFunion::CheckScriptRun(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}

		if (pMachine == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		int nId = pState->PopIntVarFormStack();
		int nResult = pMachine->CheckRun(nId) ? 1 : 0;
		pState->ClearFunParam();
		pState->PushVarToStack(nResult);
		return ECALLBACK_FINISH;
	}
	int CScriptCallBackFunion::GetMsTime(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}

		if (pMachine == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		auto nowTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime.time_since_epoch());
		pState->ClearFunParam();
		pState->PushVarToStack(duration.count());
		return ECALLBACK_FINISH;
	}
	int CScriptCallBackFunion::print(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}

		std::string str = pState->PopCharVarFormStack();
		printf("%s\n", str.c_str());

		pState->ClearFunParam();
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::Printf(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{

		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		std::string strReturn;
		char ch[2] = { 0,0 };
		std::string str = pState->PopCharVarFormStack();
		const char* pStr = str.c_str();
		for (unsigned int i = 0; i < str.size(); i++)
		{
			if (pStr[i] == '%')
			{
				if (i + 1 < str.size())
				{
					switch (pStr[i+1])
					{
					case 'd':
					case 'f':
					case 's':
						strReturn += pState->PopCharVarFormStack();
						break;
					default:
						break;
					}
					i++;
				}
			}
			else if (pStr[i] == '\\')
			{
				if (i + 1 < str.size())
				{
					i++;
					ch[0] = pStr[i];
					strReturn += ch;
				}
			}
			else
			{
				ch[0] = pStr[i];
				strReturn += ch;
			}
		}

		pState->ClearFunParam();
		pState->PushVarToStack(strReturn.c_str());
		return ECALLBACK_FINISH;
	}


	int CScriptCallBackFunion::getrand(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}


		int nval = 0x7fffffff;
		if (pState->GetParamNum() >= 1)
		{
			nval = (int)pState->PopIntVarFormStack();
			if (nval <= 0)
			{
				nval = 1;
			}
		}
		int nrand = rand() % nval;

		pState->ClearFunParam();
		pState->PushVarToStack(nrand);
		return ECALLBACK_FINISH;
	}
	int CScriptCallBackFunion::wait(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		if (pState->GetParamNum() >= 1)
		{
			pState->m_WatingTime = /*CMyTime::GetTime() + */pState->PopIntVarFormStack();
		}
		pState->ClearFunParam();
		return ECALLBACK_NEXTCONTINUE;
	}

	int CScriptCallBackFunion::TransRadian(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		__int64 nAngle = pState->PopIntVarFormStack();
		double fRadian = nAngle / 180.f * 3.1415926f;
		pState->ClearFunParam();
		pState->PushVarToStack(fRadian);
		return ECALLBACK_NEXTCONTINUE;
	}

	int CScriptCallBackFunion::CheckClassPoint(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		__int64 index = pState->PopClassPointFormStack();
		CScriptBasePointer* pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(index);
		pState->ClearFunParam();
		if (pPoint)
		{
			pState->PushVarToStack(1);
			CScriptSuperPointerMgr::GetInstance()->ReturnPointer(pPoint);
		}
		else
		{
			pState->PushVarToStack(0);
		}
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::SetEventTrigger(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		int nParmNum = pState->GetParamNum();
		__int64 nClassPoint = pState->PopClassPointFormStack();
		std::string strEvent = pState->PopCharVarFormStack();
		std::string strFlag = pState->PopCharVarFormStack();
		std::string strScript = pState->PopCharVarFormStack();

		nParmNum -= 4;
		CScriptStack vParmVars;
		for (int i = 0; i < nParmNum; i++)
		{
			ScriptVector_PushVar(vParmVars, &pState->PopVarFormStack());
		}
		CScriptTriggerMgr::GetInstance()->SetEventTrigger(strEvent, nClassPoint, strFlag, pMachine->m_nEventListIndex, strScript, vParmVars);
		pState->ClearFunParam();
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::TriggerEvent(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		__int64 nClassPoint = pState->PopClassPointFormStack();
		std::string strEvent = pState->PopCharVarFormStack();
		CScriptTriggerMgr::GetInstance()->TriggerEvent(strEvent, nClassPoint);
		pState->ClearFunParam();
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::RemoveEventTrigger(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		__int64 nClassPoint = pState->PopClassPointFormStack();
		std::string strEvent = pState->PopCharVarFormStack();
		std::string strFlag = pState->PopCharVarFormStack();
		CScriptTriggerMgr::GetInstance()->RemoveTrigger(strEvent, nClassPoint, strFlag);
		pState->ClearFunParam();
		return ECALLBACK_FINISH;
	}


	int CScriptCallBackFunion::NewArray(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}

		auto pArray = CScriptDataMgr::GetInstance()->NewArray();
		pState->ClearFunParam();
		pState->PushClassPointToStack(pArray);
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::ReleaseArray(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		__int64 nIndex = pState->PopClassPointFormStack();
		auto pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(nIndex);
		if (pPoint)
		{
			pPoint->Lock();
			auto pMaster = dynamic_cast<CScriptArray*>(pPoint->GetPoint());
			if (pMaster)
			{
				delete pMaster;
			}
			pPoint->Unlock();
		}
		CScriptSuperPointerMgr::GetInstance()->ReturnPointer(pPoint);
		pState->ClearFunParam();
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::GetData(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}

		std::string strFlag = pState->PopCharVarFormStack();
		auto pData = CScriptDataMgr::GetInstance()->GetData(strFlag.c_str());
		pState->ClearFunParam();
		pState->PushClassPointToStack(pData);
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::ReleaseData(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		__int64 index = pState->PopClassPointFormStack();
		CScriptBasePointer* pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(index);
		if (pPoint)
		{
			auto pData = dynamic_cast<CScriptData*>(pPoint->GetPoint());
			CScriptDataMgr::GetInstance()->ReleaseData(pData);
		}

		pState->ClearFunParam();
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::GetVal4Data(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		std::string strFlag = pState->PopCharVarFormStack();
		auto pData = CScriptDataMgr::GetInstance()->GetData(strFlag.c_str());
		int nParmNum = pState->GetParamNum();
		CScriptSubData* pArray = nullptr;
		if (nParmNum >= 2)
		{
			std::string strName = pState->PopCharVarFormStack();
			auto it = pData->m_mapChild.find(strName);
			if (it != pData->m_mapChild.end())
			{
				pArray = it->second;
			}
		}
		if (pArray)
		{
			for (int i = 2; i < nParmNum; i++)
			{
				std::string strName = pState->PopCharVarFormStack();
				auto it = pArray->m_mapChild.find(strName);
				if (it != pArray->m_mapChild.end())
				{
					pArray = it->second;
				}
				else
				{
					pArray = nullptr;
					break;
				}
			}
		}

		pState->ClearFunParam();
		if (pArray)
			pState->PushVarToStack(pArray->m_var);
		else
			pState->PushEmptyVarToStack();
		CScriptDataMgr::GetInstance()->ReleaseData(pData);
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::SetVal2Data(CScriptVirtualMachine* pMachine, CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		std::string strFlag = pState->PopCharVarFormStack();
		auto pData = CScriptDataMgr::GetInstance()->GetData(strFlag.c_str());
		int nParmNum = pState->GetParamNum();
		CScriptSubData* pArray = nullptr;
		if (nParmNum > 2)
		{
			std::string strName = pState->PopCharVarFormStack();
			pArray = pData->m_mapChild[strName];
			if (pArray == nullptr)
			{
				pArray = new CScriptSubData;
				pData->m_mapChild[strName] = pArray;
			}
		}

		for (int i = 2; i < nParmNum - 1; i++)
		{
			std::string strName = pState->PopCharVarFormStack();
			auto& subMap = pArray->m_mapChild;
			pArray = subMap[strName];
			if (pArray == nullptr)
			{
				pArray = new CScriptSubData;
				subMap[strName] = pArray;
			}
		}

		pArray->m_var = pState->PopVarFormStack();
		CScriptDataMgr::GetInstance()->ReleaseData(pData);
		pState->ClearFunParam();
		return ECALLBACK_FINISH;
	}

}