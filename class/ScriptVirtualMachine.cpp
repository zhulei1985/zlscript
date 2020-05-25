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

#include "EScriptSentenceType.h"
#include "EScriptVariableType.h"
#include "ScriptCallBackFunion.h"

#include "ScriptVirtualMachine.h"
#include "ScriptEventMgr.h"
#include "ScriptDebugPrint.h"
namespace zlscript
{
	unsigned long CScriptRunState::s_nIDSum = 0;
	CScriptRunState::CScriptRunState()
	{
		m_pMachine = nullptr;
		//_intUntilTime = 0;
		m_WatingTime = 0;

		nNetworkID = 0;
		nCallEventIndex = -1;
		m_CallStateId = 0;
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
	void CScriptRunState::WatingTime(unsigned int nTime)
	{
		if (m_WatingTime > nTime)
		{
			m_WatingTime -= nTime;
		}
		else
		{
			m_WatingTime = 0;
		}
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
		memset(strbuff,0,sizeof(strbuff));

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
				for (unsigned int i = pBlock->GetVarSize()- nNum; i < pBlock->GetVarSize(); i++)
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
		m_CallStateId = 0;
		FunName = "";
		nRunCount = 0;
		m_WatingTime = 0;
	}
	unsigned int CScriptRunState::Exec(unsigned int unTimeRes,CScriptVirtualMachine *pMachine)
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
								auto pVar = pBlock->GetVar(pBlock->GetVarSize()-1);
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
						SCRIPT_PRINT("script","Error script 执行错误: %s",FunName.c_str());
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

	int CScriptRunState::CallFun(CScriptVirtualMachine* pMachine, CScriptExecBlock *pCurBlock, int nType, int FunIndex, int nParmNum, bool bIsBreak)
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
							auto pVar = pCurBlock->GetVar(pCurBlock->GetVarSize()-nParmNum+i);
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
					nReturn = pFun(pMachine,this);
				}
				else
				{
					SCRIPT_PRINT("script","Error: script callfun: %d",FunIndex);
					return ECALLBACK_ERROR;
				}
			}
			break;
		case 1:
			{
				CScriptCodeLoader::tagCodeData *pCodeData = CScriptCodeLoader::GetInstance()->GetCode(FunIndex);
				if (pCodeData)
				{
					CScriptExecBlock *pBlock = 
						new CScriptExecBlock(pCodeData,this);

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
					SCRIPT_PRINT("script","Error: script scriptfun: %d",FunIndex);
					return ECALLBACK_ERROR;
				}
			}
		}

		return nReturn;
	}
	int CScriptRunState::CallFun(CScriptVirtualMachine* pMachine, const char *pFunName, CScriptStack& ParmStack, bool bIsBreak)
	{
		int nReturn = ECALLBACK_FINISH;

		CScriptCodeLoader::tagCodeData *pCode = CScriptCodeLoader::GetInstance()->GetCode(pFunName);
		if (pCode)
		{
			CScriptExecBlock *pBlock = 
				new CScriptExecBlock(pCode,this);

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
				nReturn = pFun(pMachine,this);
			}
			else
			{
				SCRIPT_PRINT("script","Error: script callfun: %s",pFunName);
				return ECALLBACK_ERROR;
			}
		}

		return nReturn;
	}
	//CScriptVirtualMachine  *CScriptVirtualMachine::m_pInstance = nullptr;
	CScriptVirtualMachine::CScriptVirtualMachine()
	{
		init();
	};

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

	void CScriptVirtualMachine::init()
	{
		clear();

		CScriptCodeLoader::GetInstance()->GetGlobalVar(vGlobalNumVar);
		
		m_nEventListIndex = CScriptEventMgr::GetInstance()->AssignID();
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

	#ifdef _DEBUG
		listRunState::iterator it = m_RunStateList.begin();
		for (; it != m_RunStateList.end(); it++)
		{
			if (*it == pState)
			{
				//assert("repeat scriptstate");
			}
		}
	#endif
		m_RunStateList.push_back(pState);

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

#ifdef _DEBUG
		listRunState::iterator it = m_RunStateList.begin();
		for (; it != m_RunStateList.end(); it++)
		{
			if (*it == pState)
			{
				//assert("repeat scriptstate");
			}
		}
#endif
		m_RunStateList.push_back(pState);
		//char cbuff[1024];
		//sprintf(cbuff, "Script State Add 2: %d:", pState);
		//zlscript::CScriptDebugPrintMgr::GetInstance()->Print(cbuff);
		return true;
	}

	unsigned int CScriptVirtualMachine::Exec(unsigned int nDelay,unsigned int unTimeRes)
	{
		std::vector<tagScriptEvent*> vEvent;
		CScriptEventMgr::GetInstance()->GetEventByChannel(m_nEventListIndex, vEvent);
		for (auto it = m_mapEventProcess.begin(); it != m_mapEventProcess.end(); it++)
		{
			CScriptEventMgr::GetInstance()->GetEventByType(it->first, vEvent);
		}

		for (size_t i = 0; i < vEvent.size(); i++)
		{
			tagScriptEvent* pEvent = vEvent[i];
			if (pEvent)
			{
				auto it = m_mapEventProcess.find(pEvent->nEventType);
				if (it != m_mapEventProcess.end())
				{
					it->second(pEvent->nSendID, pEvent->m_Parm);
				}
			}
			CScriptEventMgr::GetInstance()->ReleaseEvent(pEvent);
		}
		vEvent.clear();

		listRunState::iterator it = m_RunStateList.begin();
		for (;it != m_RunStateList.end(); )
		{
			CScriptRunState *pState = *it;
			if (pState)
			{
				if (pState->GetEnd())
				{
					it = m_RunStateList.erase(it);
				}
				else if (pState->m_WatingTime <= 0)
				{
					//pState->m_WatingTime = 0;
					int nResult = pState->Exec(unTimeRes,this);
					switch (nResult)
					{
					case ERunTime_Error:
					case ERunTime_Complete:
						{
						
							if (pState->m_CallStateId != 0)
							{
								if (pState->nNetworkID > 0)
								{
									//网络返回
									CScriptStack vRetrunVars;
									ScriptVector_PushVar(vRetrunVars, (__int64)pState->nCallEventIndex);
									ScriptVector_PushVar(vRetrunVars, (__int64)pState->m_CallStateId);
									ScriptVector_PushVar(vRetrunVars, &pState->PopVarFormStack());
									CScriptEventMgr::GetInstance()->SendEvent(E_SCRIPT_EVENT_NEWTWORK_RETURN, m_nEventListIndex, vRetrunVars, pState->nNetworkID);
								}
								else
								{
									//本地返回
									CScriptStack vRetrunVars;
									ScriptVector_PushVar(vRetrunVars, (__int64)pState->m_CallStateId);
									ScriptVector_PushVar(vRetrunVars, &pState->PopVarFormStack());
									CScriptEventMgr::GetInstance()->SendEvent(E_SCRIPT_EVENT_RETURN,m_nEventListIndex, vRetrunVars, pState->nCallEventIndex);
								}

							}
							//char cbuff[1024];
							//sprintf(cbuff, "Script State Remove : %d:", pState);
							//zlscript::CScriptDebugPrintMgr::GetInstance()->Print(cbuff);
							SAFE_DELETE(pState);
							it = m_RunStateList.erase(it);
						}
						break;
					case ERunTime_Waiting:
						{
							if (m_mapWaiting.find(pState->GetId()) != m_mapWaiting.end())
							{
								CScriptRunState *pWaitingState = m_mapWaiting[pState->GetId()];
								if (pWaitingState != pState)
								{
									//char cbuff[1024];
									//sprintf(cbuff, "Script State Remove Waiting: %d:", pWaitingState);
									//zlscript::CScriptDebugPrintMgr::GetInstance()->Print(cbuff);
									SAFE_DELETE(pWaitingState);
								}
								else
								{
									//assert(0);
								}
							}
							m_mapWaiting[pState->GetId()] = pState;

							//char cbuff[1024];
							//sprintf(cbuff, "Script State Wait : %d:", pState);
							//zlscript::CScriptDebugPrintMgr::GetInstance()->Print(cbuff);
							//pState->m_WatingTime = nowTime;
							it = m_RunStateList.erase(it);
						}
						break;
					default:
						{
							it++;
						}
						break;
					}
				}
				else
				{
					pState->WatingTime(nDelay);
					it++;
				}
			}
			else
			{
				it = m_RunStateList.erase(it);
			}
		}
		//删除
		for (auto itDel = m_listDel.begin(); itDel != m_listDel.end(); itDel++)
		{
			std::map<unsigned long, CScriptRunState*>::iterator itWait = m_mapWaiting.find(*itDel);
			if (itWait != m_mapWaiting.end())
			{
				CScriptRunState* pState = itWait->second;
				if (pState && pState->GetId() == *itDel)
				{
					SAFE_DELETE(pState);
					m_mapWaiting.erase(itWait);
					break;
				}
			}
			listRunState::iterator itRun = m_RunStateList.begin();
			for (; itRun != m_RunStateList.end(); itRun++)
			{
				CScriptRunState* pState = *itRun;
				if (pState && pState->GetId() == *itDel)
				{
					SAFE_DELETE(pState);
					itRun = m_RunStateList.erase(itRun);
					break;
				}
			}

		}
		m_listDel.clear();
		return 0;
	}
	bool CScriptVirtualMachine::HasWaitingScript(unsigned long id)
	{
		//CSingleLock xLock(m_xCtrl, true);
		std::map<unsigned long,CScriptRunState *>::iterator itWait = m_mapWaiting.find(id);
		if (itWait != m_mapWaiting.end())
		{
			return true;
		}
		return false;
	}

	bool CScriptVirtualMachine::RemoveRunState(unsigned long id)
	{
		listRunState::iterator it = m_RunStateList.begin();
		for (; it != m_RunStateList.end(); it++)
		{
			CScriptRunState *pState = *it;
			if (pState->GetId() == id)
			{
				pState->SetEnd(true);
				break;
			}
		}
		//return false;
		m_listDel.push_back(id);
		return true;
	}
	bool CScriptVirtualMachine::RemoveRunStateByShape(int id)
	{
		//listRunState::iterator it = m_RunStateList.begin();
		//for (; it != m_RunStateList.end(); )
		//{
		//	CScriptRunState *pState = *it;
		//	if (pState && pState->GetShape() && pState->GetShape()->GetID() == id)
		//	{
		//		delete pState;
		//		it = m_RunStateList.erase(it);
		//	}
		//	else
		//	{
		//		it++;
		//	}
		//}
		return true;
	}
	bool CScriptVirtualMachine::CheckRun(__int64 id)
	{
		listRunState::iterator it = m_RunStateList.begin();
		for (; it != m_RunStateList.end(); it++)
		{
			CScriptRunState *pState = *it;
			if (pState->GetId() == id)
			{
				return true;
			}
		}
		std::map<unsigned long, CScriptRunState *>::iterator itWait = m_mapWaiting.find(id);
		if (itWait != m_mapWaiting.end())
		{
			return true;
		}
		return false;
	}
	CScriptRunState *CScriptVirtualMachine::GetRunState(unsigned long id)
	{
		listRunState::iterator it = m_RunStateList.begin();
		for (; it != m_RunStateList.end(); it++)
		{
			CScriptRunState *pState = *it;
			if (pState->GetId() == id)
			{
				return pState;
			}
		}
		return nullptr;
	}
	CScriptRunState *CScriptVirtualMachine::PopRunState(unsigned long id)
	{
		listRunState::iterator it = m_RunStateList.begin();
		for (; it != m_RunStateList.end(); it++)
		{
			CScriptRunState *pState = *it;
			if (pState->GetId() == id)
			{
				m_RunStateList.erase(it);
				return pState;
			}
		}
		return nullptr;
	}
	StackVarInfo*CScriptVirtualMachine::GetGlobalVar(unsigned int pos)
	{
		if (pos < vGlobalNumVar.size())
		{
			return &vGlobalNumVar[pos];
		}
		return nullptr;
	}
	void CScriptVirtualMachine::InitEvent(int nEventType, EventProcessFun fun)
	{
		m_mapEventProcess[nEventType] = fun;

		//CScriptEventMgr::GetInstance()->RegisterEvent(nEventType, m_nEventListIndex);
	}
	void CScriptVirtualMachine::EventReturnFun(int nSendID, CScriptStack& ParmInfo)
	{
		__int64 nScriptStateID = ScriptStack_GetInt(ParmInfo);

		std::map<unsigned long, CScriptRunState*>::iterator itWait = m_mapWaiting.find(nScriptStateID);
		if (itWait != m_mapWaiting.end())
		{
			CScriptRunState* pState = itWait->second;
			pState->CopyFromStack(&ParmInfo);
			m_RunStateList.push_back(pState);

			//char cbuff[1024];
			//sprintf(cbuff, "Script State Return : %d:", pState);
			//zlscript::CScriptDebugPrintMgr::GetInstance()->Print(cbuff);
			m_mapWaiting.erase(itWait);
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
				m_pScriptState->m_CallStateId = nScriptStateID;
			}
			RunFun(m_pScriptState, strScript, ParmInfo);
		}
	}
	void CScriptVirtualMachine::EventNetworkRunScriptFun(int nSendID, CScriptStack& ParmInfo)
	{
		int nEventID = ScriptStack_GetInt(ParmInfo);
		__int64 nScriptStateID = ScriptStack_GetInt(ParmInfo);

		CScriptRunState* m_pScriptState = new CScriptRunState;
		if (m_pScriptState)
		{
			//__int64 nCallStateID = ScriptStack_GetInt(ParmInfo);
			std::string strScript = ScriptStack_GetString(ParmInfo);
			m_pScriptState->nNetworkID = nSendID;

			//printf("network run script : %s\n", strScript.c_str());
			if (nScriptStateID != 0)
			{
				m_pScriptState->nCallEventIndex = nEventID;
				m_pScriptState->m_CallStateId = nScriptStateID;
			}
			RunFun(m_pScriptState, strScript, ParmInfo);
		}
	}
}