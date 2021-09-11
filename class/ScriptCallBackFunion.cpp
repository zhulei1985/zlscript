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

		//RegisterFun("StopScript", (C_CallBackScriptFunion)StopScript);
		//RegisterFun("stopscript", (C_CallBackScriptFunion)StopScript);

		//RegisterFun("CheckScriptRun", (C_CallBackScriptFunion)CheckScriptRun);
		//RegisterFun("checkscriptrun", (C_CallBackScriptFunion)CheckScriptRun);

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


		//RegisterFun("NewArray", (C_CallBackScriptFunion)NewArray);
		//RegisterFun("newarray", (C_CallBackScriptFunion)NewArray);

		//RegisterFun("ReleaseArray", (C_CallBackScriptFunion)ReleaseArray);
		//RegisterFun("releasearray", (C_CallBackScriptFunion)ReleaseArray);

		RegisterFun("InitData", (C_CallBackScriptFunion)InitData);
		RegisterFun("initdata", (C_CallBackScriptFunion)InitData);

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

	int CScriptCallBackFunion::SetProcessID(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}

		if (pMachine == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		short nVal = pState->GetIntVarFormStack(0);

		g_Process_ID = nVal;

		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::RunScriptToChannel(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
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
		//CScriptStack vRetrunVars;
		
		int nIsWaiting = pState->GetIntVarFormStack(0);//是否等待调用函数完成
		std::string name = pState->GetStringVarFormStack(1);

		//nParmNum -= 2;
		tagScriptVarStack scriptParm;
		STACK_MOVE_ALL_BACK(scriptParm, pState->m_stackRegister, 2);

		pMachine->RunTo(name, scriptParm, nIsWaiting>0? pState->GetMasterID():0, 0);
		
		if (nIsWaiting)
		{
			return ECALLBACK_WAITING;
		}
		else
			return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::GetMsTime(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
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

		pState->SetResult(duration.count());
		return ECALLBACK_FINISH;
	}
	int CScriptCallBackFunion::print(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}

		std::string str = pState->GetStringVarFormStack(0);
		printf("%s\n", str.c_str());

		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::Printf(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{

		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		std::string strReturn;
		char ch[2] = { 0,0 };
		std::string str = pState->GetStringVarFormStack(0);
		int nIndex = 1;
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
						strReturn += pState->GetStringVarFormStack(nIndex++);
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

		pState->SetResult(strReturn.c_str());
		return ECALLBACK_FINISH;
	}


	int CScriptCallBackFunion::getrand(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}


		int nval = 0x7fffffff;
		if (pState->GetParamNum() >= 1)
		{
			nval = (int)pState->GetIntVarFormStack(0);
			if (nval <= 0)
			{
				nval = 1;
			}
		}
		int nrand = rand() % nval;

		pState->SetResult((__int64)nrand);
		return ECALLBACK_FINISH;
	}
	int CScriptCallBackFunion::wait(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		if (pState->m_pMaster == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		if (pState->GetParamNum() >= 1)
		{
			pState->m_pMaster->SetWatingTime(pState->GetIntVarFormStack(0));
		}
		return ECALLBACK_NEXTCONTINUE;
	}

	int CScriptCallBackFunion::TransRadian(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		__int64 nAngle = pState->GetIntVarFormStack(0);
		double fRadian = nAngle / 180.f * 3.1415926f;

		pState->SetResult(fRadian);
		return ECALLBACK_NEXTCONTINUE;
	}

	int CScriptCallBackFunion::CheckClassPoint(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		PointVarInfo PointVal = pState->GetClassPointFormStack(0);
		CScriptBasePointer* pPoint = PointVal.pPoint;

		if (pPoint && pPoint->GetPoint())
		{
			pState->SetResult((__int64)1);
		}
		else
		{
			pState->SetResult((__int64)0);
		}
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::SetEventTrigger(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		int nParmNum = pState->GetParamNum();
		PointVarInfo pointVal = pState->GetClassPointFormStack(0);
		std::string strEvent = pState->GetStringVarFormStack(1);
		std::string strFlag = pState->GetStringVarFormStack(2);
		std::string strScript = pState->GetStringVarFormStack(3);
		__int64 nClassPoint = 0;
		if (pointVal.pPoint)
		{
			nClassPoint = pointVal.pPoint->GetID();
		}

		tagScriptVarStack vParmVars;
		STACK_COPY_BEGIN(vParmVars, pState->m_stackRegister, 4);

		CScriptTriggerMgr::GetInstance()->SetEventTrigger(strEvent, nClassPoint, strFlag, pMachine->GetEventIndex(), strScript, vParmVars);

		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::TriggerEvent(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		PointVarInfo pointVal = pState->GetClassPointFormStack(0);
		std::string strEvent = pState->GetStringVarFormStack(1);
		__int64 nClassPoint = 0;
		if (pointVal.pPoint)
		{
			nClassPoint = pointVal.pPoint->GetID();
		}
		CScriptTriggerMgr::GetInstance()->TriggerEvent(strEvent, nClassPoint);

		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::RemoveEventTrigger(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		PointVarInfo pointVal = pState->GetClassPointFormStack(0);
		std::string strEvent = pState->GetStringVarFormStack(1);
		std::string strFlag = pState->GetStringVarFormStack(2);
		__int64 nClassPoint = 0;
		if (pointVal.pPoint)
		{
			nClassPoint = pointVal.pPoint->GetID();
		}
		CScriptTriggerMgr::GetInstance()->RemoveTrigger(strEvent, nClassPoint, strFlag);

		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::InitData(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}

		std::string strFlag = pState->GetStringVarFormStack(0);
		auto pData = CScriptDataMgr::GetInstance()->GetData(strFlag.c_str());

		pState->SetClassPointResult(pData);
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::ReleaseData(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		PointVarInfo pointVal = pState->GetClassPointFormStack(0);
		CScriptBasePointer* pPoint = pointVal.pPoint;
		if (pPoint)
		{
			auto pData = dynamic_cast<CScriptData*>(pPoint->GetPoint());
		}

		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::GetVal4Data(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		int nParmNum = pState->GetParamNum();
		std::string strFlag = pState->GetStringVarFormStack(0);
		auto pData = CScriptDataMgr::GetInstance()->GetData(strFlag.c_str());
		CScriptSubData* pArray = nullptr;
		if (nParmNum >= 2)
		{
			std::string strName = pState->GetStringVarFormStack(1);
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
				std::string strName = pState->GetStringVarFormStack(i);
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

		if (pArray)
			pState->SetResult(pArray->m_var);
		//else
			//pState->PushEmptyVarToStack();
		CScriptDataMgr::GetInstance()->ReleaseData(pData);
		return ECALLBACK_FINISH;
	}

	int CScriptCallBackFunion::SetVal2Data(CScriptVirtualMachine* pMachine, CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		int nParmNum = pState->GetParamNum();
		std::string strFlag = pState->GetStringVarFormStack(0);
		auto pData = CScriptDataMgr::GetInstance()->GetData(strFlag.c_str());
		CScriptSubData* pArray = nullptr;
		if (nParmNum > 2)
		{
			std::string strName = pState->GetStringVarFormStack(1);
			pArray = pData->m_mapChild[strName];
			if (pArray == nullptr)
			{
				pArray = new CScriptSubData;
				pData->m_mapChild[strName] = pArray;
			}
		}

		for (int i = 2; i < nParmNum - 1; i++)
		{
			std::string strName = pState->GetStringVarFormStack(i);
			auto& subMap = pArray->m_mapChild;
			pArray = subMap[strName];
			if (pArray == nullptr)
			{
				pArray = new CScriptSubData;
				subMap[strName] = pArray;
			}
		}

		pArray->m_var = pState->GetVarFormStack(nParmNum - 1);
		CScriptDataMgr::GetInstance()->ReleaseData(pData);

		return ECALLBACK_FINISH;
	}

}