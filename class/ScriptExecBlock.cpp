#include "scriptcommon.h"

#include "EScriptSentenceType.h"
#include "EMicroCodeType.h"
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
	CScriptExecBlock::CScriptExecBlock(CScriptCodeLoader::tagCodeData* pData, CScriptRunState* pMaster):m_varRegister(256)
	{
		CurCallFunParamNum = 0;
		m_nCodePoint = 0;
		m_nCycBlockEnd = 0;
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

		m_cVarType = EScriptVal_Int;
		m_vbIfSign.resize(256);
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
	int CScriptExecBlock::GetDefaultReturnType()
	{
		if (m_pCodeData)
		{
			return m_pCodeData->nDefaultReturnType;
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
		while (m_nCodePoint < m_pCodeData->vCodeData.size())
		{
			CodeStyle& code = m_pCodeData->vCodeData[m_nCodePoint];
			switch (code.wInstruct)
			{
				/*********************计算符************************/
			case ECODE_ADD:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, nVal2 + nVal1);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, dVal2 + dVal1);
				}
				break;
				case EScriptVal_String:
				{
					std::string strVal1 = GetString_StackVar(&var1);
					std::string strVal2 = GetString_StackVar(&var2);
					strVal2 = strVal2 + strVal1;
					ScriptVector_PushVar(m_varRegister, (char*)strVal2.c_str());
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_SUM:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, nVal2 - nVal1);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, dVal2 - dVal1);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_MUL:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, nVal2 * nVal1);
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, dVal2 * dVal1);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_DIV:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					if (nVal1 == 0)
					{
						ScriptVector_PushVar(m_varRegister, (__int64)0xffffffff);
					}
					else
					{
						ScriptVector_PushVar(m_varRegister, nVal2 / nVal1);
					}
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					if (dVal1 <= 0.00000001f && dVal1 >= -0.00000001f)
					{
						ScriptVector_PushVar(m_varRegister, (double)1.7976931348623158e+308);
					}
					else
					{
						ScriptVector_PushVar(m_varRegister, dVal2 / dVal1);
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
				__int64 nVal1 = ScriptStack_GetInt(m_varRegister);
				__int64 nVal2 = ScriptStack_GetInt(m_varRegister);
				if (nVal1 == 0)
				{
					ScriptVector_PushVar(m_varRegister, (__int64)0);
				}
				else
				{
					ScriptVector_PushVar(m_varRegister, nVal2 % nVal1);
				}
				//	}
				//	break;
				//}
				m_nCodePoint++;
			}
			break;
			case ECODE_MINUS:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				switch (var1.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);

					ScriptVector_PushVar(m_varRegister, -nVal1);

				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);

					ScriptVector_PushVar(m_varRegister, -dVal1);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_EQUAL://比较
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(nVal2 == nVal1 ? 1 : 0));
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(dVal2 == dVal1 ? 1 : 0));
				}
				break;
				case EScriptVal_String:
				{
					std::string strVal1 = GetString_StackVar(&var1);
					std::string strVal2 = GetString_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(strVal2 == strVal1 ? 1 : 0));
				}
				break;
				case EScriptVal_ClassPointIndex:
				{
					__int64 nVal1 = GetPointIndex_StackVar(&var1);
					__int64 nVal2 = GetPointIndex_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(nVal1 == nVal2 ? 1 : 0));
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_NOTEQUAL:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(nVal2 != nVal1 ? 1 : 0));
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(dVal2 != dVal1 ? 1 : 0));
				}
				break;
				case EScriptVal_String:
				{
					std::string strVal1 = GetString_StackVar(&var1);
					std::string strVal2 = GetString_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(strVal2 != strVal1 ? 1 : 0));
				}
				break;
				case EScriptVal_ClassPointIndex:
				{
					__int64 nVal1 = GetPointIndex_StackVar(&var1);
					__int64 nVal2 = GetPointIndex_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(nVal1 != nVal2 ? 1 : 0));
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_BIG:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(nVal2 > nVal1 ? 1 : 0));
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(dVal2 > dVal1 ? 1 : 0));
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_BIGANDEQUAL:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(nVal2 >= nVal1 ? 1 : 0));
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(dVal2 >= dVal1 ? 1 : 0));
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_LESS:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(nVal2 < nVal1 ? 1 : 0));
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(dVal2 < dVal1 ? 1 : 0));
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_CMP_LESSANDEQUAL:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(nVal2 <= nVal1 ? 1 : 0));
				}
				break;
				case EScriptVal_Double:
				{
					double dVal1 = GetFloat_StackVar(&var1);
					double dVal2 = GetFloat_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, (__int64)(dVal2 <= dVal1 ? 1 : 0));
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_BIT_AND:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, nVal2 & nVal1);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_BIT_OR:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, nVal2 | nVal1);
				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_BIT_XOR:
			{
				StackVarInfo var1 = ScriptStack_GetVar(m_varRegister);
				StackVarInfo var2 = ScriptStack_GetVar(m_varRegister);
				switch (var2.cType)
				{
				case EScriptVal_Int:
				{
					__int64 nVal1 = GetInt_StackVar(&var1);
					__int64 nVal2 = GetInt_StackVar(&var2);
					ScriptVector_PushVar(m_varRegister, nVal2 ^ nVal1);
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
					ScriptVector_PushVar(m_varRegister, (__int64)code.dwPos);
				}
				break;
				case 1://全局变量
				{
					StackVarInfo var = pMachine->GetGlobalVar(code.dwPos);
	
					ScriptVector_PushVar(m_varRegister, &var);
				}
				break;
				case 2:
				{
					StackVarInfo& var = vNumVar[code.dwPos];
					ScriptVector_PushVar(m_varRegister, &var);
				}
				break;
				case 3:
				{
					m_cVarType = EScriptVal_String;
					ScriptVector_PushVar(m_varRegister, (char*)m_pCodeData->vStrConst[code.dwPos].c_str());
				}
				break;
				case 4:
				{
					//浮点常量
					m_cVarType = EScriptVal_Double;
					ScriptVector_PushVar(m_varRegister, m_pCodeData->vFloatConst[code.dwPos]);
				}
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
			case ECODE_EVALUATE:
			{
				switch (code.cSign)
				{
				case 0://全局变量
				{
					auto var = ScriptStack_GetVar(m_varRegister);
					pMachine->SetGlobalVar(code.dwPos, var);

				}
				break;
				case 1:
				{
					StackVarInfo& var = vNumVar[code.dwPos];
					var = ScriptStack_GetVar(m_varRegister);

				}
				break;
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_BEGIN_CALL:
			{
				m_sCurStackSizeWithoutFunParam.push(m_varRegister.size());
				m_nCodePoint++;
			}
			break;
			case ECODE_CALL:		//调用函数
			{
				int nParmNum = code.cExtend;
				if (m_sCurStackSizeWithoutFunParam.size() > 0)
				{
					nParmNum = m_varRegister.size() - m_sCurStackSizeWithoutFunParam.top();
					m_sCurStackSizeWithoutFunParam.pop();
				}
				unsigned int nCodeIndex = code.dwPos;
				if (code.cSign == 1)
				{
					if (m_pCodeData->vCallFunName.size() > code.dwPos)
					{
						std::string& funname = m_pCodeData->vCallFunName[code.dwPos];
						if (CScriptExecCodeMgr::GetInstance()->RemoteFunctionCall(funname, m_pMaster, nParmNum))
						{
							m_nCodePoint++;
							nResult = ERESULT_WAITING;
							break;
						}

						nCodeIndex = CScriptCodeLoader::GetInstance()->GetCodeIndex(funname.c_str());
					}
					else
					{
						nResult = ERESULT_ERROR;
						break;
					}
				}

				switch (m_pMaster->CallFun(pMachine, this, code.cSign, nCodeIndex, nParmNum))
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
				__int64 nVal = ScriptStack_GetInt(m_varRegister);
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
				__int64 nVal = ScriptStack_GetInt(m_varRegister);
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
				if (m_pCodeData->nDefaultReturnType != EScriptVal_None)
				{
					//当此函数需要返回值，但是没有的时候
					if (CurStackSizeWithoutFunParam == m_varRegister.size())
					{
						//压入一个空值
						ScriptVector_PushEmptyVar(m_varRegister);
					}
				}
			}
			break;
			case ECODE_CLEAR_PARAM:
			{
				while (m_varRegister.size() > 0)
				{
					m_varRegister.pop();
				}
				CurStackSizeWithoutFunParam = 0;
				CurCallFunParamNum = 0;
				m_nCodePoint++;
			}
			break;
			case ECODE_CALL_CLASS_FUN:
			{
				__int64 nVal = ScriptStack_GetClassPointIndex(m_varRegister);
				int nParmNum = code.cExtend;
				if (m_sCurStackSizeWithoutFunParam.size() > 0)
				{
					nParmNum = m_varRegister.size() - m_sCurStackSizeWithoutFunParam.top();
					m_sCurStackSizeWithoutFunParam.pop();
				}
				CScriptBasePointer* pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(nVal);
				if (pPoint)
				{
					SetCallFunParamNum(nParmNum);
					//CurCallFunParamNum = code.cExtend;

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
			case ECODE_NEW_CLASS: //新建一个类实例
			{
				CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(code.dwPos);
				if (pMgr)
				{
					auto pNewPoint = pMgr->New();
					ScriptVector_PushVar(m_varRegister, pNewPoint);
				}
				else
				{
					ScriptVector_PushVar(m_varRegister, (CScriptPointInterface*)nullptr);
				}
				m_nCodePoint++;
			}
			break;
			case ECODE_RELEASE_CLASS://释放一个类实例
			{
				__int64 nVal = ScriptStack_GetClassPointIndex(m_varRegister);
				CScriptBasePointer* pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(nVal);
				if (pPoint)
				{
					CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(pPoint->GetType());
					if (pMgr)
					{
						pMgr->Release(pPoint);
					}
					CScriptSuperPointerMgr::GetInstance()->ReturnPointer(pPoint);
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

	void CScriptExecBlock::PushVar(StackVarInfo& var)
	{
		m_varRegister.push(var);
	}

	StackVarInfo CScriptExecBlock::PopVar()
	{
		StackVarInfo var;
		if (!m_varRegister.empty())
		{
			var = m_varRegister.top();
			m_varRegister.pop();
		}
		return var;
	}

	StackVarInfo* CScriptExecBlock::GetVar(unsigned int index)
	{
		if (m_varRegister.size() > index)
		{
			return m_varRegister.GetVal(index);
		}
		return nullptr;
	}

	unsigned int CScriptExecBlock::GetVarSize()
	{
		return m_varRegister.size();
	}

	void CScriptExecBlock::ClearFunParam()
	{
		while (m_varRegister.size() > 0 && (int)m_varRegister.size() > CurStackSizeWithoutFunParam)
		{
			m_varRegister.pop();
		}
	}

	void CScriptExecBlock::ClearStack()
	{
		while (!m_varRegister.empty())
		{
			m_varRegister.pop();
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
}