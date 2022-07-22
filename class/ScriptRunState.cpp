#include "scriptcommon.h"
#include "ScriptRunState.h"
#include "ScriptCallBackFunion.h"
#include "ScriptDebugPrint.h"
#include "EScriptSentenceType.h"
#include "EScriptVariableType.h"
#include "ScriptExeCodeData.h"
#include "ScriptVarAssignmentMgr.h"
#include "ScriptCodeLoader.h"
#include <chrono>

namespace zlscript
{
	CScriptCallState::~CScriptCallState()
	{
		CScriptVarTypeMgr::GetInstance()->ReleaseVar(m_varReturn);
	}
	__int64 CScriptCallState::GetMasterID()
	{
		return m_pMaster ? m_pMaster->GetId() : 0;
	}

	CBaseVar* CScriptCallState::GetVarFormStack(unsigned int index)
	{
		if (index < m_stackRegister.nIndex)
		{
			return m_stackRegister.m_vData[index];
		}

		return nullptr;
	}
	int CScriptCallState::GetParamNum()
	{
		return m_stackRegister.nIndex;
	}

	CBaseVar* CScriptCallState::GetResult()
	{
		return m_varReturn;
	}

	void CScriptCallState::SetResult(CBaseVar* pVal)
	{
		SCRIPTVAR_COPY_VAR(m_varReturn, pVal);
	}


	__int64 CScriptRunState::s_nIDSum = 0;
	CScriptRunState::CScriptRunState()
	{
		m_pMachine = nullptr;
		//_intUntilTime = 0;
		m_WatingTime = 0;

		//nNetworkID = 0;
		nCallEventIndex = -1;
		m_CallReturnId = 0;
		nRunCount = 0;
		//m_pBattle = nullptr;
		//m_pShape = nullptr;
		if (s_nIDSum == 0)
		{
			s_nIDSum++;//跳过0
		}
		m_id = s_nIDSum++;

		SetEnd(false);
	}
	CScriptRunState::~CScriptRunState()
	{
		SCRIPTVAR_RELEASE(m_varReturn);
		ClearStack();
		ClearExecBlock();
	}
	void CScriptRunState::SetWatingTime(unsigned int nTime)
	{
		auto nowTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
		m_WatingTime = nowTime.time_since_epoch().count();
		m_WatingTime += nTime;
	}

	//void CScriptRunState::CopyToStack(CScriptStack* pStack, int nNum)
	//{
	//	if (pStack == nullptr)
	//	{
	//		return;
	//	}
	//	if (m_BlockStack.size() > 0)
	//	{
	//		CScriptExecBlock* pBlock = m_BlockStack.top();
	//		if (pBlock)
	//		{
	//			for (unsigned int i = pBlock->m_stackRegister.nIndex - nNum; i < pBlock->m_stackRegister.nIndex; i++)
	//			{
	//				StackVarInfo var;
	//				STACK_GET_INDEX(pBlock->m_stackRegister, var,i);
	//				pStack->push(var);
	//			}
	//		}
	//	}
	//}



