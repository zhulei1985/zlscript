#include "scriptcommon.h"

#include "EScriptSentenceType.h"

#include "ScriptVirtualMachine.h"
#include "EScriptVariableType.h"
#include "ScriptCallBackFunion.h"
#include "ScriptExecCodeMgr.h"
#include "ScriptExecBlock.h"
#include "ScriptSuperPointer.h"
#include "ScriptClassMgr.h"
#include <string>

namespace zlscript
{
	CScriptExecBlock::CScriptExecBlock(CScriptCodeLoader::tagCodeData* pData, CScriptRunState* pMaster):m_stackRegister(256)
	{
		m_cReturnRegisterIndex = 0;
		m_pCurCode = nullptr;
		m_pMaster = pMaster;
		m_pCodeData = pData;
		m_nTempVarSize = 0;
		m_pTempVar = nullptr;
		if (pData)
		{
			m_pCurCode = pData->pBeginCode;
			m_nTempVarSize = pData->vNumVar.size();
			if (m_nTempVarSize > 0)
			{
				m_pTempVar = new StackVarInfo[m_nTempVarSize];

				for (unsigned int i = 0; i < m_nTempVarSize; i++)
				{
					m_pTempVar[i] = pData->vNumVar[i];
				}
			}
		}

	}

	CScriptExecBlock::~CScriptExecBlock(void)
	{
		//vNumVar.clear();
		if (m_pTempVar)
		{
			delete[] m_pTempVar;
		}
	}

	int CScriptExecBlock::GetFunType()
	{
		if (m_pCodeData)
		{
			return m_pCodeData->nType;
		}
		return 0;
	}
	inline __int64 CScriptExecBlock::GetVal_Int64(char cType, unsigned int pos)
	{
		switch (cType)
		{
		case ESIGN_VALUE_INT://数值常量
			{
				return (__int64)pos;
			}
			break;
		}
		StackVarInfo var = GetVal(cType, pos);
		return GetInt_StackVar(&var);
	}
	StackVarInfo CScriptExecBlock::GetVal(char cType, unsigned int pos)
	{
		switch (cType)
		{
		case ESIGN_VALUE_INT:
		{
			return StackVarInfo((__int64)pos);
		}
		break;
		case ESIGN_POS_GLOBAL_VAR://全局变量
		{
			return m_pMaster->m_pMachine->GetGlobalVar(pos);
		}
		break;
		case ESIGN_POS_LOACL_VAR:
		{
			if (m_pTempVar && pos < m_nTempVarSize)
				return m_pTempVar[pos];
		}
		break;
		case ESIGN_POS_CONST_STRING:
		{
			return m_pCodeData->vStrConst[pos].c_str();
		}
		break;
		case ESIGN_POS_CONST_FLOAT:
		{
			//浮点常量
			return m_pCodeData->vFloatConst[pos];
		}
		break;
		case ESIGN_POS_CONST_INT64:
		{
			return m_pCodeData->vInt64Const[pos];
		}
		break;
		case ESIGN_REGISTER:
		{
			return m_register[pos];
		}
		}
		return StackVarInfo();
	}
	bool CScriptExecBlock::SetVal(char cType, unsigned int pos, StackVarInfo& var)
	{
		switch (cType)
		{
		case ESIGN_POS_GLOBAL_VAR://全局变量
		{
			m_pMaster->m_pMachine->SetGlobalVar(pos, var);
		}
		break;
		case ESIGN_POS_LOACL_VAR:
		{
			if (m_pTempVar && pos < m_nTempVarSize)
			{
				m_pTempVar[pos] = var;
			}
		}
		break;
		case ESIGN_REGISTER:
		{
			m_register[pos]= var;
		}
		break;
		default:
		return false;
		}
		return true;
	}
	int CScriptExecBlock::GetDefaultReturnType()
	{
		if (m_pCodeData)
		{
			return m_pCodeData->nDefaultReturnType;
		}
		return 0;
	}

	int CScriptExecBlock::GetCurCodeSoureIndex()
	{
		if (m_pCurCode)
		{
			return m_pCurCode->nSoureWordIndex;
		}
		return 0;
	}

