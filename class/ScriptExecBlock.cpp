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
		m_nCodePoint = 0;
		m_pMaster = pMaster;
		m_pCodeData = pData;
		if (pData)
		{
			vNumVar.resize(pData->vNumVar.size());
			for (unsigned int i = 0; i < pData->vNumVar.size(); i++)
			{
				StackVarInfo& var = vNumVar[i];
				var = pData->vNumVar[i];
			}

		}

	}

	CScriptExecBlock::~CScriptExecBlock(void)
	{
		vNumVar.clear();
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
			if (pos < vNumVar.size())
				return vNumVar[pos];
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
	int CScriptExecBlock::GetDefaultReturnType()
	{
		if (m_pCodeData)
		{
			return m_pCodeData->nDefaultReturnType;
		}
		return 0;
	}

	CodeStyle CScriptExecBlock::GetCurCode()
	{
		if (m_pCodeData)
		{
			return m_pCodeData->vCodeData[m_nCodePoint];
		}
		return CodeStyle(0);
	}

	unsigned int CScriptExecBlock::ExecBlock(CScriptVirtualMachine* pMachine)
	{
		if (m_pMaster == nullptr || m_pCodeData == nullptr || pMachine == nullptr)
		{
			return ERESULT_ERROR;
		}
		int nResult = ERESULT_CONTINUE;
		auto oldTime = std::chrono::steady_clock::now();
		while (m_nCodePoint < m_pCodeData->vCodeData.size())
		{
			CodeStyle& code = m_pCodeData->vCodeData[m_nCodePoint];
			switch (code.wInstruct)
			{
				/*********************计算符************************/
			case ECODE_ADD:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					m_register[code.cExtend] = nVal1 + nVal2;
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&m_register[R_A]);
					double dVal2 = GetFloat_StackVar(&rightVar);
					m_register[code.cExtend] = dVal1 + dVal2;
				}
				break;
				case EScriptVal_String:
				{
					std::string strVal1 = GetString_StackVar(&m_register[R_A]);
					std::string strVal2 = GetString_StackVar(&rightVar);
					strVal1 = strVal1 + strVal2;
					m_register[code.cExtend] = strVal1.c_str();
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_SUM:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					m_register[code.cExtend] = nVal1 - nVal2;;
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&m_register[R_A]);
					double dVal2 = GetFloat_StackVar(&rightVar);
					m_register[code.cExtend] = dVal1 - dVal2;
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_MUL:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					m_register[code.cExtend] = nVal1 * nVal2;
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&m_register[R_A]);
					double dVal2 = GetFloat_StackVar(&rightVar);
					m_register[code.cExtend] = dVal1 * dVal2;
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_DIV:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					if (nVal2 == 0)
					{
						m_register[code.cExtend] = (__int64)0xffffffff;
					}
					else
					{
						m_register[code.cExtend] = nVal1 / nVal2;
					}
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&m_register[R_A]);
					double dVal2 = GetFloat_StackVar(&rightVar);
					if (dVal1 <= 0.00000001f && dVal1 >= -0.00000001f)
					{
						m_register[code.cExtend] = (double)1.7976931348623158e+308;
					}
					else
					{
						m_register[code.cExtend] = dVal1 / dVal2;
					}
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_MOD:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				//switch (var2.cType)
				//{
				//case EScriptVal_Int:
				//	{
				__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
				__int64 nVal2 = GetInt_StackVar(&rightVar);
				if (nVal2 == 0)
				{
					m_register[code.cExtend] = (__int64)0;
				}
				else
				{
					m_register[code.cExtend] = nVal1 % nVal2;
				}
				//	}
				//	break;
				//}
				m_nCodePoint++;
			}
			break;
			case ECODE_MINUS:
			{
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					m_register[code.cExtend].Int64 = -m_register[R_A].Int64;
				}
				break;
				case EScriptVal_Double:
				{
					m_register[code.cExtend].Double = -m_register[R_A].Double;
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_EQUAL://比较
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					m_register[code.cExtend] = (__int64)(nVal1 == nVal2 ? 1 : 0);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&m_register[R_A]);
					double dVal2 = GetFloat_StackVar(&rightVar);
					m_register[code.cExtend] =(__int64)(dVal1 == dVal2 ? 1 : 0);
				}
				break;
				case EScriptVal_String:
				{
					std::string strVal1 = GetString_StackVar(&m_register[R_A]);
					std::string strVal2 = GetString_StackVar(&rightVar);
					m_register[code.cExtend] = (__int64)(strVal1 == strVal2 ? 1 : 0);
				}
				break;
				case EScriptVal_ClassPoint:
				{
					__int64 nVal1 = GetPointIndex_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetPointIndex_StackVar(&rightVar);
					m_register[code.cExtend] = (__int64)(nVal1 == nVal2 ? 1 : 0);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_NOTEQUAL:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					m_register[code.cExtend] =(__int64)(nVal1 != nVal2 ? 1 : 0);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&m_register[R_A]);
					double dVal2 = GetFloat_StackVar(&rightVar);
					m_register[code.cExtend] = (__int64)(dVal1 != dVal2 ? 1 : 0);
				}
				break;
				case EScriptVal_String:
				{
					std::string strVal1 = GetString_StackVar(&m_register[R_A]);
					std::string strVal2 = GetString_StackVar(&rightVar);
					m_register[code.cExtend] = (__int64)(strVal1 != strVal2 ? 1 : 0);
				}
				break;
				case EScriptVal_ClassPoint:
				{
					__int64 nVal1 = GetPointIndex_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetPointIndex_StackVar(&rightVar);
					m_register[code.cExtend] =(__int64)(nVal1 != nVal2 ? 1 : 0);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_BIG:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					m_register[code.cExtend] = (__int64)(nVal1 > nVal2 ? 1 : 0);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&m_register[R_A]);
					double dVal2 = GetFloat_StackVar(&rightVar);
					m_register[code.cExtend] = (__int64)(dVal1 > dVal2 ? 1 : 0);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_BIGANDEQUAL:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					m_register[code.cExtend] =(__int64)(nVal1 >= nVal2 ? 1 : 0);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&m_register[R_A]);
					double dVal2 = GetFloat_StackVar(&rightVar);
					m_register[code.cExtend] = (__int64)(dVal1 >= dVal2 ? 1 : 0);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_LESS:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					m_register[code.cExtend] = (__int64)(nVal1 < nVal2 ? 1 : 0);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&m_register[R_A]);
					double dVal2 = GetFloat_StackVar(&rightVar);
					m_register[code.cExtend] = (__int64)(dVal1 < dVal2 ? 1 : 0);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_LESSANDEQUAL:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					m_register[code.cExtend] = (__int64)(nVal1 <= nVal2 ? 1 : 0);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&m_register[R_A]);
					double dVal2 = GetFloat_StackVar(&rightVar);
					m_register[code.cExtend] = (__int64)(dVal1 <= dVal2 ? 1 : 0);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_BIT_AND:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					m_register[code.cExtend] = nVal1 & nVal2;
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_BIT_OR:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					m_register[code.cExtend] = nVal1 | nVal2;
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_BIT_XOR:
			{
				StackVarInfo rightVar = GetVal(code.cSign, code.dwPos);
				switch (m_register[R_A].cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&m_register[R_A]);
					__int64 nVal2 = GetInt_StackVar(&rightVar);
					m_register[code.cExtend] = nVal1 ^ nVal2;
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			/*********************************功能符*******************************/
			case ECODE_PUSH://压入变量
			{
				switch (code.cSign)
				{
				case ESIGN_VALUE_INT://数值常量
				{
					ScriptVector_PushVar(m_stackRegister, (__int64)code.dwPos);
				}
				break;
				case ESIGN_POS_GLOBAL_VAR://全局变量
				{
					StackVarInfo var = pMachine->GetGlobalVar(code.dwPos);
	
					ScriptVector_PushVar(m_stackRegister, &var);
				}
				break;
				case ESIGN_POS_LOACL_VAR:
				{
					if (code.dwPos < vNumVar.size())
					{
						StackVarInfo& var = vNumVar[code.dwPos];
						ScriptVector_PushVar(m_stackRegister, &var);
					}
					else
						ScriptVector_PushEmptyVar(m_stackRegister);
				}
				break;
				case ESIGN_POS_CONST_STRING:
				{
					ScriptVector_PushVar(m_stackRegister, m_pCodeData->vStrConst[code.dwPos].c_str());
				}
				break;
				case ESIGN_POS_CONST_FLOAT:
				{
					//浮点常量
					ScriptVector_PushVar(m_stackRegister, m_pCodeData->vFloatConst[code.dwPos]);
				}
				break;
				case ESIGN_POS_CONST_INT64:
				{
					ScriptVector_PushVar(m_stackRegister, m_pCodeData->vInt64Const[code.dwPos]);
				}
				break;
				case ESIGN_REGISTER:
				{
					ScriptVector_PushVar(m_stackRegister, &m_register[code.dwPos]);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_POP:
			{
				switch (code.cSign)
				{
				case ESIGN_POS_GLOBAL_VAR://全局变量
					{
						StackVarInfo var = ScriptStack_GetVar(m_stackRegister);
						pMachine->SetGlobalVar(code.dwPos, var);
					}
					break;
				case ESIGN_POS_LOACL_VAR:
					{
						StackVarInfo& var = vNumVar[code.dwPos];
						var = ScriptStack_GetVar(m_stackRegister);
					}
					break;
				case ESIGN_REGISTER:
					{
						StackVarInfo& var = m_register[code.dwPos];
						var = ScriptStack_GetVar(m_stackRegister);
					}
					break;
				default:
					//TODO 报错
					break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_STATEMENT_END:
			{
				ClearStack();
				//m_pMaster->ClearStack();
				m_nCodePoint++;
			}
			break;
			case ECODE_LOAD:
			{
				switch (code.cSign)
				{
				case ESIGN_VALUE_INT://数值常量
				{
					m_register[code.cExtend] = (__int64)code.dwPos;
				}
				break;
				case ESIGN_POS_GLOBAL_VAR://全局变量
				{
					m_register[code.cExtend] = pMachine->GetGlobalVar(code.dwPos);
				}
				break;
				case ESIGN_POS_LOACL_VAR:
				{
					m_register[code.cExtend] = vNumVar[code.dwPos];
				}
				break;
				case ESIGN_POS_CONST_STRING:
				{
					m_register[code.cExtend] = m_pCodeData->vStrConst[code.dwPos].c_str();
				}
				break;
				case ESIGN_POS_CONST_FLOAT:
				{
					//浮点常量
					m_register[code.cExtend] = m_pCodeData->vFloatConst[code.dwPos];
				}
				break;
				case ESIGN_POS_CONST_INT64:
				{
					m_register[code.cExtend] = m_pCodeData->vInt64Const[code.dwPos];
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_MOVE:
			{
				switch (code.cSign)
				{
				case ESIGN_POS_GLOBAL_VAR://全局变量
					{
						pMachine->SetGlobalVar(code.dwPos, m_register[code.cExtend]);
					}
					break;
				case ESIGN_POS_LOACL_VAR:
					{
						vNumVar[code.dwPos] = m_register[code.cExtend];
					}
					break;
				case ESIGN_REGISTER:
					{
						m_register[code.dwPos] = m_register[code.cExtend];
					}
					break;
				default:
					//TODO 报错
					break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_SET_CLASS_PARAM:
			{
				if (m_register[code.cSign].cType == EScriptVal_ClassPoint)
				{
					//PointVarInfo pointVal = GetPointIndex_StackVar(&m_register[code.cSign]);
					CScriptBasePointer* pPoint = m_register[code.cSign].pPoint;
					if (pPoint)
					{
						pPoint->Lock();
						auto pAttribute = pPoint->GetAttribute(code.dwPos);
						if (pAttribute)
						{
							pAttribute->SetVal(m_register[code.cExtend]);
						}
						else
						{
							//TODO 报错
						}
						pPoint->Unlock();
					}
					m_nCodePoint++;
				}
				else
				{
					//TODO 报错
				}
			}
			break;
			case ECODE_GET_CLASS_PARAM:
			{
				if (m_register[code.cSign].cType == EScriptVal_ClassPoint)
				{
					//PointVarInfo pointVal = GetPointIndex_StackVar(&m_register[code.cSign]);
					CScriptBasePointer* pPoint = m_register[code.cSign].pPoint;
					if (pPoint)
					{
						pPoint->Lock();
						auto pAttribute = pPoint->GetAttribute(code.dwPos);
						if (pAttribute)
						{
							m_register[code.cExtend] = pAttribute->ToScriptVal();
						}
						else
						{
							//TODO 报错
						}
						pPoint->Unlock();
					}
					m_nCodePoint++;
				}
				else
				{
					//TODO 报错
				}
			}
			break;
			//case ECODE_BEGIN_CALL:
			//{
			//	m_sCurStackSizeWithoutFunParam.push(m_stackRegister.size());
			//	m_nCodePoint++;
			//}
			//break;
			case ECODE_CALL_CALLBACK:		//调用回调函数
			{
				if (m_stackRegister.size() >= code.cExtend)
				{
					CACHE_NEW(CScriptCallState, pCallState, m_pMaster);
					if (pCallState)
					{
						//设置参数
						for (unsigned int i = m_stackRegister.size() - code.cExtend; i < m_stackRegister.size(); i++)
						{
							auto pVar = m_stackRegister.GetVal(i);
							if (pVar)
								pCallState->PushVarToStack(*pVar);
						}
						for (unsigned char i = 0; i < code.cExtend; i++)
						{
							m_stackRegister.pop();
						}
						//运行回调函数
						switch (m_pMaster->CallFun_CallBack(pMachine, code.dwPos, pCallState))
						{
						case ECALLBACK_ERROR:
							nResult = ERESULT_ERROR;
							break;
						case ECALLBACK_WAITING:
							nResult = ERESULT_WAITING;
							break;
						case ECALLBACK_CALLSCRIPTFUN:

							nResult = ERESULT_CALLSCRIPTFUN;
							break;
						case ECALLBACK_NEXTCONTINUE:
							nResult = ERESULT_NEXTCONTINUE;
							break;
						}

						m_nCodePoint++;
						//执行完将结果放入寄存器
						m_register[code.cSign] = pCallState->GetResult();
					}
					CACHE_DELETE(pCallState);
				}
				else
				{
					//TODO 报错
					nResult = ERESULT_ERROR;
				}
			}
			break;
			case ECODE_CALL_SCRIPT:	//调用脚本函数
			{
				if (m_stackRegister.size() >= code.cExtend)
				{
					//CScriptStack ParmStack;
					//for (unsigned int i = m_stackRegister.size() - code.cExtend; i < m_stackRegister.size(); i++)
					//{
					//	auto pVar = m_stackRegister.GetVal(i);
					//	if (pVar)
					//		ParmStack.push(*pVar);
					//}
					//for (unsigned char i = 0; i < code.cExtend; i++)
					//{
					//	m_stackRegister.pop();
					//}
					//运行回调函数
					switch (m_pMaster->CallFun_Script(pMachine, code.dwPos, m_stackRegister, code.cExtend))
					{
					case ECALLBACK_ERROR:
						nResult = ERESULT_ERROR;
						break;
					case ECALLBACK_WAITING:
						nResult = ERESULT_WAITING;
						break;
					case ECALLBACK_CALLSCRIPTFUN:

						nResult = ERESULT_CALLSCRIPTFUN;
						break;
					case ECALLBACK_NEXTCONTINUE:
						nResult = ERESULT_NEXTCONTINUE;
						break;
					}
					for (unsigned char i = 0; i < code.cExtend; i++)
					{
						m_stackRegister.pop();
					}
					m_nCodePoint++;

				}
				else
				{
					//TODO 报错
					nResult = ERESULT_ERROR;
				}
			}
			break;
			case ECODE_JUMP:
			{
				if (code.cSign == 0)
				{
					m_nCodePoint = code.dwPos;
				}
				else
				{
					m_nCodePoint = m_nCodePoint + code.dwPos;
				}
			}
			break;
			case ECODE_JUMP_TRUE:
			{
				if (CheckRegisterTrue(code.cExtend))
				{
					if (code.cSign == 0)
					{
						m_nCodePoint = code.dwPos;
					}
					else
					{
						m_nCodePoint = m_nCodePoint + code.dwPos;
					}
				}
				else
				{
					m_nCodePoint++;
				}
			}
			break;
			case ECODE_JUMP_FALSE:
			{
				if (!CheckRegisterTrue(code.cExtend))
				{
					if (code.cSign == 0)
					{
						m_nCodePoint = code.dwPos;
					}
					else
					{
						m_nCodePoint = m_nCodePoint + code.dwPos;
					}
				}
				else
				{
					m_nCodePoint++;
				}
			}
			break;
			case ECODE_RETURN:	//退出此块
			{
				m_nCodePoint = m_pCodeData->vCodeData.size();
				m_varReturnVar = m_register[code.cExtend];
				nResult = ERESULT_END;
			}
			break;
			case ECODE_CLEAR_PARAM:
			{
				while (m_stackRegister.size() > 0)
				{
					m_stackRegister.pop();
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CALL_CLASS_FUN:
			{
				PointVarInfo pointVal = GetPoint_StackVar(&m_register[code.cSign]);

				if (m_stackRegister.size() >= code.cExtend)
				{
					CACHE_NEW(CScriptCallState, pCallState, m_pMaster);
					if (pCallState)
					{
						//设置参数
						for (unsigned int i = m_stackRegister.size() - code.cExtend; i < m_stackRegister.size(); i++)
						{
							auto pVar = m_stackRegister.GetVal(i);
							if (pVar)
								pCallState->PushVarToStack(*pVar);
						}
						for (unsigned char i = 0; i < code.cExtend; i++)
						{
							m_stackRegister.pop();
						}

						CScriptBasePointer* pPoint = pointVal.pPoint;
						if (pPoint)
						{
							switch (pPoint->RunFun(code.dwPos, pCallState))
							{
							case ECALLBACK_ERROR:
								nResult = ERESULT_ERROR;
								break;
							case ECALLBACK_WAITING:
								nResult = ERESULT_WAITING;
								break;
							case ECALLBACK_CALLSCRIPTFUN:

								nResult = ERESULT_CALLSCRIPTFUN;
								break;
							case ECALLBACK_NEXTCONTINUE:
								nResult = ERESULT_NEXTCONTINUE;
								break;
							}
							m_nCodePoint++;
							//执行完将结果放入寄存器
							m_register[code.cSign] = pCallState->GetResult();
						}
						else
						{
							nResult = ERESULT_ERROR;
						}
					}
					CACHE_DELETE(pCallState);
				}
				else
				{
					nResult = ERESULT_ERROR;
				}
			}
			break;
			case ECODE_NEW_CLASS: //新建一个类实例
			{
				CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(code.dwPos);
				if (pMgr)
				{
					auto pNewPoint = pMgr->New(SCRIPT_NO_USED_AUTO_RELEASE);
					m_register[code.cSign] = pNewPoint;
				}
				else
				{
					m_register[code.cSign] = (CScriptPointInterface*)nullptr;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_RELEASE_CLASS://释放一个类实例
			{
				PointVarInfo pointVal = GetPoint_StackVar(&m_register[code.cSign]);
				CScriptBasePointer* pPoint = pointVal.pPoint;
				if (pPoint)
				{
					CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(pPoint->GetType());
					if (pMgr)
					{
						pMgr->Release(pPoint);
					}
				}
				m_nCodePoint++;
			}
			break;
			//****************标识，接下来的运算需要什么类型的变量***************************//

			default:
			{
				//警告，未知代码
				m_nCodePoint++;
			}
			break;
			}
			if (nResult != ERESULT_CONTINUE)
			{	
				break;
			}
		}

		auto nowTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - oldTime);
		m_msRunningTime += duration;
		if (m_nCodePoint >= m_pCodeData->vCodeData.size())
		{
			nResult = ERESULT_END;
		}
		return nResult;
	}

	inline bool CScriptExecBlock::CheckRegisterTrue(char index)
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
		int nResult = ERESULT_CONTINUE;
		if (m_nCodePoint < m_pCodeData->vCodeData.size())
		{
			CodeStyle& code = m_pCodeData->vCodeData[m_nCodePoint];

		}
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