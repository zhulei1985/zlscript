#include "ScriptRunState.h"
#include "ScriptCallBackFunion.h"
#include "ScriptDebugPrint.h"
#include "EScriptSentenceType.h"
#include "EScriptVariableType.h"
#include <chrono>

namespace zlscript
{
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
		ClearStack();
		ClearExecBlock();
	}
	void CScriptRunState::SetWatingTime(unsigned int nTime)
	{
		auto nowTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
		m_WatingTime = nowTime.time_since_epoch().count();
		m_WatingTime += nTime;
	}

	bool CScriptRunState::PushEmptyVarToStack()
	{
		StackVarInfo var;
		var.cType = EScriptVal_None;
		var.Int64 = 0;

		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				pBlock->PushVar(var);
			}
		}
		return true;
	}
	bool CScriptRunState::PushVarToStack(int nVal)
	{
		StackVarInfo var;
		var.cType = EScriptVal_Int;
		var.Int64 = nVal;

		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				pBlock->PushVar(var);
			}
		}

		return true;
	}

	bool CScriptRunState::PushVarToStack(__int64 nVal)
	{
		StackVarInfo var;
		var.cType = EScriptVal_Int;
		var.Int64 = nVal;

		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				pBlock->PushVar(var);
			}
		}

		return true;
	}
	bool CScriptRunState::PushVarToStack(double Double)
	{
		StackVarInfo var;
		var.cType = EScriptVal_Double;
		var.Double = Double;

		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				pBlock->PushVar(var);
			}
		}

		return true;
	}
	bool CScriptRunState::PushVarToStack(const char* pstr)
	{
		StackVarInfo var;
		var.cType = EScriptVal_String;
		var.Int64 = StackVarInfo::s_strPool.NewString(pstr);

		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				pBlock->PushVar(var);
			}
		}

		return true;
	}
	bool CScriptRunState::PushClassPointToStack(__int64 nIndex)
	{
		StackVarInfo var;
		var.cType = EScriptVal_ClassPointIndex;
		var.Int64 = nIndex;

		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				pBlock->PushVar(var);
			}
		}

		return true;
	}

	bool CScriptRunState::PushVarToStack(StackVarInfo& Val)
	{
		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				pBlock->PushVar(Val);
			}
		}

		return true;
	}



	__int64 CScriptRunState::PopIntVarFormStack()
	{
		StackVarInfo var;
		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				var = pBlock->PopVar();
			}
		}
		__int64 nReturn = 0;
		switch (var.cType)
		{
		case EScriptVal_None:
			nReturn = 0;
			break;
		case EScriptVal_Int:
			nReturn = var.Int64;
			break;
		case EScriptVal_Double:
			nReturn = (__int64)(var.Double + 0.5f);
			break;
		case EScriptVal_String:
		{
			const char* pStr = StackVarInfo::s_strPool.GetString(var.Int64);
			if (pStr)
			{
				nReturn = atoi(pStr);

			}
			else
			{
				nReturn = 0;
			}
		}

		break;
		}
		return nReturn;
	}
	double CScriptRunState::PopDoubleVarFormStack()
	{
		StackVarInfo var;
		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				var = pBlock->PopVar();
			}
		}
		double qReturn = 0;
		switch (var.cType)
		{
		case EScriptVal_None:
			qReturn = 0;
			break;
		case EScriptVal_Int:
			qReturn = (double)var.Int64;
			break;
		case EScriptVal_Double:
			qReturn = var.Double;
			break;
		case EScriptVal_String:
			const char* pStr = StackVarInfo::s_strPool.GetString(var.Int64);
			if (pStr)
			{
				qReturn = atof(pStr);

			}
			else
			{
				qReturn = 0;
			}
			break;
		}

		return qReturn;
	}
	char* CScriptRunState::PopCharVarFormStack()
	{
		memset(strbuff, 0, sizeof(strbuff));

		StackVarInfo var;
		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				var = pBlock->PopVar();
			}
		}
		switch (var.cType)
		{
		case EScriptVal_None:
			break;
		case EScriptVal_Int:
			//ltoa(var.Int64,strbuff,10);
			sprintf(strbuff, "%lld", var.Int64);
			break;
		case EScriptVal_Double:
			sprintf(strbuff, "%f", var.Double);
			//gcvt(var.Double,8,strbuff);
			break;
		case EScriptVal_String:
		{
			const char* pStr = StackVarInfo::s_strPool.GetString(var.Int64);
			if (pStr)
				strcpy(strbuff, pStr);
		}


		break;
		}

		return strbuff;
	}
	__int64 CScriptRunState::PopClassPointFormStack()
	{
		StackVarInfo var;
		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				var = pBlock->PopVar();
			}
		}

		__int64 nReturn = 0;
		switch (var.cType)
		{
		case EScriptVal_ClassPointIndex:
			nReturn = var.Int64;
			break;
		}

		return nReturn;
	}
	StackVarInfo CScriptRunState::PopVarFormStack()
	{
		StackVarInfo var;
		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				var = pBlock->PopVar();
			}
		}
		else
		{
			var = m_varReturn;
		}
		return var;
	}
	int CScriptRunState::GetParamNum()
	{
		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				return pBlock->GetParamNum();
			}
		}
		return 0;
	}
	void CScriptRunState::CopyToStack(CScriptStack* pStack, int nNum)
	{
		if (pStack == nullptr)
		{
			return;
		}
		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				for (unsigned int i = pBlock->GetVarSize() - nNum; i < pBlock->GetVarSize(); i++)
				{
					auto pVar = pBlock->GetVar(i);
					if (pVar)
						pStack->push(*pVar);
				}
			}
		}
	}

	void CScriptRunState::CopyFromStack(CScriptStack* pStack)
	{
		if (pStack == nullptr)
		{
			return;
		}
		std::vector<StackVarInfo> vVars;
		while (!pStack->empty())
		{
			vVars.push_back(pStack->top());
			pStack->pop();
		}
		if (m_BlockStack.size() > 0)
		{
			CScriptExecBlock* pBlock = m_BlockStack.top();
			if (pBlock)
			{
				for (unsigned int i = 0; i < vVars.size(); i++)
				{
					pBlock->PushVar(vVars[i]);
				}
			}
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
				pBlock->ClearFunParam();
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
				pBlock->ClearStack();
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
				zlscript::CScriptDebugPrintMgr::GetInstance()->Print(pBlock->GetCurSourceWords().c_str());
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
	int CScriptRunState::CallFunImmediately(CScriptVirtualMachine* pMachine, const char* pFunName, CScriptStack& ParmStack)
	{
		int nReturn = ERunTime_Complete;

		CScriptCodeLoader::tagCodeData* pCode = CScriptCodeLoader::GetInstance()->GetCode(pFunName);
		if (pCode)
		{
			CScriptExecBlock* pBlock =
				new CScriptExecBlock(pCode, this);

			if (pBlock)
			{
				//注入参数
				for (int i = 0; i < ParmStack.size(); i++)
				{
					StackVarInfo* pVar = ParmStack.GetVal(i);
					if (pVar)
					{
						pBlock->PushVar(*pVar);
					}
				}



				m_BlockStack.push(pBlock);
				nReturn = Exec(999999, pMachine);
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

			while (unTimeRes > 0)
			{
				bool bBreak = false;
				unsigned int nResult = pBlock->ExecBlock(pMachine);
				switch (nResult)
				{
				case CScriptExecBlock::ERESULT_END:
				{
					m_BlockStack.pop();
					if (m_BlockStack.empty())
					{
						if (pBlock->GetVarSize())
						{
							auto pVar = pBlock->GetVar(pBlock->GetVarSize() - 1);
							if (pVar)
								m_varReturn = *pVar;
						}
						SAFE_DELETE(pBlock);
						return ERunTime_Complete;
					}
					else
					{
						auto pCurBlock = m_BlockStack.top();
						if (pCurBlock)
						{
							for (unsigned int i = 0; i < pBlock->GetVarSize(); i++)
							{
								auto pVar = pBlock->GetVar(i);
								if (pVar)
									pCurBlock->PushVar(*pVar);
								else
								{
									StackVarInfo var;
									pCurBlock->PushVar(var);
								}
							}
						}
						SAFE_DELETE(pBlock);
					}
					bBreak = true;
					break;
				}
				case CScriptExecBlock::ERESULT_ERROR:
				{
					//2020/4/23 执行错误后不再退出整个堆栈，而是只终结当前代码块并且返回默认值
						//ClearExecBlock(true);
					SCRIPT_PRINT("script", "Error script 执行错误: %s", FunName.c_str());
					//退出本代码块，返回默认返回值
					m_BlockStack.pop();
					if (pBlock->GetDefaultReturnType() != EScriptVal_None)
					{
						if (m_BlockStack.empty())
						{
							StackVarInfo var;
							m_varReturn = var;
							return ERunTime_Complete;
						}
						else
						{
							auto pCurBlock = m_BlockStack.top();
							if (pCurBlock)
							{
								StackVarInfo var;
								pCurBlock->PushVar(var);
							}

						}
					}
					SAFE_DELETE(pBlock);
					bBreak = true;
					break;
				}
				case CScriptExecBlock::ERESULT_WAITING:
				{
					return ERunTime_Waiting;
				}
				case CScriptExecBlock::ERESULT_CALLSCRIPTFUN:
				{
					//检查是否有被打断的函数
					bBreak = true;
				}
				break;
				case CScriptExecBlock::ERESULT_NEXTCONTINUE:
				{
					bNeedNext = true;
				}
				break;
				}
				if (bNeedNext)
				{
					break;
				}
				if (bBreak)
				{
					break;
				}

				unTimeRes--;
				nRunCount++;
			}
			if (bNeedNext)
			{
				break;
			}
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


	int CScriptRunState::CallFun(CScriptVirtualMachine* pMachine, CScriptExecBlock* pCurBlock, int nType, int FunIndex, int nParmNum, bool bIsBreak)
	{
		int nReturn = ECALLBACK_FINISH;

		switch (nType)
		{
		case 0:
		{
			pCurBlock->SetCallFunParamNum(nParmNum);
			C_CallBackScriptFunion pFun = CScriptCallBackFunion::GetFun(FunIndex);
			if (pFun)
			{
				nReturn = pFun(pMachine, this);
			}
			else
			{
				SCRIPT_PRINT("script", "Error: script callfun: %d", FunIndex);
				return ECALLBACK_ERROR;
			}
		}
		break;
		case 1:
		{
			CScriptCodeLoader::tagCodeData* pCodeData = CScriptCodeLoader::GetInstance()->GetCode(FunIndex);
			if (pCodeData)
			{
				CScriptExecBlock* pBlock =
					new CScriptExecBlock(pCodeData, this);

				if (pBlock)
				{
					//提取参数
					if (pCurBlock)
					{

						for (int i = 0; i < nParmNum; i++)
						{
							auto pVar = pCurBlock->GetVar(pCurBlock->GetVarSize() - nParmNum + i);
							if (pVar)
								pBlock->PushVar(*pVar);
							else
							{
								StackVarInfo var;
								pCurBlock->PushVar(var);
							}
						}
						pBlock->SetCallFunParamNum(nParmNum);
						for (int i = 0; i < nParmNum; i++)
						{
							pCurBlock->PopVar();
						}
					}

					m_BlockStack.push(pBlock);
					nReturn = ECALLBACK_CALLSCRIPTFUN;
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
		}

		return nReturn;
	}

	int CScriptRunState::CallFun(CScriptVirtualMachine* pMachine, int nType, int FunIndex, CScriptStack& ParmStack, bool bIsBreak)
	{
		int nReturn = ECALLBACK_FINISH;

		switch (nType)
		{
		case 0:
		{
			//注入参数
			if (m_BlockStack.size() > 0)
			{
				CScriptExecBlock* pBlock = m_BlockStack.top();
				if (pBlock)
				{
					for (int i = 0; i < ParmStack.size(); i++)
					{
						StackVarInfo* pVar = ParmStack.GetVal(i);
						if (pVar)
						{
							pBlock->PushVar(*pVar);
						}
					}
				}
			}
			C_CallBackScriptFunion pFun = CScriptCallBackFunion::GetFun(FunIndex);
			if (pFun)
			{
				nReturn = pFun(pMachine, this);
			}
			else
			{
				SCRIPT_PRINT("script", "Error: script callfun: %d", FunIndex);
				return ECALLBACK_ERROR;
			}
		}
		break;
		case 1:
		{
			CScriptCodeLoader::tagCodeData* pCodeData = CScriptCodeLoader::GetInstance()->GetCode(FunIndex);
			if (pCodeData)
			{
				CScriptExecBlock* pBlock =
					new CScriptExecBlock(pCodeData, this);

				if (pBlock)
				{
					//注入参数
					for (int i = 0; i < ParmStack.size(); i++)
					{
						StackVarInfo* pVar = ParmStack.GetVal(i);
						if (pVar)
						{
							pBlock->PushVar(*pVar);
						}
					}
					if (bIsBreak && m_BlockStack.size() > 0)
					{
						CScriptExecBlock* pOldBlock = m_BlockStack.top();
						if (pOldBlock->GetFunType() == EICODE_FUN_CAN_BREAK)
						{
							m_BlockStack.pop();
						}
					}

					m_BlockStack.push(pBlock);
					nReturn = ECALLBACK_CALLSCRIPTFUN;
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
		}

		return nReturn;
	}
	int CScriptRunState::CallFun(CScriptVirtualMachine* pMachine, const char* pFunName, CScriptStack& ParmStack, bool bIsBreak)
	{
		int nReturn = ECALLBACK_FINISH;

		CScriptCodeLoader::tagCodeData* pCode = CScriptCodeLoader::GetInstance()->GetCode(pFunName);
		if (pCode)
		{
			CScriptExecBlock* pBlock =
				new CScriptExecBlock(pCode, this);

			if (pBlock)
			{
				//注入参数
				for (int i = 0; i < ParmStack.size(); i++)
				{
					StackVarInfo* pVar = ParmStack.GetVal(i);
					if (pVar)
					{
						pBlock->PushVar(*pVar);
					}
				}
				if (bIsBreak && m_BlockStack.size() > 0)
				{
					CScriptExecBlock* pOldBlock = m_BlockStack.top();
					if (pOldBlock->GetFunType() == EICODE_FUN_CAN_BREAK)
					{
						m_BlockStack.pop();
					}
				}

				m_BlockStack.push(pBlock);
				nReturn = ECALLBACK_CALLSCRIPTFUN;
			}
			else
			{
				nReturn = ECALLBACK_ERROR;
			}
		}
		else
		{
			//注入参数
			if (m_BlockStack.size() > 0)
			{
				CScriptExecBlock* pBlock = m_BlockStack.top();
				if (pBlock)
				{
					for (int i = 0; i < ParmStack.size(); i++)
					{
						StackVarInfo* pVar = ParmStack.GetVal(i);
						if (pVar)
						{
							pBlock->PushVar(*pVar);
						}
					}
				}
			}

			C_CallBackScriptFunion pFun = CScriptCallBackFunion::GetFun(pFunName);
			if (pFun)
			{
				nReturn = pFun(pMachine, this);
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