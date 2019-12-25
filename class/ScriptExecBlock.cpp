#include "scriptcommon.h"

#include "EScriptSentenceType.h"
#include "EMicroCodeType.h"
#include "ScriptVirtualMachine.h"
#include "EScriptVariableType.h"
#include "ScriptCallBackFunion.h"

#include "ScriptExecBlock.h"
#include "ScriptSuperPointer.h"
#include <string>
namespace zlscript
{
	CScriptExecBlock::CScriptExecBlock(CScriptCodeLoader::tagCodeData* pData, CScriptRunState* pMaster)
	{
		m_nCodePoint = 0;
		m_nCycBlockEnd = 0;
		m_pMaster = pMaster;
		m_pCodeData = pData;
		if (pData)
		{
			vNumVar.resize(pData->vNumVar.size());
			for (unsigned int i = 0; i < pData->vNumVar.size(); i++)
			{
				CScriptCodeLoader::VarPoint& var = vNumVar[i];
				var.cType = pData->vNumVar[i].cType;
				var.unArraySize = pData->vNumVar[i].unArraySize;
				switch (var.cType)
				{
				case EScriptVal_Int:
				{
					var.pInt64 = new __int64[var.unArraySize];
				}
				break;
				case EScriptVal_Double:
				{
					var.pDouble = new double[var.unArraySize];
				}
				break;
				case  EScriptVal_String:
				{
					var.pStr = new char[var.unArraySize * MAXSIZE_STRING];
				}
				break;
				}
			}

		}

		m_cVarType = EScriptVal_Int;
		m_vbIfSign.resize(256);
	}

	CScriptExecBlock::~CScriptExecBlock(void)
	{
		for (unsigned int i = 0; i < vNumVar.size(); i++)
		{
			CScriptCodeLoader::VarPoint& var = vNumVar[i];
			switch (var.cType)
			{
			case EScriptVal_Int:
			{
				SAFE_DELETE_ARRAY(var.pInt64);
			}
			break;
			case EScriptVal_Double:
			{
				SAFE_DELETE_ARRAY(var.pDouble);
			}
			break;
			case  EScriptVal_String:
			{
				SAFE_DELETE_ARRAY(var.pStr);
			}
			break;
			}
		}
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

	unsigned int CScriptExecBlock::ExecBlock(CScriptVirtualMachine* pMachine)
	{
		if (m_pMaster == NULL || m_pCodeData == NULL || pMachine == NULL)
		{
			return ERESULT_ERROR;
		}
		int nResult = ERESULT_CONTINUE;
		if (m_nCodePoint < m_pCodeData->vCodeData.size())
		{
			CScriptCodeLoader::CodeStyle& code = m_pCodeData->vCodeData[m_nCodePoint];
			switch (code.wInstruct)
			{
				/*********************计算符************************/
			case ECODE_ADD:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal2 + nVal1);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					m_pMaster->PushVarToStack(dVal2 + dVal1);
				}
				break;
				case EScriptVal_String:
				{
					std::string strVal1 = GetString_StackVar(&var1);
					std::string strVal2 = GetString_StackVar(&var2);
					strVal2 = strVal2 + strVal1;
					m_pMaster->PushAndNewStrVarToStack((char*)strVal2.c_str());
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_SUM:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal2 - nVal1);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					m_pMaster->PushVarToStack(dVal2 - dVal1);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_MUL:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal2 * nVal1);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					m_pMaster->PushVarToStack(dVal2 * dVal1);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_DIV:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					if (nVal1 == 0)
					{
						m_pMaster->PushVarToStack((int)0xffffffff);
					}
					else
					{
						m_pMaster->PushVarToStack(nVal2 / nVal1);
					}
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					if (dVal1 <= 0.00000001f && dVal1 >= -0.00000001f)
					{
						m_pMaster->PushVarToStack((double)1.7976931348623158e+308);
					}
					else
					{
						m_pMaster->PushVarToStack(dVal2 / dVal1);
					}
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_MOD:
			{
				//switch (var2.cType)
				//{
				//case EScriptVal_Int:
				//	{
				__int64 nVal1 = m_pMaster->PopIntVarFormStack();
				__int64 nVal2 = m_pMaster->PopIntVarFormStack();
				if (nVal1 == 0)
				{
					m_pMaster->PushVarToStack(0);
				}
				else
				{
					m_pMaster->PushVarToStack(nVal2 % nVal1);
				}
				//	}
				//	break;
				//}
				m_nCodePoint++;
			}
			break;
			case ECODE_MINUS:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				switch (var1.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);

					m_pMaster->PushVarToStack(-nVal1);

				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);

