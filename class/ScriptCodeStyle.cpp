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

	int CAddExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];
		//pBlock->GetVal(rightVar, cType, dwPos);
		switch (leftVar.cType)
		{
		case EScriptVal_Int:
		{
			__int64 nVal2 = 0;
			pBlock->GetVal(nVal2, cType, dwPos);
			//resultVar = leftVar.Int64 + nVal2;
			SCRIPTVAR_SET_INT(resultVar, leftVar.Int64 + nVal2);
		}
		break;
		case EScriptVal_Double:
		{
			double dVal2 = 0;
			pBlock->GetVal(dVal2, cType, dwPos);
			SCRIPTVAR_SET_FLOAT(resultVar , leftVar.Double + dVal2);
		}
		break;
		case EScriptVal_String:
		{
			std::string strVal1;
			SCRIPTVAR_GET_STRING(leftVar, strVal1);
			std::string strVal2;
			pBlock->GetVal(strVal2, cType, dwPos);
			strVal1 = strVal1 + strVal2;
			SCRIPTVAR_SET_STRING(resultVar, strVal1);
		}
		break;
		default:
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CAddExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "ADD\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CSumExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];

		if (leftVar.cType == EScriptVal_Int)
		{
			__int64 nVal2 = 0;
			pBlock->GetVal(nVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar,leftVar.Int64-nVal2);
		}
		else if (leftVar.cType == EScriptVal_Double)
		{
			double dVal2 = 0;
			pBlock->GetVal(dVal2, cType, dwPos);
			SCRIPTVAR_SET_FLOAT(resultVar, leftVar.Double - dVal2);
		}
		else
		{
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CSumExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "Sum\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CMulExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];
		if (leftVar.cType == EScriptVal_Int)
		{
			__int64 nVal2 = 0;
			pBlock->GetVal(nVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, leftVar.Int64 * nVal2);
		}
		else if (leftVar.cType == EScriptVal_Double)
		{
			double dVal2 = 0;
			pBlock->GetVal(dVal2, cType, dwPos);
			SCRIPTVAR_SET_FLOAT(resultVar ,leftVar.Double * dVal2);
		}
		else{
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CMulExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "Mul\tResultRegister:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CDivExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];
		
		if (leftVar.cType == EScriptVal_Int)
		{
			__int64 nVal2 = 0; GetInt_StackVar(&pBlock->m_register[R_D]);
			pBlock->GetVal(nVal2, cType, dwPos);
			if (nVal2 == 0)
			{
				SCRIPTVAR_SET_INT(resultVar, (__int64)0xffffffffffffffff);
			}
			else
			{
				SCRIPTVAR_SET_INT(resultVar, leftVar.Int64 / nVal2);
			}
		}
		else if (leftVar.cType == EScriptVal_Double)
		{
			double dVal2 = 0;
			pBlock->GetVal(dVal2, cType, dwPos);
			if (dVal2 <= 0.00000001f && dVal2 >= -0.00000001f)
			{
				SCRIPTVAR_SET_FLOAT(resultVar, (double)1.7976931348623158e+308);
			}
			else
			{
				SCRIPTVAR_SET_FLOAT(resultVar, leftVar.Double / dVal2);
			}
		}
		else{
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CDivExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "Div\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CModExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];
		__int64 nVal1 = 0;
		SCRIPTVAR_GET_INT(leftVar,nVal1);
		__int64 nVal2 = 0;
		pBlock->GetVal(nVal2, cType, dwPos);
		if (nVal2 == 0)
		{
			resultVar = (__int64)0;
		}
		else
		{
			resultVar = nVal1 % nVal2;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CModExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "Mod\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CMinusExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];
		if (leftVar.cType == EScriptVal_Int)
		{
			SCRIPTVAR_SET_INT(resultVar, -leftVar.Int64);
		}
		if (leftVar.cType == EScriptVal_Double)
		{
			SCRIPTVAR_SET_FLOAT(resultVar, -leftVar.Double);
		}
		else{
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CMinusExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "Minus\tRegisterIndex(extend):%d", (int)cResultRegister);
		return strbuff;
	}
	int CCmpEqualExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];
		switch (leftVar.cType)
		{
		case EScriptVal_Int:
		{
			__int64 nVal2 = 0;
			pBlock->GetVal(nVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(leftVar.Int64 == nVal2 ? 1 : 0));
		}
		break;
		case EScriptVal_Double:
		{
			double dVal2 = 0;
			pBlock->GetVal(dVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(leftVar.Double == dVal2 ? 1 : 0));
		}
		break;
		case EScriptVal_String:
		{
			std::string strVal1;
			SCRIPTVAR_GET_STRING(leftVar, strVal1);
			std::string strVal2;
			pBlock->GetVal(strVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(strVal1 == strVal2 ? 1 : 0));
		}
		break;
		case EScriptVal_ClassPoint:
		{
			StackVarInfo rightVar;
			pBlock->GetVal(rightVar, cType, dwPos);
			if (leftVar.pPoint == nullptr && (rightVar.cType == EScriptVal_None || rightVar.cType == EScriptVal_Void))
			{
				SCRIPTVAR_SET_INT(resultVar, (__int64)(1));
			}
			else if (rightVar.cType == EScriptVal_ClassPoint && leftVar.pPoint == rightVar.pPoint)
			{
				SCRIPTVAR_SET_INT(resultVar, (__int64)(1));
			}
			else
			{
				SCRIPTVAR_SET_INT(resultVar, (__int64)(0));
			}
		}
		break;
		default:
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CCmpEqualExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CMP_EQUAL\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CCmpNotEqualExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];
		switch (leftVar.cType)
		{
		case EScriptVal_Int:
		{
			__int64 nVal2 = 0;
			pBlock->GetVal(nVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(leftVar.Int64 != nVal2 ? 1 : 0));
		}
		break;
		case EScriptVal_Double:
		{
			double dVal2 = 0;
			pBlock->GetVal(dVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(leftVar.Double != dVal2 ? 1 : 0));
		}
		break;
		case EScriptVal_String:
		{
			std::string strVal1;
			SCRIPTVAR_GET_STRING(leftVar, strVal1);
			std::string strVal2;
			pBlock->GetVal(strVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(strVal1 != strVal2 ? 1 : 0));
		}
		break;
		case EScriptVal_ClassPoint:
		{
			StackVarInfo rightVar;
			pBlock->GetVal(rightVar, cType, dwPos);
			if (leftVar.pPoint == nullptr && (rightVar.cType == EScriptVal_None || rightVar.cType == EScriptVal_Void))
			{
				SCRIPTVAR_SET_INT(resultVar, (__int64)(0));
			}
			else if (rightVar.cType == EScriptVal_ClassPoint && leftVar.pPoint == rightVar.pPoint)
			{
				SCRIPTVAR_SET_INT(resultVar, (__int64)(0));
			}
			else
			{
				SCRIPTVAR_SET_INT(resultVar, (__int64)(1));
			}
		}
		break;
		default:
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CCmpNotEqualExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CMP_NOT_EQUAL\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CCmpBigExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];

		if (leftVar.cType==EScriptVal_Int)
		{
			__int64 nVal2 = 0;
			pBlock->GetVal(nVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(leftVar.Int64 > nVal2 ? 1 : 0));
		}
		else if (leftVar.cType == EScriptVal_Double)
		{
			double dVal2 = 0;
			pBlock->GetVal(dVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(leftVar.Double > dVal2 ? 1 : 0));
		}
		else{
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CCmpBigExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CMP_BIG\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CCmpBigAndEqualExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];

		if (leftVar.cType == EScriptVal_Int)
		{
			__int64 nVal2 = 0;
			pBlock->GetVal(nVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(leftVar.Int64 >= nVal2 ? 1 : 0));
		}
		else if (leftVar.cType == EScriptVal_Double)
		{
			double dVal2 = 0;
			pBlock->GetVal(dVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(leftVar.Double >= dVal2 ? 1 : 0));
		}
		else {
			return CScriptExecBlock::ERESULT_ERROR;
		}

		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CCmpBigAndEqualExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CMP_BIG_EQUAL\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CCmpLessExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];

		if (leftVar.cType == EScriptVal_Int)
		{
			__int64 nVal2 = 0;
			pBlock->GetVal(nVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(leftVar.Int64 < nVal2 ? 1 : 0));
		}
		else if (leftVar.cType == EScriptVal_Double)
		{
			double dVal2 = 0;
			pBlock->GetVal(dVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(leftVar.Double < dVal2 ? 1 : 0));
		}
		else {
			return CScriptExecBlock::ERESULT_ERROR;
		}

		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CCmpLessExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CMP_LESS\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CCmpLessAndEqualExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];

		if (leftVar.cType == EScriptVal_Int)
		{
			__int64 nVal2 = 0;
			pBlock->GetVal(nVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(leftVar.Int64 <= nVal2 ? 1 : 0));
		}
		else if (leftVar.cType == EScriptVal_Double)
		{
			double dVal2 = 0;
			pBlock->GetVal(dVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, (__int64)(leftVar.Double <= dVal2 ? 1 : 0));
		}
		else {
			return CScriptExecBlock::ERESULT_ERROR;
		}

		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CCmpLessAndEqualExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CMP_LESS_EQUAL\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CBitAndExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];
		if (leftVar.cType == EScriptVal_Int)
		{
			__int64 nVal2 = 0;
			pBlock->GetVal(nVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, leftVar.Int64 & nVal2);
		}
		else{
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CBitAndExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "BIT_AND\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CBitOrExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];
		if (leftVar.cType == EScriptVal_Int)
		{
			__int64 nVal2 = 0;
			pBlock->GetVal(nVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, leftVar.Int64 | nVal2);
		}
		else {
			return CScriptExecBlock::ERESULT_ERROR;
		}

		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CBitOrExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "BIT_OR\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CBitXorExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& resultVar = pBlock->m_register[cResultRegister];
		StackVarInfo& leftVar = pBlock->m_register[R_A];
		if (leftVar.cType == EScriptVal_Int)
		{
			__int64 nVal2 = 0;
			pBlock->GetVal(nVal2, cType, dwPos);
			SCRIPTVAR_SET_INT(resultVar, leftVar.Int64 ^ nVal2);
		}
		else {
			return CScriptExecBlock::ERESULT_ERROR;
		}

		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CBitXorExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "BIT_XOR\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CPushExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& var = pBlock->m_register[R_D];
		pBlock->GetVal(var, cType, dwPos);
		STACK_PUSH(pBlock->m_stackRegister, var);
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CPushExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "PUSH\tVarType:%d\tpos:%d", (int)cType, (int)dwPos);
		return strbuff;
	}
	int CPopExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		StackVarInfo& var = pBlock->m_register[R_D];
		STACK_GET(pBlock->m_stackRegister, var);
		STACK_POP(pBlock->m_stackRegister);
		if (pBlock->SetVal(cType, dwPos, var) == false)
		{
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CPopExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "POP\tVarType:%d\tpos:%d", (int)cType, (int)dwPos);
		return strbuff;
	}
	int CLoadExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		pBlock->GetVal(pBlock->m_register[cResultRegister],cType, dwPos);
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CLoadExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "LOAD\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cResultRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CMoveExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		if (pBlock->SetVal(cType, dwPos, pBlock->m_register[cVarRegister]) == false)
		{
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CMoveExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "MOVE\tRegisterIndex:%d\tVarType:%d\tpos:%d", (int)cVarRegister, (int)cType, (int)dwPos);
		return strbuff;
	}
	int CGetClassParamExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		int nResult = CScriptExecBlock::ERESULT_CONTINUE;

		StackVarInfo& var = pBlock->m_register[cClassRegIndex];
		if (var.cType == EScriptVal_ClassPoint)
		{
			//PointVarInfo pointVal = GetPointIndex_StackVar(&m_register[code.cSign]);
			CScriptBasePointer* pPoint = var.pPoint;
			if (pPoint)
			{
				pPoint->Lock();
				auto pAttribute = pPoint->GetAttribute(dwPos);
				if (pAttribute)
				{
					var = pAttribute->ToScriptVal();
				}
				else
				{
					//TODO 报错
					nResult = CScriptExecBlock::ERESULT_ERROR;
				}
				pPoint->Unlock();
			}
		}
		else
		{
			//TODO 报错
			nResult = CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return nResult;
	}
	std::string CGetClassParamExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "GetClassParam\tRegisterIndex:%d\tClassReg:%d\tpos:%d", (int)cResultRegister, (int)cClassRegIndex, (int)dwPos);
		return strbuff;
	}
	int CSetClassParamExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		int nResult = CScriptExecBlock::ERESULT_CONTINUE;
		StackVarInfo& var = pBlock->m_register[cClassRegIndex];
		if (var.cType == EScriptVal_ClassPoint)
		{
			//PointVarInfo pointVal = GetPointIndex_StackVar(&m_register[code.cSign]);
			CScriptBasePointer* pPoint = var.pPoint;
			if (pPoint)
			{
				pPoint->Lock();
				auto pAttribute = pPoint->GetAttribute(dwPos);
				if (pAttribute)
				{
					pAttribute->SetVal(var);
				}
				else
				{
					//TODO 报错
					nResult = CScriptExecBlock::ERESULT_ERROR;
				}
				pPoint->Unlock();
			}
		}
		else
		{
			//TODO 报错
			nResult = CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return nResult;
	}
	std::string CSetClassParamExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "SetClassParam\tVarRegister:%d\tClassReg:%d\tpos:%d", (int)cVarRegister, (int)cClassRegIndex, (int)dwPos);
		return strbuff;
	}
	int CCallBackExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		int nResult = CScriptExecBlock::ERESULT_CONTINUE;
		if (pBlock->m_stackRegister.nIndex >= cParmSize)
		{
			CACHE_NEW(CScriptCallState, pCallState, pBlock->m_pMaster);
			if (pCallState)
			{
				//设置参数
				unsigned nBegin = pBlock->m_stackRegister.nIndex - (unsigned int)cParmSize;
				STACK_MOVE_ALL_BACK(pCallState->m_stackRegister, pBlock->m_stackRegister, nBegin);
				//for (unsigned int i = pBlock->m_stackRegister.nIndex - cParmSize; i < pBlock->m_stackRegister.nIndex; i++)
				//{
				//	StackVarInfo var;
				//	STACK_GET_INDEX(pBlock->m_stackRegister, var,i);
				//	STACK_PUSH(pCallState->m_stackRegister, var);
				//}
				//for (unsigned char i = 0; i < cParmSize; i++)
				//{
				//	STACK_POP(pBlock->m_stackRegister);
				//}
				//运行回调函数
				switch (pBlock->m_pMaster->CallFun_CallBack(pBlock->m_pMaster->m_pMachine, unFunIndex, pCallState))
				{
				case ECALLBACK_ERROR:
					nResult = CScriptExecBlock::ERESULT_ERROR;
					break;
				case ECALLBACK_WAITING:
					pBlock->m_cReturnRegisterIndex = cResultRegister;
					nResult = CScriptExecBlock::ERESULT_WAITING;
					break;
				case ECALLBACK_CALLSCRIPTFUN:

					nResult = CScriptExecBlock::ERESULT_CALLSCRIPTFUN;
					break;
				case ECALLBACK_NEXTCONTINUE:
					nResult = CScriptExecBlock::ERESULT_NEXTCONTINUE;
					break; 
				case ECALLBACK_FINISH:
					pBlock->m_register[cResultRegister] = pCallState->GetResult();
					break;
				}
				//执行完将结果放入寄存器
				//pBlock->m_register[cResultRegister] = pCallState->GetResult();
			}
			CACHE_DELETE(pCallState);
		}
		else
		{
			//TODO 报错
			nResult = CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return nResult;
	}
	std::string CCallBackExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CallBack\tResultRegister:%d\tFunIndex:%d\tParmSize:%d", (int)cResultRegister, (int)unFunIndex, (int)cParmSize);
		return strbuff;
	}
	int CCallScriptExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		int nResult = CScriptExecBlock::ERESULT_CONTINUE;
		if (pBlock->m_stackRegister.nIndex >= cParmSize)
		{
			//运行回调函数
			switch (pBlock->m_pMaster->CallFun_Script(pBlock->m_pMaster->m_pMachine, unFunIndex, pBlock->m_stackRegister, cParmSize, cResultRegister))
			{
			case ECALLBACK_ERROR:
				nResult = CScriptExecBlock::ERESULT_ERROR;
				break;
			case ECALLBACK_WAITING:
				nResult = CScriptExecBlock::ERESULT_WAITING;
				break;
			case ECALLBACK_CALLSCRIPTFUN:

				nResult = CScriptExecBlock::ERESULT_CALLSCRIPTFUN;
				break;
			case ECALLBACK_NEXTCONTINUE:
				nResult = CScriptExecBlock::ERESULT_NEXTCONTINUE;
				break;
			}
			for (unsigned char i = 0; i < cParmSize; i++)
			{
				STACK_POP(pBlock->m_stackRegister);
			}
		}
		else
		{
			nResult = CScriptExecBlock::ERESULT_ERROR;
		}
		if (pNextPoint)
		{
			*pNextPoint = this->m_pNext;
		}
		return nResult;
	}
	std::string CCallScriptExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CallScript\tResultRegister:%d\tFunIndex:%d\tParmSize:%d", (int)cResultRegister, (int)unFunIndex, (int)cParmSize);
		return strbuff;
	}
	int CJumpExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint)
	{
		if (pNextPoint)
		{
			if (this->pJumpCode)
				*pNextPoint = this->pJumpCode;
			else
				*pNextPoint = m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CJumpExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "JUMP\tIndex:%d", (int)pJumpCode? pJumpCode->nCodeIndex:0);
		return strbuff;
	}
	int CJumpTrueExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		if (pBlock->CheckRegisterTrue(cVarRegister))
		{
			if (pNextPoint)
			{
				*pNextPoint = this->pJumpCode;
			}
		}
		else
		{
			if (pNextPoint)
			{
				*pNextPoint = this->m_pNext;
			}
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CJumpTrueExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "JUMP_TRUE\tVarRegister:%d\tIndex:%d", (int)cVarRegister,(int)pJumpCode ? pJumpCode->nCodeIndex : 0);
		return strbuff;
	}
	int CJumpFalseExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		if (!pBlock->CheckRegisterTrue(cVarRegister))
		{
			if (pNextPoint)
			{
				*pNextPoint = this->pJumpCode;
			}
		}
		else
		{
			if (pNextPoint)
			{
				*pNextPoint = this->m_pNext;
			}
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CJumpFalseExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "JUMP_FALSE\tVarRegister:%d\tIndex:%d", (int)cVarRegister, (int)pJumpCode ? pJumpCode->nCodeIndex : 0);
		return strbuff;
	}
	int CReturnExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		if (pNextPoint)
		{
			*pNextPoint = nullptr;
		}
		pBlock->m_varReturnVar = pBlock->m_register[cVarRegister];
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CReturnExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "RETURN\tVarRegister:%d", (int)cVarRegister);
		return strbuff;
	}
	int CCallClassFunExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		int nResult = CScriptExecBlock::ERESULT_CONTINUE;
		PointVarInfo pointVal = GetPoint_StackVar(&pBlock->m_register[cClassRegIndex]);

		if (pBlock->m_stackRegister.nIndex >= cParmSize)
		{
			CACHE_NEW(CScriptCallState, pCallState, pBlock->m_pMaster);
			if (pCallState)
			{
				//设置参数
				unsigned nBegin = pBlock->m_stackRegister.nIndex - (unsigned int)cParmSize;
				STACK_MOVE_ALL_BACK(pCallState->m_stackRegister, pBlock->m_stackRegister, nBegin);

				CScriptBasePointer* pPoint = pointVal.pPoint;
				if (pPoint)
				{
					switch (pPoint->RunFun(dwPos, pCallState))
					{
					case ECALLBACK_ERROR:
						nResult = CScriptExecBlock::ERESULT_ERROR;
						break;
					case ECALLBACK_WAITING:
						pBlock->m_cReturnRegisterIndex = cResultRegister;
						nResult = CScriptExecBlock::ERESULT_WAITING;
						break;
					case ECALLBACK_CALLSCRIPTFUN:

						nResult = CScriptExecBlock::ERESULT_CALLSCRIPTFUN;
						break;
					case ECALLBACK_NEXTCONTINUE:
						nResult = CScriptExecBlock::ERESULT_NEXTCONTINUE;
						break;
					case ECALLBACK_FINISH:
						//执行完将结果放入寄存器
						pBlock->m_register[cResultRegister] = pCallState->GetResult();
						break;
					}
					if (pNextPoint)
					{
						*pNextPoint = m_pNext;
					}
					////执行完将结果放入寄存器
					//pBlock->m_register[cResultRegister] = pCallState->GetResult();
					//pBlock->m_cReturnRegisterIndex = cResultRegister; 
				}
				else
				{
					nResult = CScriptExecBlock::ERESULT_ERROR;
				}
			}
			CACHE_DELETE(pCallState);
		}
		else
		{
			nResult = CScriptExecBlock::ERESULT_ERROR;
		}
		return nResult;
	}
	std::string CCallClassFunExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CALL_CLASS_FUN\tResultRegister:%d\tClassReg:%d\tFunIndex:%d\tParmSize:%d", (int)cResultRegister,(int)cClassRegIndex,(int)dwPos,(int)cParmSize);
		return strbuff;
	}
	int CNewClassExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(dwClassIndex);
		if (pMgr)
		{
			auto pNewPoint = pMgr->New(SCRIPT_NO_USED_AUTO_RELEASE);
			pBlock->m_register[cResultRegister] = pNewPoint;
		}
		else
		{
			pBlock->m_register[cResultRegister] = (CScriptPointInterface*)nullptr;
		}
		if (pNextPoint)
		{
			*pNextPoint = m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CNewClassExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "NEW_CLASS\tResultRegister:%d\tClassIndex:%d", (int)cResultRegister, (int)dwClassIndex);
		return strbuff;
	}
	int CReleaseClassExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		PointVarInfo pointVal = GetPoint_StackVar(&pBlock->m_register[cVarRegister]);
		CScriptBasePointer* pPoint = pointVal.pPoint;
		if (pPoint)
		{
			CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(pPoint->GetType());
			if (pMgr)
			{
				pMgr->Release(pPoint);
			}
		}
		if (pNextPoint)
		{
			*pNextPoint = m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CReleaseClassExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "RELEASE_CLASS\tVarRegister:%d", (int)cVarRegister);
		return strbuff;
	}
	int CBreakExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		if (pNextPoint)
		{
			if (this->pJumpCode)
				*pNextPoint = this->pJumpCode;
			else
				*pNextPoint = m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}

	std::string CBreakExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "BREAK\tIndex:%d", (int)pJumpCode ? pJumpCode->nCodeIndex : 0);
		return strbuff;
	}

	int CContinueExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		if (pNextPoint)
		{
			if (this->pJumpCode)
				*pNextPoint = this->pJumpCode;
			else
				*pNextPoint = m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	std::string CContinueExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "CONTINUE\tIndex:%d", (int)pJumpCode ? pJumpCode->nCodeIndex : 0);
		return strbuff;
	}
	int CSignExeCode::Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint)
	{
		if (pNextPoint)
		{
			*pNextPoint = m_pNext;
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}

	std::string CSignExeCode::GetCodeString()
	{
		char strbuff[128] = { 0 };
		sprintf(strbuff, "SIGN");
		return strbuff;
	}

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