	unsigned int CScriptExecBlock::ExecBlock(CScriptVirtualMachine* pMachine)
	{
		if (m_pMaster == nullptr || m_pCodeData == nullptr || pMachine == nullptr)
		{
			return ERESULT_ERROR;
		}
		int nResult = ERESULT_CONTINUE;
		auto oldTime = std::chrono::steady_clock::now();
		//unsigned int nDataLen = m_pCodeData->vCodeData.size();
		//CodeStyle* pData = &m_pCodeData->vCodeData[0];
		while (m_pCurCode)
		{
			nResult = ERESULT_CONTINUE;
			//nResult = m_pCurCode->Run(this,&m_pCurCode);

			switch (m_pCurCode->unInstruct)
			{
			case ECODE_ADD:
				{
					CAddExeCode* pCode = (CAddExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = nVal1 + nVal2;
					}
					break;
					case EScriptVal_Double:
					{
						double dVal1 = GetFloat_StackVar(&m_register[R_A]);
						double dVal2 = GetFloat_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = dVal1 + dVal2;
					}
					break;
					case EScriptVal_String:
					{
						std::string strVal1 = GetString_StackVar(&m_register[R_A]);
						std::string strVal2 = GetString_StackVar(&rightVar);
						strVal1 = strVal1 + strVal2;
						m_register[pCode->cResultRegister] = strVal1.c_str();
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}

					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_SUM:
				{
					CSumExeCode* pCode = (CSumExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = nVal1 - nVal2;;
					}
					break;
					case EScriptVal_Double:
					{
						double dVal1 = GetFloat_StackVar(&m_register[R_A]);
						double dVal2 = GetFloat_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = dVal1 - dVal2;
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_MUL:
				{
					CMulExeCode* pCode = (CMulExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = nVal1 * nVal2;
					}
					break;
					case EScriptVal_Double:
					{
						double dVal1 = GetFloat_StackVar(&m_register[R_A]);
						double dVal2 = GetFloat_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = dVal1 * dVal2;
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_DIV:
				{
					CDivExeCode* pCode = (CDivExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						if (nVal2 == 0)
						{
							m_register[pCode->cResultRegister] = (__int64)0xffffffff;
						}
						else
						{
							m_register[pCode->cResultRegister] = nVal1 / nVal2;
						}
					}
					break;
					case EScriptVal_Double:
					{
						double dVal1 = GetFloat_StackVar(&m_register[R_A]);
						double dVal2 = GetFloat_StackVar(&rightVar);
						if (dVal1 <= 0.00000001f && dVal1 >= -0.00000001f)
						{
							m_register[pCode->cResultRegister] = (double)1.7976931348623158e+308;
						}
						else
						{
							m_register[pCode->cResultRegister] = dVal1 / dVal2;
						}
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_MOD:
				{
					CModExeCode* pCode = (CModExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);

					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					if (nVal2 == 0)
					{
						m_register[pCode->cResultRegister] = (__int64)0;
					}
					else
					{
						m_register[pCode->cResultRegister] = nVal1 % nVal2;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_MINUS:
				{
					CMinusExeCode* pCode = (CMinusExeCode*)m_pCurCode;
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						m_register[pCode->cResultRegister] = -m_register[R_A].Int64;
					}
					break;
					case EScriptVal_Double:
					{
						m_register[pCode->cResultRegister] = -m_register[R_A].Double;
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_CMP_EQUAL:
				{
					CCmpEqualExeCode* pCode = (CCmpEqualExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(nVal1 == nVal2 ? 1 : 0);
					}
					break;
					case EScriptVal_Double:
					{
						double dVal1 = GetFloat_StackVar(&m_register[R_A]);
						double dVal2 = GetFloat_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(dVal1 == dVal2 ? 1 : 0);
					}
					break;
					case EScriptVal_String:
					{
						std::string strVal1 = GetString_StackVar(&m_register[R_A]);
						std::string strVal2 = GetString_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(strVal1 == strVal2 ? 1 : 0);
					}
					break;
					case EScriptVal_ClassPoint:
					{
						__int64 nVal1 = GetPointIndex_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetPointIndex_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(nVal1 == nVal2 ? 1 : 0);
					}
					break;
					default:
						return CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_CMP_NOTEQUAL:
				{
					CCmpNotEqualExeCode* pCode = (CCmpNotEqualExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(nVal1 != nVal2 ? 1 : 0);
					}
					break;
					case EScriptVal_Double:
					{
						double dVal1 = GetFloat_StackVar(&m_register[R_A]);
						double dVal2 = GetFloat_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(dVal1 != dVal2 ? 1 : 0);
					}
					break;
					case EScriptVal_String:
					{
						std::string strVal1 = GetString_StackVar(&m_register[R_A]);
						std::string strVal2 = GetString_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(strVal1 != strVal2 ? 1 : 0);
					}
					break;
					case EScriptVal_ClassPoint:
					{
						__int64 nVal1 = GetPointIndex_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetPointIndex_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(nVal1 != nVal2 ? 1 : 0);
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_CMP_BIG:
				{
					CCmpBigExeCode* pCode = (CCmpBigExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(nVal1 > nVal2 ? 1 : 0);
					}
					break;
					case EScriptVal_Double:
					{
						double dVal1 = GetFloat_StackVar(&m_register[R_A]);
						double dVal2 = GetFloat_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(dVal1 > dVal2 ? 1 : 0);
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_CMP_BIGANDEQUAL:
				{
					CCmpBigAndEqualExeCode* pCode = (CCmpBigAndEqualExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(nVal1 >= nVal2 ? 1 : 0);
					}
					break;
					case EScriptVal_Double:
					{
						double dVal1 = GetFloat_StackVar(&m_register[R_A]);
						double dVal2 = GetFloat_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(dVal1 >= dVal2 ? 1 : 0);
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_CMP_LESS:
				{
					CCmpLessExeCode* pCode = (CCmpLessExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(nVal1 < nVal2 ? 1 : 0);
					}
					break;
					case EScriptVal_Double:
					{
						double dVal1 = GetFloat_StackVar(&m_register[R_A]);
						double dVal2 = GetFloat_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(dVal1 < dVal2 ? 1 : 0);
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_CMP_LESSANDEQUAL:
				{
					CCmpLessAndEqualExeCode* pCode = (CCmpLessAndEqualExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(nVal1 <= nVal2 ? 1 : 0);
					}
					break;
					case EScriptVal_Double:
					{
						double dVal1 = GetFloat_StackVar(&m_register[R_A]);
						double dVal2 = GetFloat_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = (__int64)(dVal1 <= dVal2 ? 1 : 0);
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_BIT_AND:
				{
					CBitAndExeCode* pCode = (CBitAndExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = nVal1 & nVal2;
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_BIT_OR:
				{
					CBitOrExeCode* pCode = (CBitOrExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = nVal1 | nVal2;
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_BIT_XOR:
				{
					CBitXorExeCode* pCode = (CBitXorExeCode*)m_pCurCode;
					StackVarInfo rightVar = GetVal(pCode->cType, pCode->dwPos);
					switch (m_register[R_A].cType)
					{
					case EScriptVal_Int:
					{
						__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
						__int64 nVal2 = GetInt_StackVar(&rightVar);
						m_register[pCode->cResultRegister] = nVal1 ^ nVal2;
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_PUSH:
				{
					CPushExeCode* pCode = (CPushExeCode*)m_pCurCode;
					StackVarInfo var = GetVal(pCode->cType, pCode->dwPos);
					ScriptVector_PushVar(m_stackRegister, &var);
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_POP:
				{
					CPopExeCode* pCode = (CPopExeCode*)m_pCurCode;
					StackVarInfo var = ScriptStack_GetVar(m_stackRegister);
					if (SetVal(pCode->cType, pCode->dwPos, var) == false)
					{
						return CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_LOAD:
				{
					CLoadExeCode* pCode = (CLoadExeCode*)m_pCurCode;
					m_register[pCode->cResultRegister] = GetVal(pCode->cType, pCode->dwPos);
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_MOVE:
				{
					CMoveExeCode* pCode = (CMoveExeCode*)m_pCurCode;
					switch (pCode->cType)
					{
					case ESIGN_POS_GLOBAL_VAR://全局变量
					{
						m_pMaster->m_pMachine->SetGlobalVar(pCode->dwPos, m_register[pCode->cResultRegister]);
					}
					break;
					case ESIGN_POS_LOACL_VAR:
					{
						if (m_pTempVar && pCode->dwPos < m_nTempVarSize)
						{
							m_pTempVar[pCode->dwPos] = m_register[pCode->cResultRegister];
						}
					}
					break;
					case ESIGN_REGISTER:
					{
						m_register[pCode->dwPos] = m_register[pCode->cResultRegister];
					}
					break;
					default:
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					//if (SetVal(pCode->cType, pCode->dwPos, m_register[pCode->cResultRegister]) == false)
					//{
					//	nResult= CScriptExecBlock::ERESULT_ERROR;
					//}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_GET_CLASS_PARAM:
				{
					CGetClassParamExeCode* pCode = (CGetClassParamExeCode*)m_pCurCode;
					if (m_register[pCode->cClassRegIndex].cType == EScriptVal_ClassPoint)
					{
						//PointVarInfo pointVal = GetPointIndex_StackVar(&m_register[code.cSign]);
						CScriptBasePointer* pPoint = m_register[pCode->cClassRegIndex].pPoint;
						if (pPoint)
						{
							pPoint->Lock();
							auto pAttribute = pPoint->GetAttribute(pCode->dwPos);
							if (pAttribute)
							{
								m_register[pCode->cResultRegister] = pAttribute->ToScriptVal();
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
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_SET_CLASS_PARAM:
				{
					CSetClassParamExeCode* pCode = (CSetClassParamExeCode*)m_pCurCode;
					if (m_register[pCode->cClassRegIndex].cType == EScriptVal_ClassPoint)
					{
						//PointVarInfo pointVal = GetPointIndex_StackVar(&m_register[code.cSign]);
						CScriptBasePointer* pPoint = m_register[pCode->cClassRegIndex].pPoint;
						if (pPoint)
						{
							pPoint->Lock();
							auto pAttribute = pPoint->GetAttribute(pCode->dwPos);
							if (pAttribute)
							{
								pAttribute->SetVal(m_register[pCode->cVarRegister]);
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
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_CALL_CALLBACK:
				{
					CCallBackExeCode* pCode = (CCallBackExeCode*)m_pCurCode;
					if (m_stackRegister.size() >= pCode->cParmSize)
					{
						CACHE_NEW(CScriptCallState, pCallState, m_pMaster);
						if (pCallState)
						{
							//设置参数
							for (unsigned int i = m_stackRegister.size() - pCode->cParmSize; i < m_stackRegister.size(); i++)
							{
								auto pVar = m_stackRegister.GetVal(i);
								if (pVar)
									pCallState->PushVarToStack(*pVar);
							}
							for (unsigned char i = 0; i < pCode->cParmSize; i++)
							{
								m_stackRegister.pop();
							}
							//运行回调函数
							switch (m_pMaster->CallFun_CallBack(m_pMaster->m_pMachine, pCode->unFunIndex, pCallState))
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
							//执行完将结果放入寄存器
							m_register[pCode->cResultRegister] = pCallState->GetResult();
						}
						CACHE_DELETE(pCallState);
					}
					else
					{
						//TODO 报错
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_CALL_SCRIPT:
				{
					CCallScriptExeCode* pCode = (CCallScriptExeCode*)m_pCurCode;
					if (m_stackRegister.size() >= pCode->cParmSize)
					{
						//运行回调函数
						switch (m_pMaster->CallFun_Script(m_pMaster->m_pMachine, pCode->unFunIndex, m_stackRegister, pCode->cParmSize, pCode->cResultRegister))
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
						for (unsigned char i = 0; i < pCode->cParmSize; i++)
						{
							m_stackRegister.pop();
						}
					}
					else
					{
						nResult = CScriptExecBlock::ERESULT_ERROR;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_JUMP:
				{
					CJumpExeCode* pCode = (CJumpExeCode*)m_pCurCode;
					if (pCode->pJumpCode)
						m_pCurCode = pCode->pJumpCode;
					else
						m_pCurCode = pCode->m_pNext;
				}
				break;
			case ECODE_JUMP_TRUE:
				{
					CJumpTrueExeCode* pCode = (CJumpTrueExeCode*)m_pCurCode;
					if (CheckRegisterTrue(pCode->cVarRegister))
					{
						m_pCurCode = pCode->pJumpCode;
					}
					else
					{
						m_pCurCode = pCode->m_pNext;
					}
				}
				break;
			case ECODE_JUMP_FALSE:
				{
					CJumpFalseExeCode* pCode = (CJumpFalseExeCode*)m_pCurCode;
					if (!CheckRegisterTrue(pCode->cVarRegister))
					{
						m_pCurCode = pCode->pJumpCode;
					}
					else
					{
						m_pCurCode = pCode->m_pNext;
					}
				}
				break;
			case ECODE_RETURN:
				{
					CReturnExeCode* pCode = (CReturnExeCode*)m_pCurCode;
					m_varReturnVar = m_register[pCode->cVarRegister];
					m_pCurCode = nullptr;
					nResult = CScriptExecBlock::ERESULT_END;
				}
				break;
			case ECODE_CALL_CLASS_FUN:
				{
					CCallClassFunExeCode* pCode = (CCallClassFunExeCode*)m_pCurCode;
					PointVarInfo pointVal = GetPoint_StackVar(&m_register[pCode->cClassRegIndex]);

					if (m_stackRegister.size() >= pCode->cParmSize)
					{
						CACHE_NEW(CScriptCallState, pCallState, m_pMaster);
						if (pCallState)
						{
							//设置参数
							for (unsigned int i = m_stackRegister.size() - pCode->cParmSize; i < m_stackRegister.size(); i++)
							{
								auto pVar = m_stackRegister.GetVal(i);
								if (pVar)
									pCallState->PushVarToStack(*pVar);
							}
							for (unsigned char i = 0; i < pCode->cParmSize; i++)
							{
								m_stackRegister.pop();
							}

							CScriptBasePointer* pPoint = pointVal.pPoint;
							if (pPoint)
							{
								switch (pPoint->RunFun(pCode->dwPos, pCallState))
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
								//执行完将结果放入寄存器
								m_register[pCode->cResultRegister] = pCallState->GetResult();
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
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_NEW_CLASS:
				{
					CNewClassExeCode* pCode = (CNewClassExeCode*)m_pCurCode;
					CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(pCode->dwClassIndex);
					if (pMgr)
					{
						auto pNewPoint = pMgr->New(SCRIPT_NO_USED_AUTO_RELEASE);
						m_register[pCode->cResultRegister] = pNewPoint;
					}
					else
					{
						m_register[pCode->cResultRegister] = (CScriptPointInterface*)nullptr;
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_RELEASE_CLASS:
				{
					CReleaseClassExeCode* pCode = (CReleaseClassExeCode*)m_pCurCode;
					PointVarInfo pointVal = GetPoint_StackVar(&m_register[pCode->cVarRegister]);
					CScriptBasePointer* pPoint = pointVal.pPoint;
					if (pPoint)
					{
						CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(pPoint->GetType());
						if (pMgr)
						{
							pMgr->Release(pPoint);
						}
					}
					m_pCurCode = m_pCurCode->m_pNext;
				}
				break;
			case ECODE_BREAK:
				{
					CBreakExeCode* pCode = (CBreakExeCode*)m_pCurCode;
					if (pCode->pJumpCode)
						m_pCurCode = pCode->pJumpCode;
					else
						m_pCurCode = pCode->m_pNext;
				}
				break;
			case ECODE_CONTINUE:
				{
					CContinueExeCode* pCode = (CContinueExeCode*)m_pCurCode;
					if (pCode->pJumpCode)
						m_pCurCode = pCode->pJumpCode;
					else
						m_pCurCode = pCode->m_pNext;
				}
				break;
			default:
				m_pCurCode = m_pCurCode->m_pNext;
			}
			if (nResult != ERESULT_CONTINUE)
			{	
				break;
			}
		}

		auto nowTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - oldTime);
		m_msRunningTime += duration;
		if (m_pCurCode == nullptr)
		{
			nResult = ERESULT_END;
		}
		return nResult;
	}

	bool CScriptExecBlock::CheckRegisterTrue(char index)
	{
		switch (m_register[index].cType)
		{
		case EScriptVal_None:
			return false;
		case EScriptVal_Int:
			if (m_register[index].Int64 != 0)
			{
				return true;
			}
			break;
		case EScriptVal_Double:
			if (m_register[index].Double != 0)
			{
				return true;
			}
			break;
		case EScriptVal_String:
		{
			const char* pStr = StackVarInfo::s_strPool.GetString(m_register[index].Int64);
			if (pStr)
			{
				if (strlen(pStr) > 0)
				{
					return true;
				}
			}
		}
		break;
		case EScriptVal_ClassPoint:
			if (m_register[index].pPoint)
			{
				if (m_register[index].pPoint->GetPoint())
				{
					return true;
				}
			}
			break;
		case EScriptVal_Binary:
		{
			unsigned int nSize = 0;
			const char* pStr = StackVarInfo::s_binPool.GetBinary(m_register[index].Int64, nSize);
			if (pStr)
			{
				return true;
			}
		}
		break;
		default:
			//TODO 报错
			break;
		}
		return false;
	}

	void CScriptExecBlock::PushVar(StackVarInfo& var)
	{
		m_stackRegister.push(var);
	}

	StackVarInfo CScriptExecBlock::PopVar()
	{
		StackVarInfo var;
		if (!m_stackRegister.empty())
		{
			var = m_stackRegister.top();
			m_stackRegister.pop();
		}
		return var;
	}

	StackVarInfo* CScriptExecBlock::GetVar(unsigned int index)
	{
		if (m_stackRegister.size() > index)
		{
			return m_stackRegister.GetVal(index);
		}
		return nullptr;
	}

	unsigned int CScriptExecBlock::GetVarSize()
	{
		return m_stackRegister.size();
	}

	void CScriptExecBlock::ClearStack()
	{
		while (!m_stackRegister.empty())
		{
			m_stackRegister.pop();
		}
	}

	std::string CScriptExecBlock::GetCurSourceWords()
	{

		if (m_pMaster == nullptr || m_pCodeData == nullptr)
		{
			return std::string();
		}
		//int nResult = ERESULT_CONTINUE;
		//if (m_nCodePoint < m_pCodeData->vCodeData.size())
		//{
		//	CodeStyle& code = m_pCodeData->vCodeData[m_nCodePoint];

		//}
		return std::string();
	}
	CScriptExecBlockStack::CScriptExecBlockStack()
	{
		unIndex = 0;
		m_Vec.resize(32);
	}
	CScriptExecBlockStack::~CScriptExecBlockStack()
	{
	}
	unsigned int CScriptExecBlockStack::size()
	{
		return unIndex;
	}
	bool CScriptExecBlockStack::empty()
	{
		return unIndex == 0;
	}
	void CScriptExecBlockStack::push(CScriptExecBlock* pBlock)
	{
		if (pBlock)
		{
			if (unIndex >= m_Vec.size())
			{
				m_Vec.push_back(pBlock);
				unIndex = m_Vec.size();
			}
			else
			{
				m_Vec[unIndex] = pBlock;
				unIndex++;
			}

		}
	}
	CScriptExecBlock* CScriptExecBlockStack::top()
	{
		if (unIndex > 0)
		{
			return m_Vec[unIndex - 1];
		}
		return nullptr;
	}
	void CScriptExecBlockStack::pop()
	{
		if (unIndex > 0)
		{
			m_Vec[unIndex - 1] = nullptr;
			unIndex--;
		}
	}
	CScriptExecBlock* CScriptExecBlockStack::get(unsigned int i)
	{
		if (unIndex > i)
		{
			return m_Vec[i];
		}
		return nullptr;
	}
}