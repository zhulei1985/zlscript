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
	CScriptExecBlock::CScriptExecBlock(tagCodeData* pData, CScriptRunState* pMaster):m_stackRegister(256)
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
			nResult = m_pCurCode->Run(this,&m_pCurCode);

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