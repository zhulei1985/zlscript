#pragma once
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
#include "ScriptCodeStyle.h"
#include "ScriptExecBlock.h"
#include "ScriptRunState.h"
#include "ScriptClassMgr.h"
namespace zlscript
{

	//int CAddExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = nVal1 + nVal2;
	//	}
	//	break;
	//	case EScriptVal_Double:
	//	{
	//		double dVal1 = GetFloat_StackVar(&pBlock->m_register[R_A]);
	//		double dVal2 = GetFloat_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = dVal1 + dVal2;
	//	}
	//	break;
	//	case EScriptVal_String:
	//	{
	//		std::string strVal1 = GetString_StackVar(&pBlock->m_register[R_A]);
	//		std::string strVal2 = GetString_StackVar(&rightVar);
	//		strVal1 = strVal1 + strVal2;
	//		pBlock->m_register[cResultRegister] = strVal1.c_str();
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CSumExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = nVal1 - nVal2;;
	//	}
	//	break;
	//	case EScriptVal_Double:
	//	{
	//		double dVal1 = GetFloat_StackVar(&pBlock->m_register[R_A]);
	//		double dVal2 = GetFloat_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = dVal1 - dVal2;
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CMulExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = nVal1 * nVal2;
	//	}
	//	break;
	//	case EScriptVal_Double:
	//	{
	//		double dVal1 = GetFloat_StackVar(&pBlock->m_register[R_A]);
	//		double dVal2 = GetFloat_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = dVal1 * dVal2;
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CDivExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		if (nVal2 == 0)
	//		{
	//			pBlock->m_register[cResultRegister] = (__int64)0xffffffff;
	//		}
	//		else
	//		{
	//			pBlock->m_register[cResultRegister] = nVal1 / nVal2;
	//		}
	//	}
	//	break;
	//	case EScriptVal_Double:
	//	{
	//		double dVal1 = GetFloat_StackVar(&pBlock->m_register[R_A]);
	//		double dVal2 = GetFloat_StackVar(&rightVar);
	//		if (dVal1 <= 0.00000001f && dVal1 >= -0.00000001f)
	//		{
	//			pBlock->m_register[cResultRegister] = (double)1.7976931348623158e+308;
	//		}
	//		else
	//		{
	//			pBlock->m_register[cResultRegister] = dVal1 / dVal2;
	//		}
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CModExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);

	//	__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//	__int64 nVal2 = GetInt_StackVar(&rightVar);
	//	if (nVal2 == 0)
	//	{
	//		pBlock->m_register[cResultRegister] = (__int64)0;
	//	}
	//	else
	//	{
	//		pBlock->m_register[cResultRegister] = nVal1 % nVal2;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CMinusExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		pBlock->m_register[cResultRegister] = -pBlock->m_register[R_A].Int64;
	//	}
	//	break;
	//	case EScriptVal_Double:
	//	{
	//		pBlock->m_register[cResultRegister] = -pBlock->m_register[R_A].Double;
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CCmpEqualExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(nVal1 == nVal2 ? 1 : 0);
	//	}
	//	break;
	//	case EScriptVal_Double:
	//	{
	//		double dVal1 = GetFloat_StackVar(&pBlock->m_register[R_A]);
	//		double dVal2 = GetFloat_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(dVal1 == dVal2 ? 1 : 0);
	//	}
	//	break;
	//	case EScriptVal_String:
	//	{
	//		std::string strVal1 = GetString_StackVar(&pBlock->m_register[R_A]);
	//		std::string strVal2 = GetString_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(strVal1 == strVal2 ? 1 : 0);
	//	}
	//	break;
	//	case EScriptVal_ClassPoint:
	//	{
	//		__int64 nVal1 = GetPointIndex_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetPointIndex_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(nVal1 == nVal2 ? 1 : 0);
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CCmpNotEqualExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(nVal1 != nVal2 ? 1 : 0);
	//	}
	//	break;
	//	case EScriptVal_Double:
	//	{
	//		double dVal1 = GetFloat_StackVar(&pBlock->m_register[R_A]);
	//		double dVal2 = GetFloat_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(dVal1 != dVal2 ? 1 : 0);
	//	}
	//	break;
	//	case EScriptVal_String:
	//	{
	//		std::string strVal1 = GetString_StackVar(&pBlock->m_register[R_A]);
	//		std::string strVal2 = GetString_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(strVal1 != strVal2 ? 1 : 0);
	//	}
	//	break;
	//	case EScriptVal_ClassPoint:
	//	{
	//		__int64 nVal1 = GetPointIndex_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetPointIndex_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(nVal1 != nVal2 ? 1 : 0);
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CCmpBigExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(nVal1 > nVal2 ? 1 : 0);
	//	}
	//	break;
	//	case EScriptVal_Double:
	//	{
	//		double dVal1 = GetFloat_StackVar(&pBlock->m_register[R_A]);
	//		double dVal2 = GetFloat_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(dVal1 > dVal2 ? 1 : 0);
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CCmpBigAndEqualExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(nVal1 >= nVal2 ? 1 : 0);
	//	}
	//	break;
	//	case EScriptVal_Double:
	//	{
	//		double dVal1 = GetFloat_StackVar(&pBlock->m_register[R_A]);
	//		double dVal2 = GetFloat_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(dVal1 >= dVal2 ? 1 : 0);
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CCmpLessExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(nVal1 < nVal2 ? 1 : 0);
	//	}
	//	break;
	//	case EScriptVal_Double:
	//	{
	//		double dVal1 = GetFloat_StackVar(&pBlock->m_register[R_A]);
	//		double dVal2 = GetFloat_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(dVal1 < dVal2 ? 1 : 0);
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CCmpLessAndEqualExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(nVal1 <= nVal2 ? 1 : 0);
	//	}
	//	break;
	//	case EScriptVal_Double:
	//	{
	//		double dVal1 = GetFloat_StackVar(&pBlock->m_register[R_A]);
	//		double dVal2 = GetFloat_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = (__int64)(dVal1 <= dVal2 ? 1 : 0);
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CBitAndExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = nVal1 & nVal2;
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CBitOrExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = nVal1 | nVal2;
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CBitXorExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo rightVar = pBlock->GetVal(cType, dwPos);
	//	switch (pBlock->m_register[R_A].cType)
	//	{
	//	case EScriptVal_Int:
	//	{
	//		__int64 nVal1 = GetInt_StackVar(&pBlock->m_register[R_A]);
	//		__int64 nVal2 = GetInt_StackVar(&rightVar);
	//		pBlock->m_register[cResultRegister] = nVal1 ^ nVal2;
	//	}
	//	break;
	//	default:
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CPushExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo var = pBlock->GetVal(cType, dwPos);
	//	ScriptVector_PushVar(pBlock->m_stackRegister, &var);
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CPopExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	StackVarInfo var = ScriptStack_GetVar(pBlock->m_stackRegister);
	//	if (pBlock->SetVal(cType, dwPos, var) == false)
	//	{
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CLoadExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	pBlock->m_register[cResultRegister] = pBlock->GetVal(cType, dwPos);
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CMoveExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	if (pBlock->SetVal(cType, dwPos, pBlock->m_register[cResultRegister]) == false)
	//	{
	//		return CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CGetClassParamExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	int nResult = CScriptExecBlock::ERESULT_CONTINUE;
	//	if (pBlock->m_register[cClassRegIndex].cType == EScriptVal_ClassPoint)
	//	{
	//		//PointVarInfo pointVal = GetPointIndex_StackVar(&m_register[code.cSign]);
	//		CScriptBasePointer* pPoint = pBlock->m_register[cClassRegIndex].pPoint;
	//		if (pPoint)
	//		{
	//			pPoint->Lock();
	//			auto pAttribute = pPoint->GetAttribute(dwPos);
	//			if (pAttribute)
	//			{
	//				pBlock->m_register[cResultRegister] = pAttribute->ToScriptVal();
	//			}
	//			else
	//			{
	//				//TODO 报错
	//				nResult = CScriptExecBlock::ERESULT_ERROR;
	//			}
	//			pPoint->Unlock();
	//		}
	//	}
	//	else
	//	{
	//		//TODO 报错
	//		nResult = CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return nResult;
	//}
	//int CSetClassParamExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	int nResult = CScriptExecBlock::ERESULT_CONTINUE;
	//	if (pBlock->m_register[cClassRegIndex].cType == EScriptVal_ClassPoint)
	//	{
	//		//PointVarInfo pointVal = GetPointIndex_StackVar(&m_register[code.cSign]);
	//		CScriptBasePointer* pPoint = pBlock->m_register[cClassRegIndex].pPoint;
	//		if (pPoint)
	//		{
	//			pPoint->Lock();
	//			auto pAttribute = pPoint->GetAttribute(dwPos);
	//			if (pAttribute)
	//			{
	//				pAttribute->SetVal(pBlock->m_register[cVarRegister]);
	//			}
	//			else
	//			{
	//				//TODO 报错
	//				nResult = CScriptExecBlock::ERESULT_ERROR;
	//			}
	//			pPoint->Unlock();
	//		}
	//	}
	//	else
	//	{
	//		//TODO 报错
	//		nResult = CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return nResult;
	//}
	//int CCallBackExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	int nResult = CScriptExecBlock::ERESULT_CONTINUE;
	//	if (pBlock->m_stackRegister.size() >= cParmSize)
	//	{
	//		CACHE_NEW(CScriptCallState, pCallState, pBlock->m_pMaster);
	//		if (pCallState)
	//		{
	//			//设置参数
	//			for (unsigned int i = pBlock->m_stackRegister.size() - cParmSize; i < pBlock->m_stackRegister.size(); i++)
	//			{
	//				auto pVar = pBlock->m_stackRegister.GetVal(i);
	//				if (pVar)
	//					pCallState->PushVarToStack(*pVar);
	//			}
	//			for (unsigned char i = 0; i < cParmSize; i++)
	//			{
	//				pBlock->m_stackRegister.pop();
	//			}
	//			//运行回调函数
	//			switch (pBlock->m_pMaster->CallFun_CallBack(pBlock->m_pMaster->m_pMachine, unFunIndex, pCallState))
	//			{
	//			case ECALLBACK_ERROR:
	//				nResult = CScriptExecBlock::ERESULT_ERROR;
	//				break;
	//			case ECALLBACK_WAITING:
	//				nResult = CScriptExecBlock::ERESULT_WAITING;
	//				break;
	//			case ECALLBACK_CALLSCRIPTFUN:

	//				nResult = CScriptExecBlock::ERESULT_CALLSCRIPTFUN;
	//				break;
	//			case ECALLBACK_NEXTCONTINUE:
	//				nResult = CScriptExecBlock::ERESULT_NEXTCONTINUE;
	//				break;
	//			}
	//			//执行完将结果放入寄存器
	//			pBlock->m_register[cResultRegister] = pCallState->GetResult();
	//		}
	//		CACHE_DELETE(pCallState);
	//	}
	//	else
	//	{
	//		//TODO 报错
	//		nResult = CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return nResult;
	//}
	//int CCallScriptExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	int nResult = CScriptExecBlock::ERESULT_CONTINUE;
	//	if (pBlock->m_stackRegister.size() >= cParmSize)
	//	{
	//		//运行回调函数
	//		switch (pBlock->m_pMaster->CallFun_Script(pBlock->m_pMaster->m_pMachine, unFunIndex, pBlock->m_stackRegister, cParmSize, cResultRegister))
	//		{
	//		case ECALLBACK_ERROR:
	//			nResult = CScriptExecBlock::ERESULT_ERROR;
	//			break;
	//		case ECALLBACK_WAITING:
	//			nResult = CScriptExecBlock::ERESULT_WAITING;
	//			break;
	//		case ECALLBACK_CALLSCRIPTFUN:

	//			nResult = CScriptExecBlock::ERESULT_CALLSCRIPTFUN;
	//			break;
	//		case ECALLBACK_NEXTCONTINUE:
	//			nResult = CScriptExecBlock::ERESULT_NEXTCONTINUE;
	//			break;
	//		}
	//		for (unsigned char i = 0; i < cParmSize; i++)
	//		{
	//			pBlock->m_stackRegister.pop();
	//		}
	//	}
	//	else
	//	{
	//		nResult = CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = this->m_pNext;
	//	}
	//	return nResult;
	//}
	//int CJumpExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	//{
	//	if (pNextPoint)
	//	{
	//		if (this->pJumpCode)
	//			*pNextPoint = this->pJumpCode;
	//		else
	//			*pNextPoint = m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CJumpTrueExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	//{
	//	if (pBlock->CheckRegisterTrue(cVarRegister))
	//	{
	//		if (pNextPoint)
	//		{
	//			*pNextPoint = this->pJumpCode;
	//		}
	//	}
	//	else
	//	{
	//		if (pNextPoint)
	//		{
	//			*pNextPoint = this->m_pNext;
	//		}
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CJumpFalseExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	//{
	//	if (!pBlock->CheckRegisterTrue(cVarRegister))
	//	{
	//		if (pNextPoint)
	//		{
	//			*pNextPoint = this->pJumpCode;
	//		}
	//	}
	//	else
	//	{
	//		if (pNextPoint)
	//		{
	//			*pNextPoint = this->m_pNext;
	//		}
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CReturnExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	//{
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = nullptr;
	//	}
	//	pBlock->m_varReturnVar = pBlock->m_register[cVarRegister];
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CCallClassFunExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	//{
	//	int nResult = CScriptExecBlock::ERESULT_CONTINUE;
	//	PointVarInfo pointVal = GetPoint_StackVar(&pBlock->m_register[cClassRegIndex]);

	//	if (pBlock->m_stackRegister.size() >= cParmSize)
	//	{
	//		CACHE_NEW(CScriptCallState, pCallState, pBlock->m_pMaster);
	//		if (pCallState)
	//		{
	//			//设置参数
	//			for (unsigned int i = pBlock->m_stackRegister.size() - cParmSize; i < pBlock->m_stackRegister.size(); i++)
	//			{
	//				auto pVar = pBlock->m_stackRegister.GetVal(i);
	//				if (pVar)
	//					pCallState->PushVarToStack(*pVar);
	//			}
	//			for (unsigned char i = 0; i < cParmSize; i++)
	//			{
	//				pBlock->m_stackRegister.pop();
	//			}

	//			CScriptBasePointer* pPoint = pointVal.pPoint;
	//			if (pPoint)
	//			{
	//				switch (pPoint->RunFun(dwPos, pCallState))
	//				{
	//				case ECALLBACK_ERROR:
	//					nResult = CScriptExecBlock::ERESULT_ERROR;
	//					break;
	//				case ECALLBACK_WAITING:
	//					nResult = CScriptExecBlock::ERESULT_WAITING;
	//					break;
	//				case ECALLBACK_CALLSCRIPTFUN:

	//					nResult = CScriptExecBlock::ERESULT_CALLSCRIPTFUN;
	//					break;
	//				case ECALLBACK_NEXTCONTINUE:
	//					nResult = CScriptExecBlock::ERESULT_NEXTCONTINUE;
	//					break;
	//				}
	//				if (pNextPoint)
	//				{
	//					*pNextPoint = m_pNext;
	//				}
	//				//执行完将结果放入寄存器
	//				pBlock->m_register[cResultRegister] = pCallState->GetResult();
	//			}
	//			else
	//			{
	//				nResult = CScriptExecBlock::ERESULT_ERROR;
	//			}
	//		}
	//		CACHE_DELETE(pCallState);
	//	}
	//	else
	//	{
	//		nResult = CScriptExecBlock::ERESULT_ERROR;
	//	}
	//	return nResult;
	//}
	//int CNewClassExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	//{
	//	CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(dwClassIndex);
	//	if (pMgr)
	//	{
	//		auto pNewPoint = pMgr->New(SCRIPT_NO_USED_AUTO_RELEASE);
	//		pBlock->m_register[cResultRegister] = pNewPoint;
	//	}
	//	else
	//	{
	//		pBlock->m_register[cResultRegister] = (CScriptPointInterface*)nullptr;
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CReleaseClassExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	//{
	//	PointVarInfo pointVal = GetPoint_StackVar(&pBlock->m_register[cVarRegister]);
	//	CScriptBasePointer* pPoint = pointVal.pPoint;
	//	if (pPoint)
	//	{
	//		CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(pPoint->GetType());
	//		if (pMgr)
	//		{
	//			pMgr->Release(pPoint);
	//		}
	//	}
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CBreakExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	//{
	//	if (pNextPoint)
	//	{
	//		if (this->pJumpCode)
	//			*pNextPoint = this->pJumpCode;
	//		else
	//			*pNextPoint = m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}

	//int CContinueExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	//{
	//	if (pNextPoint)
	//	{
	//		if (this->pJumpCode)
	//			*pNextPoint = this->pJumpCode;
	//		else
	//			*pNextPoint = m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//int CSignExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	//{
	//	if (pNextPoint)
	//	{
	//		*pNextPoint = m_pNext;
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}

	CExeCodeMgr CExeCodeMgr::s_Instance;
	CExeCodeMgr::CExeCodeMgr()
	{
	}
	CExeCodeMgr::~CExeCodeMgr()
	{
	}
	void CExeCodeMgr::Clear()
	{
		for (auto it = m_listExeCode.begin(); it != m_listExeCode.end(); it++)
		{
			CBaseExeCode* pCode = *it;
			if (pCode)
			{
				delete pCode;
			}
		}
		m_listExeCode.clear();
	}


}