					m_pMaster->PushVarToStack(-dVal1);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_EQUAL://比较
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal2 == nVal1 ? 1 : 0);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					m_pMaster->PushVarToStack(dVal2 == dVal1 ? 1 : 0);
				}
				break;
				case EScriptVal_String:
				{
					std::string strVal1 = GetString_StackVar(&var1);
					std::string strVal2 = GetString_StackVar(&var2);
					m_pMaster->PushVarToStack(strVal2 == strVal1 ? 1 : 0);
				}
				break;
				case EScriptVal_ClassPointIndex:
				{
					__int64 nVal1 = GetPointIndex_StackVar(&var1);
					__int64 nVal2 = GetPointIndex_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal1 == nVal2 ? 1 : 0);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_NOTEQUAL:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal2 != nVal1 ? 1 : 0);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					m_pMaster->PushVarToStack(dVal2 != dVal1 ? 1 : 0);
				}
				break;
				case EScriptVal_String:
				{
					std::string strVal1 = GetString_StackVar(&var1);
					std::string strVal2 = GetString_StackVar(&var2);
					m_pMaster->PushVarToStack(strVal2 != strVal1 ? 1 : 0);
				}
				break;
				case EScriptVal_ClassPointIndex:
				{
					__int64 nVal1 = GetPointIndex_StackVar(&var1);
					__int64 nVal2 = GetPointIndex_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal1 != nVal2 ? 1 : 0);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_BIG:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal2 > nVal1 ? 1 : 0);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					m_pMaster->PushVarToStack(dVal2 > dVal1 ? 1 : 0);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_BIGANDEQUAL:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal2 >= nVal1 ? 1 : 0);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					m_pMaster->PushVarToStack(dVal2 >= dVal1 ? 1 : 0);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_LESS:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal2 < nVal1 ? 1 : 0);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					m_pMaster->PushVarToStack(dVal2 < dVal1 ? 1 : 0);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_LESSANDEQUAL:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal2 <= nVal1 ? 1 : 0);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					m_pMaster->PushVarToStack(dVal2 <= dVal1 ? 1 : 0);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_BIT_AND:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal2 & nVal1);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_BIT_OR:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal2 | nVal1);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_BIT_XOR:
			{
				StackVarInfo var1 = m_pMaster->PopVarFormStack();
				StackVarInfo var2 = m_pMaster->PopVarFormStack();
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					m_pMaster->PushVarToStack(nVal2 ^ nVal1);
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
				case 0://数值常量
				{
					m_cVarType = EScriptVal_Int;
					m_pMaster->PushVarToStack((int)code.dwPos);
				}
				break;
				case 1://全局变量
				{
					CScriptCodeLoader::VarPoint* var = pMachine->GetGlobalVar(code.dwPos);
					if (var)
					{
						switch (var->cType)
						{
						case EScriptVal_Int:
							m_cVarType = EScriptVal_Int;
							m_pMaster->PushVarToStack(var->pInt64[code.cExtend]);
							break;
						case EScriptVal_Double:
							m_cVarType = EScriptVal_Double;
							m_pMaster->PushVarToStack(var->pDouble[code.cExtend]);
							break;
						case EScriptVal_String:
							m_cVarType = EScriptVal_String;
							m_pMaster->PushVarToStack(var->pStr[MAXSIZE_STRING * code.cExtend]);
							break;
						case EScriptVal_ClassPointIndex:
							m_cVarType = EScriptVal_ClassPointIndex;
							m_pMaster->PushClassPointToStack(var->nClassPointIndex);
							break;
						}
					}
					else
					{
						m_pMaster->PushVarToStack((int)0);
					}
				}
				break;
				case 2:
				{
					CScriptCodeLoader::VarPoint& var = vNumVar[code.dwPos];
					switch (var.cType)
					{
					case EScriptVal_Int:
						m_cVarType = EScriptVal_Int;
						m_pMaster->PushVarToStack(var.pInt64[code.cExtend]);
						break;
					case EScriptVal_Double:
						m_cVarType = EScriptVal_Double;
						m_pMaster->PushVarToStack(var.pDouble[code.cExtend]);
						break;
					case EScriptVal_String:
						m_cVarType = EScriptVal_String;
						m_pMaster->PushVarToStack(&var.pStr[MAXSIZE_STRING * code.cExtend]);
						break;
					case EScriptVal_ClassPointIndex:
						m_cVarType = EScriptVal_ClassPointIndex;
						m_pMaster->PushClassPointToStack(var.nClassPointIndex);
						break;
					}
				}
				break;
				case 3:
				{
					m_cVarType = EScriptVal_String;
					m_pMaster->PushVarToStack((char*)m_pCodeData->vStrConst[code.dwPos].c_str());
				}
				break;
				case 4:
				{
					//浮点常量
					m_cVarType = EScriptVal_Double;
					m_pMaster->PushVarToStack(m_pCodeData->vFloatConst[code.dwPos]);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_STATEMENT_END:
			{
				m_pMaster->ClearStack();
				m_nCodePoint++;
			}
			break;
			case ECODE_EVALUATE:
			{
				switch (code.cSign)
				{
				case 0://全局变量
				{
					CScriptCodeLoader::VarPoint* var = pMachine->GetGlobalVar(code.dwPos);
					if (var)
					{
						switch (var->cType)
						{
						case EScriptVal_Int:
							var->pInt64[code.cExtend] = m_pMaster->PopIntVarFormStack();
							break;
						case EScriptVal_Double:
							var->pDouble[code.cExtend] = m_pMaster->PopDoubleVarFormStack();
							break;
						case EScriptVal_String:
							strcpy(&var->pStr[MAXSIZE_STRING * code.cExtend], m_pMaster->PopCharVarFormStack());
							break;
						case EScriptVal_ClassPointIndex:
							var->nClassPointIndex = m_pMaster->PopClassPointFormStack();
							break;
						}
					}
					else
					{
						m_pMaster->PopIntVarFormStack();
					}
				}
				break;
				case 1:
				{
					CScriptCodeLoader::VarPoint& var = vNumVar[code.dwPos];
					switch (var.cType)
					{
					case EScriptVal_Int:
						var.pInt64[code.cExtend] = m_pMaster->PopIntVarFormStack();
						break;
					case EScriptVal_Double:
						var.pDouble[code.cExtend] = m_pMaster->PopDoubleVarFormStack();
						break;
					case EScriptVal_String:
						strcpy(&var.pStr[MAXSIZE_STRING * code.cExtend], m_pMaster->PopCharVarFormStack());
						break;
					case EScriptVal_ClassPointIndex:
						var.nClassPointIndex = m_pMaster->PopClassPointFormStack();
						break;
					}
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CALL:		//调用函数
			{
				switch (m_pMaster->CallFun(pMachine, code.cSign, code.dwPos, code.cExtend))
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
			}
			break;
			case ECODE_BRANCH_IF:	// if分支,从堆栈中取一个值，如果非0则执行接下来的块
			{
				__int64 nVal = m_pMaster->PopIntVarFormStack();
				if (nVal == 0)
				{
					m_vbIfSign[code.cSign] = false;
					m_nCodePoint = m_nCodePoint + code.dwPos;
				}
				else
				{
					m_vbIfSign[code.cSign] = true;
					m_nCodePoint++;
				}
			}
			break;
			case ECODE_BRANCH_ELSE:	// else分支，如果之前的if没有执行，则执行
			{
				if (m_vbIfSign[code.cSign] == true)
				{
					m_nCodePoint = m_nCodePoint + code.dwPos;
				}
				else
				{
					m_nCodePoint++;
				}
			}
			break;
			case ECODE_CYC_IF:
			{
				__int64 nVal = m_pMaster->PopIntVarFormStack();
				if (nVal == 0)
				{
					m_nCodePoint = m_nCodePoint + code.dwPos;
				}
				else
				{
					m_sCycBlockEnd.push(m_nCodePoint + code.dwPos);
					m_nCodePoint++;
				}
			}
			break;
			case ECODE_BLOCK://块开始标志 cSign: cExtend: dwPos:表示块大小
			{
				m_nCodePoint++;
			}
			break;
			case ECODE_BLOCK_CYC://放在块尾，执行到此返回块头
			{
				m_pMaster->ClearStack();
				m_nCodePoint = m_nCodePoint - code.dwPos;
				if (m_sCycBlockEnd.size())
				{
					m_sCycBlockEnd.pop();
				}
			}
			break;
			case ECODE_BREAK:
			{
				if (m_sCycBlockEnd.size())
				{
					m_nCodePoint = m_sCycBlockEnd.top();
					m_sCycBlockEnd.pop();
				}
				else
				{
					m_nCodePoint++;
				}
				m_pMaster->ClearStack();
			}
			break;
			case ECODE_RETURN:	//退出此块
			{
				m_nCodePoint = m_pCodeData->vCodeData.size();

				nResult = ERESULT_END;
			}
			break;
			case ECODE_CLEAR_PARAM:
			{
				m_pMaster->ClearFunParam(CurStackSizeWithoutFunParam);
				m_nCodePoint++;
			}
			break;
			case ECODE_CALL_CLASS_FUN:
			{
				__int64 nVal = m_pMaster->PopClassPointFormStack();
				CScriptBasePointer* pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(nVal);
				if (pPoint)
				{
					m_pMaster->SetParamNum(code.cExtend);
					m_pMaster->CurStackSizeWithoutFunParam = m_pMaster->m_varRegister.size() - code.cExtend;
					switch (pPoint->RunFun(code.dwPos, m_pMaster))
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
					CScriptSuperPointerMgr::GetInstance()->ReturnPointer(pPoint);
				}
				else
				{
					nResult = ERESULT_ERROR;
				}
				m_nCodePoint++;
			}
			break;
			//****************标识，接下来的运算需要什么类型的变量***************************//
			case ECODE_INT:
			{
				m_cVarType = EScriptVal_Int;
				m_nCodePoint++;
			}
			break;
			case ECODE_DOUDLE:
			{
				m_cVarType = EScriptVal_Double;
				m_nCodePoint++;
			}
			break;
			case ECODE_STRING:
			{
				m_cVarType = EScriptVal_String;
				m_nCodePoint++;
			}
			break;
			case ECODE_CLASSPOINT:
			{
				m_cVarType = EScriptVal_ClassPointIndex;
				m_nCodePoint++;
			}
			break;
			default:
			{
				//警告，未知代码
				m_nCodePoint++;
			}
			break;
			}
		}
		else
		{
			return ERESULT_END;
		}
		return nResult;
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
			CScriptCodeLoader::CodeStyle& code = m_pCodeData->vCodeData[m_nCodePoint];
#if _SCRIPT_DEBUG
			return CScriptCodeLoader::GetInstance()->GetSourceWords(code.nSourseWordIndex);
#endif
		}
		return std::string();
	}
}