	void CScriptRunState::CopyFromStack(tagScriptVarStack& stack)
	{
		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				STACK_MOVE_ALL_BACK(pBlock->registerStack, stack, 0);
			}
		}
		while (stack.nIndex > 0)
		{
			CBaseVar* pVar = nullptr;
			STACK_POP(stack, pVar);
			SCRIPTVAR_RELEASE(pVar);
		}
	}
	//__int64 CScriptRunState::GetIntParamVar(int index)
	//{
	//	index = index + CurCallFunParamNum;
	//	if (m_varRegister.size() > index)
	//	{
	//		return 0;
	//	}
	//	StackVarInfo & var = m_varRegister.GetVal(index);
	//	__int64 nReturn = 0;
	//	switch (var.cType)
	//	{
	//	case EScriptVal_Int:
	//		nReturn = var.Int64;
	//		break;
	//	case EScriptVal_Double:
	//		nReturn = (__int64)(var.Double + 0.5f);
	//		break;
	//	case EScriptVal_String:
	//		if (var.pStr)
	//		{
	//			nReturn = atoi(var.pStr);
	//
	//			if (var.cExtend > 0)
	//			{
	//				SAFE_DELETE_ARRAY(var.pStr);
	//			}
	//		}
	//		else
	//		{
	//			nReturn = 0;
	//		}
	//		break;
	//	}
	//	return nReturn;
	//}
	//double CScriptRunState::GetDoubleParamVar(int index)
	//{
	//	index = index + CurCallFunParamNum;
	//	if (m_varRegister.size() > index)
	//	{
	//		return 0;
	//	}
	//	StackVarInfo& var = m_varRegister.GetVal(index);
	//	double qReturn = 0;
	//	switch (var.cType)
	//	{
	//	case EScriptVal_Int:
	//		qReturn = (double)var.Int64;
	//		break;
	//	case EScriptVal_Double:
	//		qReturn = var.Double;
	//		break;
	//	case EScriptVal_String:
	//		if (var.pStr)
	//		{
	//			qReturn = atof(var.pStr);
	//
	//			if (var.cExtend > 0)
	//			{
	//				SAFE_DELETE_ARRAY(var.pStr);
	//			}
	//		}
	//		else
	//		{
	//			qReturn = 0;
	//		}
	//		break;
	//	}
	//
	//	m_varRegister.pop();
	//	return qReturn;
	//}
	//char* CScriptRunState::GetCharParamVar(int index)
	//{
	//	static char strbuff[2048];
	//	memset(strbuff,0,sizeof(strbuff));
	//
	//	index = index + CurCallFunParamNum;
	//	if (m_varRegister.size() > index)
	//	{
	//		return strbuff;
	//	}
	//
	//	StackVarInfo& var = m_varRegister.GetVal(index);
	//	char* pReturn = nullptr;
	//	switch (var.cType)
	//	{
	//	case EScriptVal_Int:
	//		ltoa(var.Int64,strbuff,10);
	//		break;
	//	case EScriptVal_Double:
	//		gcvt(var.Double,8,strbuff);
	//		break;
	//	case EScriptVal_String:
	//		strcpy(strbuff,var.pStr);
	//
	//		if (var.cExtend > 0)
	//		{
	//			SAFE_DELETE_ARRAY(var.pStr);
	//		}
	//		break;
	//	}
	//	return strbuff;
	//}
	void CScriptRunState::ClearFunParam()
	{
		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				//pBlock->ClearFunParam();
			}
		}
	}

	void CScriptRunState::ClearStack()
	{
		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				STACK_CLEAR(pBlock->registerStack);
				//pBlock->ClearStack();
			}
		}
	}
	void CScriptRunState::ClearExecBlock(bool bPrint)
	{
		//if (!m_BlockStack.empty())
		//	zlscript::CScriptDebugPrintMgr::GetInstance()->Print("Script Run Error");
		while (!m_BlockStack.empty())
		{

			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (bPrint)
			{
				//zlscript::CScriptDebugPrintMgr::GetInstance()->Print(pBlock->GetCurSourceWords().c_str());
			}
			SAFE_DELETE(pBlock);
			m_BlockStack.pop();
		}
	}
	void CScriptRunState::PrintExecBlock()
	{
		//if (!m_BlockStack.empty())
		//	zlscript::CScriptDebugPrintMgr::GetInstance()->Print("Script Run Error");
		//while (!m_BlockStack.empty())
		//{

		//	CScriptExecBlock* pBlock = m_BlockStack.top();
		//	if (bPrint)
		//	{
		//		zlscript::CScriptDebugPrintMgr::GetInstance()->Print(pBlock->GetCurSourceWords().c_str());
		//	}
		//	SAFE_DELETE(pBlock);
		//	m_BlockStack.pop();
		//}
	}
	void CScriptRunState::ClearAll()
	{
		ClearExecBlock();
		ClearStack();
		ClearFunParam();
		nCallEventIndex = -1;
		m_CallReturnId = 0;
		FunName = "";
		nRunCount = 0;
		m_WatingTime = 0;
	}
	int CScriptRunState::CallFun_CallBack(CScriptVirtualMachine* pMachine, int FunIndex, CScriptCallState* pCallState)
	{
		int nReturn = ECALLBACK_FINISH;
		C_CallBackScriptFunion pFun = CScriptCallBackFunion::GetFun(FunIndex);
		if (pFun)
		{
			nReturn = pFun(pMachine, pCallState);
		}
		else
		{
			SCRIPT_PRINT("script", "Error: script callfun: %d", FunIndex);
			return ECALLBACK_ERROR;
		}
		return nReturn;
	}
	int CScriptRunState::CallFun_Script(CScriptVirtualMachine* pMachine, int FunIndex, tagScriptVarStack& ParmStack, int nParmNum, bool bIsBreak)
	{
		int nReturn = ECALLBACK_FINISH;
		{
			CExeCodeData* pCodeData = CScriptCodeLoader::GetInstance()->GetCode(FunIndex);
			if (pCodeData)
			{
				CScriptExecBlock* pBlock =
					new CScriptExecBlock(pCodeData, this);

				if (pBlock)
				{
					//提取参数
					int nOffset = ParmStack.nIndex - nParmNum;
					if (nOffset < 0)
						nOffset = 0;

					for (int i = 0; i < nParmNum && i < pCodeData->nFunParamNums; i++)
					{
						CBaseVar* pVar1 = pBlock->loaclVarStack.m_vData[i];
						CBaseVar* pVar2 = ParmStack.m_vData[i+ nOffset];
						AssignVar(pVar1, pVar2);
						//CBaseVar* pVar = nullptr;
						//STACK_GET_INDEX(ParmStack, pVar, (i + nOffset));
						//STACK_PUSH_MOVE(pBlock->registerStack, pVar);
					}


					m_BlockStack.push(pBlock);
					nReturn = ECALLBACK_NEXTCONTINUE;
				}
				else
				{
					nReturn = ECALLBACK_ERROR;
				}
			}
			else
			{
				SCRIPT_PRINT("script", "Error: script scriptfun: %d", FunIndex);
				return ECALLBACK_ERROR;
			}
		}
		return nReturn;
	}
	int CScriptRunState::CallFun_Script(CScriptVirtualMachine* pMachine, int FunIndex, std::vector<const CBaseVar*>& vParams)
	{
		int nReturn = ECALLBACK_FINISH;
		{
			CExeCodeData* pCodeData = CScriptCodeLoader::GetInstance()->GetCode(FunIndex);
			if (pCodeData)
			{
				CScriptExecBlock* pBlock =
					new CScriptExecBlock(pCodeData, this);

				if (pBlock)
				{
					//提取参数

					for (unsigned int i = 0; i < vParams.size() && i < pCodeData->nFunParamNums; i++)
					{
						CBaseVar* pVar1 = pBlock->loaclVarStack.m_vData[i];
						const CBaseVar* pVar2 = vParams[i];
						AssignVar(pVar1, pVar2);
					}


					m_BlockStack.push(pBlock);
					nReturn = ECALLBACK_NEXTCONTINUE;
				}
				else
				{
					nReturn = ECALLBACK_ERROR;
				}
			}
			else
			{
				SCRIPT_PRINT("script", "Error: script scriptfun: %d", FunIndex);
				return ECALLBACK_ERROR;
			}
		}
		return nReturn;
	}
	int CScriptRunState::CallFunImmediately(CScriptVirtualMachine* pMachine, const char* pFunName, tagScriptVarStack& ParmStack)
	{
		int nReturn = ERunTime_Complete;

		CExeCodeData* pCode = CScriptCodeLoader::GetInstance()->GetCode(pFunName);
		if (pCode)
		{
			CScriptExecBlock* pBlock =
				new CScriptExecBlock(pCode, this);

			if (pBlock)
			{
				//注入参数
				for (int i = 0; i < ParmStack.nSize && i < pCode->nFunParamNums; i++)
				{
					CBaseVar* pVar1 = pBlock->loaclVarStack.m_vData[i];
					CBaseVar* pVar2 = ParmStack.m_vData[i];
					AssignVar(pVar1, pVar2);
					//CBaseVar* pVar = nullptr;
					//STACK_GET_INDEX(ParmStack, pVar, (i + nOffset));
					//STACK_PUSH_MOVE(pBlock->registerStack, pVar);
				}
				//STACK_MOVE_ALL_BACK(pBlock->registerStack, ParmStack, 0);

				m_BlockStack.push(pBlock);
				nReturn = Exec(0xffffffff, pMachine);
			}
			else
			{
				nReturn = ERunTime_Error;
			}
		}

		return nReturn;
	}
	unsigned int CScriptRunState::Exec(unsigned int unTimeRes, CScriptVirtualMachine* pMachine)
	{
		if (unTimeRes == 0)
		{
			unTimeRes = 1;
		}
		while (!m_BlockStack.empty() && unTimeRes > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock == nullptr)
			{
				m_BlockStack.pop();
				continue;
			}
			bool bNeedNext = false;

			unsigned int nResult = pBlock->ExecBlock(pMachine);
			switch (nResult)
			{
			case CScriptExecBlock::ERESULT_END:
			{
				m_BlockStack.pop();
				if (m_BlockStack.empty())
				{
					STACK_POP(pBlock->registerStack, m_varReturn);
					SAFE_DELETE(pBlock);
					return ERunTime_Complete;
				}
				else
				{
					if (pBlock->GetFunType() != -1)
					{
						auto pCurBlock = m_BlockStack.top();
						if (pCurBlock)
						{
							CBaseVar* pTemp = nullptr;
							STACK_POP(pBlock->registerStack, pTemp);
							STACK_PUSH_MOVE(pCurBlock->registerStack, pTemp)
						}
					}
					SAFE_DELETE(pBlock);
				}
				break;
			}
			case CScriptExecBlock::ERESULT_ERROR:
			{
				//2020/4/23 执行错误后不再退出整个堆栈，而是只终结当前代码块并且返回默认值
					//ClearExecBlock(true);
				SCRIPT_PRINT("script", "Error script 执行错误: %s", FunName.c_str());

				SAFE_DELETE(pBlock);
				m_BlockStack.pop();
				break;
			}
			case CScriptExecBlock::ERESULT_WAITING:
			{
				return ERunTime_Waiting;
			}
			case CScriptExecBlock::ERESULT_NEXTCONTINUE:
			{

			}
			break;
			}


			unTimeRes--;
			nRunCount++;

		}

		if (m_BlockStack.empty())
		{
			return ERunTime_Complete;
		}
		//if (nRunCount > 50000)
		//{
		//	ClearExecBlock();
		//	PrintDebug("script","Error script 执行步骤过长: %s",FunName.c_str());
		//	return ERunTime_Error;
		//}
		return ERunTime_Continue;
	}


	//int CScriptRunState::CallFun(CScriptVirtualMachine* pMachine, CScriptExecBlock* pCurBlock, int nType, int FunIndex, int nParmNum, bool bIsBreak)
	//{
	//	int nReturn = ECALLBACK_FINISH;

	//	switch (nType)
	//	{
	//	case 0:
	//	{
	//		//pCurBlock->SetCallFunParamNum(nParmNum);
	//		C_CallBackScriptFunion pFun = CScriptCallBackFunion::GetFun(FunIndex);
	//		if (pFun)
	//		{
	//			CACHE_NEW(CScriptCallState, pCallState, this);
	//			if (pCurBlock)
	//			{
	//				unsigned int nBegin = pCurBlock->m_stackRegister.nIndex - nParmNum;
	//				STACK_MOVE_ALL_BACK(pCallState->m_stackRegister, pCurBlock->m_stackRegister, nBegin);
	//				//for (int i = 0; i < nParmNum; i++)
	//				//{
	//				//	StackVarInfo var;
	//				//	STACK_GET_INDEX(pCurBlock->m_stackRegister, var, (pCurBlock->m_stackRegister.nIndex - nParmNum+i));
	//				//	STACK_PUSH(pCallState->m_stackRegister, var);
	//				//}
	//				////pBlock->SetCallFunParamNum(nParmNum);
	//				//for (int i = 0; i < nParmNum; i++)
	//				//{
	//				//	//pCurBlock->PopVar();
	//				//	STACK_POP(pCurBlock->m_stackRegister);
	//				//}
	//			}
	//			nReturn = pFun(pMachine, pCallState);
	//			CACHE_DELETE(pCallState);
	//		}
	//		else
	//		{
	//			SCRIPT_PRINT("script", "Error: script callfun: %d", FunIndex);
	//			return ECALLBACK_ERROR;
	//		}
	//	}
	//	break;
	//	case 1:
	//	{
	//		tagCodeData* pCodeData = CScriptCodeLoader::GetInstance()->GetCode(FunIndex);
	//		if (pCodeData)
	//		{
	//			CScriptExecBlock* pBlock =
	//				new CScriptExecBlock(pCodeData, this);

	//			if (pBlock)
	//			{
	//				//提取参数
	//				if (pCurBlock)
	//				{

	//					for (int i = 0; i < nParmNum; i++)
	//					{
	//						StackVarInfo var;
	//						STACK_GET_INDEX(pCurBlock->m_stackRegister, var, pCurBlock->m_stackRegister.nIndex - nParmNum + i);
	//						STACK_PUSH(pBlock->m_stackRegister, var);
	//					}
	//					//pBlock->SetCallFunParamNum(nParmNum);
	//					for (int i = 0; i < nParmNum; i++)
	//					{
	//						//pCurBlock->PopVar();
	//						STACK_POP(pCurBlock->m_stackRegister);
	//					}
	//				}

	//				m_BlockStack.push(pBlock);
	//				nReturn = ECALLBACK_CALLSCRIPTFUN;
	//			}
	//			else
	//			{
	//				nReturn = ECALLBACK_ERROR;
	//			}
	//		}
	//		else
	//		{
	//			SCRIPT_PRINT("script", "Error: script scriptfun: %d", FunIndex);
	//			return ECALLBACK_ERROR;
	//		}
	//	}
	//	}

	//	return nReturn;
	//}

	//int CScriptRunState::CallFun(CScriptVirtualMachine* pMachine, int nType, int FunIndex, tagScriptVarStack& ParmStack, bool bIsBreak)
	//{
	//	int nReturn = ECALLBACK_FINISH;

	//	switch (nType)
	//	{
	//	case 0:
	//	{
	//		C_CallBackScriptFunion pFun = CScriptCallBackFunion::GetFun(FunIndex);
	//		if (pFun)
	//		{
	//			CACHE_NEW(CScriptCallState, pCallState, this);
	//			//注入参数

	//			for (int i = 0; i < ParmStack.nIndex; i++)
	//			{
	//				StackVarInfo var;
	//				STACK_GET_INDEX(ParmStack, var, i);
	//				STACK_PUSH(pCallState->m_stackRegister, var);
	//			}

	//			nReturn = pFun(pMachine, pCallState);
	//			CACHE_DELETE(pCallState);
	//		}
	//		else
	//		{
	//			SCRIPT_PRINT("script", "Error: script callfun: %d", FunIndex);
	//			return ECALLBACK_ERROR;
	//		}
	//	}
	//	break;
	//	case 1:
	//	{
	//		tagCodeData* pCodeData = CScriptCodeLoader::GetInstance()->GetCode(FunIndex);
	//		if (pCodeData)
	//		{
	//			CScriptExecBlock* pBlock =
	//				new CScriptExecBlock(pCodeData, this);

	//			if (pBlock)
	//			{
	//				//注入参数
	//				for (int i = 0; i < ParmStack.nIndex; i++)
	//				{
	//					StackVarInfo var;
	//					STACK_GET_INDEX(ParmStack, var, i);
	//					STACK_PUSH(pBlock->m_stackRegister, var);
	//				}
	//				if (bIsBreak && m_BlockStack.size() > 0)
	//				{
	//					CScriptExecBlock* pOldBlock = m_BlockStack.top();
	//					if (pOldBlock->GetFunType() == EICODE_FUN_CAN_BREAK)
	//					{
	//						m_BlockStack.pop();
	//					}
	//				}

	//				m_BlockStack.push(pBlock);
	//				nReturn = ECALLBACK_CALLSCRIPTFUN;
	//			}
	//			else
	//			{
	//				nReturn = ECALLBACK_ERROR;
	//			}
	//		}
	//		else
	//		{
	//			SCRIPT_PRINT("script", "Error: script scriptfun: %d", FunIndex);
	//			return ECALLBACK_ERROR;
	//		}
	//	}
	//	}

	//	return nReturn;
	//}
	
int CScriptRunState::CallFun(CScriptVirtualMachine* pMachine, const char* pFunName, tagScriptVarStack& ParmStack, bool bIsBreak)
	{
		int nReturn = ECALLBACK_FINISH;
		int nFunIndex = CScriptCodeLoader::GetInstance()->GetCodeIndex(pFunName);
		if (nFunIndex >= 0)
		{
			nReturn = CallFun_Script(pMachine, nFunIndex, ParmStack, ParmStack.nIndex, bIsBreak);
		}
		else
		{
			C_CallBackScriptFunion pFun = CScriptCallBackFunion::GetFun(pFunName);
			if (pFun)
			{
				CACHE_NEW(CScriptCallState, pCallState, this);
				//注入参数
				STACK_MOVE_ALL_BACK(pCallState->m_stackRegister, ParmStack, 0);

				nReturn = pFun(pMachine, pCallState);
				CACHE_DELETE(pCallState);
			}
			else
			{
				SCRIPT_PRINT("script", "Error: script callfun: %s", pFunName);
				return ECALLBACK_ERROR;
			}
		}

		return nReturn;
	}
}