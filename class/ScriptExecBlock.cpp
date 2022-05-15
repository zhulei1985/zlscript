#include "scriptcommon.h"

#include "EScriptSentenceType.h"

#include "ScriptVirtualMachine.h"
#include "EScriptVariableType.h"
#include "ScriptCallBackFunion.h"
#include "ScriptExecBlock.h"
#include "ScriptSuperPointer.h"
#include "ScriptClassMgr.h"
#include "ScriptIntermediateCode.h"
#include <string>

namespace zlscript
{
	CScriptExecBlock::CScriptExecBlock(CFunICode* pCode, CScriptRunState* pMaster)
	{
		m_pCurCode = nullptr;
		m_pMaster = pMaster;
		m_pCurCode = pCode;


	}

	CScriptExecBlock::~CScriptExecBlock(void)
	{
		STACK_CLEAR(registerStack);
	}

	int CScriptExecBlock::GetFunType()
	{
		//if (m_pCodeData)
		//{
		//	return m_pCodeData->nType;
		//}
		return 0;
	}

	int CScriptExecBlock::GetDefaultReturnType()
	{

		return 0;
	}

	void CScriptExecBlock::RegisterLoaclVar(int index, int type)
	{
		if (index < 0)
		{
			return;
		}
		if (index >= loaclVarStack.nSize)
		{
			loaclVarStack.m_vData.resize(index+1);
			loaclVarStack.nIndex = index;
			loaclVarStack.nSize = index + 1;		
		}
		loaclVarStack.m_vData[index] = CScriptVarTypeMgr::GetInstance()->GetVar(type);
	}

	CBaseVar* CScriptExecBlock::GetLoaclVar(int index)
	{
		CBaseVar* pResult = nullptr;
		STACK_GET_INDEX(loaclVarStack, pResult, index);
		return pResult;
	}

	int& CScriptExecBlock::GetRunState(int index)
	{
		if (index < 0)
		{
			return nOtherState;
		}
		if (m_vRunState.size() <= index)
		{
			m_vRunState.resize(index + 1);
		}
		// TODO: 在此处插入 return 语句
		return m_vRunState[index];
	}

	void CScriptExecBlock::RevertRunState(int index)
	{
		if (index >= 0 && index < m_vRunState.size())
		{
			m_vRunState[index] = 0;
		}
	}

	int CScriptExecBlock::GetCurCodeSoureIndex()
	{
		if (m_pCurCode)
		{
			return m_pCurCode->m_unBeginSoureIndex;
		}
		return 0;
	}

	unsigned int CScriptExecBlock::ExecBlock(CScriptVirtualMachine* pMachine)
	{
		if (m_pMaster == nullptr || m_pCurCode == nullptr || pMachine == nullptr)
		{
			return ERESULT_ERROR;
		}
		int nResult = ERESULT_CONTINUE;
		auto oldTime = std::chrono::steady_clock::now();
		//unsigned int nDataLen = m_pCodeData->vCodeData.size();
		//CodeStyle* pData = &m_pCodeData->vCodeData[0];
		while (m_pCurCode)
		{
			nResult = m_pCurCode->Run(this);

			if (nResult != ERESULT_CONTINUE)
			{	
				break;
			}
		}

		auto nowTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - oldTime);
		m_msRunningTime += duration;
		//if (nResult == ERESULT_CALLSCRIPTFUN)
		//{
		//	return nResult;
		//}
		if (m_pCurCode == nullptr)
		{
			nResult = ERESULT_END;
		}
		return nResult;
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