#include "scriptcommon.h"

#include "ScriptCodeLoader.h"
#include "EScriptSentenceType.h"

#include "ScriptVirtualMachine.h"
#include "EScriptVariableType.h"
#include "ScriptCallBackFunion.h"
#include "ScriptExecBlock.h"
#include "ScriptSuperPointer.h"
#include "ScriptClassMgr.h"
#include "ScriptIntermediateCode.h"
#include "ScriptCodeStyle.h"
#include "ScriptVarAssignmentMgr.h"
#include <string>

namespace zlscript
{
	CScriptExecBlock::CScriptExecBlock(CExeCodeData* pCode, CScriptRunState* pMaster)
	{
		m_pCodeData = nullptr;
		m_pMaster = pMaster;
		m_pCodeData = pCode;

		initLocalVar();
		//initFixedRegister();
		if (m_pCodeData)
		{
			m_pCurExeCode = m_pCodeData->pBeginCode;
		}
	}

	CScriptExecBlock::~CScriptExecBlock(void)
	{
		STACK_CLEAR(registerStack);
		for (unsigned int i = 0; i < m_pCodeData->vLocalVarType.size(); i++)
		{
			CScriptVarTypeMgr::GetInstance()->ReleaseVar(loaclVarStack.m_vData[i]);
		}
		loaclVarStack.m_vData.clear();
		loaclVarStack.nIndex = 0;
		loaclVarStack.nSize = 0;

		//for (unsigned int i = 0; i < fixedRegister.size(); i++)
		//{
		//	CScriptVarTypeMgr::GetInstance()->ReleaseVar(fixedRegister[i]);
		//}
		//fixedRegister.clear();
		
		for (auto it = m_mapVarCache.begin(); it != m_mapVarCache.end(); it++)
		{
			tagScriptVarStack& stack = it->second;
			STACK_CLEAR(stack);
		}
	}

	int CScriptExecBlock::GetFunType()
	{
		//if (m_pCodeData)
		//{
		//	return m_pCodeData->nType;
		//}
		return 0;
	}

	void CScriptExecBlock::initLocalVar()
	{
		if (m_pCodeData == nullptr)
		{
			return;
		}
		loaclVarStack.m_vData.resize(m_pCodeData->vLocalVarType.size());
		loaclVarStack.nIndex = 0;
		loaclVarStack.nSize = loaclVarStack.m_vData.size();
		for (unsigned int i = 0; i < m_pCodeData->vLocalVarType.size(); i++)
		{
			loaclVarStack.m_vData[i] = CScriptVarTypeMgr::GetInstance()->GetVar(m_pCodeData->vLocalVarType[i]);
		}
	}

	//void CScriptExecBlock::initFixedRegister()
	//{
	//}

	int CScriptExecBlock::GetDefaultReturnType()
	{

		return 0;
	}
	
	//void CScriptExecBlock::RegisterLoaclVar(int index, int type)
	//{
	//	if (index < 0)
	//	{
	//		return;
	//	}
	//	if (index >= loaclVarStack.nSize)
	//	{
	//		loaclVarStack.m_vData.resize(index+1);
	//		loaclVarStack.nIndex = index;
	//		loaclVarStack.nSize = index + 1;		
	//	}
	//	loaclVarStack.m_vData[index] = CScriptVarTypeMgr::GetInstance()->GetVar(type);
	//}

	const CBaseVar* CScriptExecBlock::GetLoaclVar(int index)
	{
		CBaseVar* pResult = nullptr;
		if (index < loaclVarStack.nSize)
		{
			pResult = loaclVarStack.m_vData[index];
		}
		return pResult;
	}

	bool CScriptExecBlock::SetLoaclVar(int index, const CBaseVar* pVar)
	{
		if (index < loaclVarStack.nSize)
		{
			CBaseVar *pLoackVar = loaclVarStack.m_vData[index];
			AssignVar(pLoackVar, pVar);
		}
		return false;
	}

	const CBaseVar* CScriptExecBlock::GetConstVar(int index)
	{
		if ((unsigned int)index < m_pCodeData->vConstVar.size())
		{
			return m_pCodeData->vConstVar[index];
		}
		return nullptr;
	}

	CBaseVar* CScriptExecBlock::NewVar(int type)
	{
		auto it = m_mapVarCache.find(type);
		if (it != m_mapVarCache.end())
		{
			tagScriptVarStack& stack = it->second;
			CBaseVar* pResult = nullptr;
			STACK_POP(stack, pResult);
			if (pResult)
			{
				return pResult;
			}
		}
		return CScriptVarTypeMgr::GetInstance()->GetVar(type);
	}

	void CScriptExecBlock::ReleaseVar(CBaseVar* pVar)
	{
		if (pVar == nullptr)
		{
			return;
		}
		if (!pVar->isClassPoint())
		{
			tagScriptVarStack& stack = m_mapVarCache[pVar->GetType()];
			STACK_PUSH_MOVE(stack, pVar);
			return;
		}
		CScriptVarTypeMgr::GetInstance()->ReleaseVar(pVar);
	}

	//void CScriptExecBlock::RegisterRunState(int size)
	//{
	//	m_vRunState.resize(size);
	//}

	//int& CScriptExecBlock::GetRunState(int index)
	//{
	//	if (index < 0)
	//	{
	//		return nOtherState;
	//	}
	//	if (m_vRunState.size() <= index)
	//	{
	//		return nOtherState;
	//	}
	//	// TODO: 在此处插入 return 语句
	//	return m_vRunState[index];
	//}

	//void CScriptExecBlock::RevertRunState(int index)
	//{
	//	if (index >= 0 && index < m_vRunState.size())
	//	{
	//		m_vRunState[index] = 0;
	//	}
	//}

	int CScriptExecBlock::GetCurCodeSoureIndex()
	{
		if (m_pCurExeCode)
		{
			return m_pCurExeCode->nSoureWordIndex;
		}
		return 0;
	}

	unsigned int CScriptExecBlock::ExecBlock(CScriptVirtualMachine* pMachine)
	{
		if (m_pMaster == nullptr || m_pCodeData == nullptr || pMachine == nullptr)
		{
			return ERESULT_ERROR;
		}
		//FILE* fp = fopen("RunDebug.txt", "ab");
		//if (fp)
		//{
		//	fputs("********************************\n", fp);
		//	fputs(m_pCodeData->funname.c_str(), fp);
		//	fputc('\t', fp);
		//	char strbuff[32] = { 0 };
		//	sprintf(strbuff, "runIndex[%d]\n", (int)this);
		//	fputs(strbuff, fp);
		//	fputs("********************************\n", fp);
		//}
		int nResult = ERESULT_CONTINUE;
		auto oldTime = std::chrono::steady_clock::now();

		while (m_pCurExeCode)
		{
			//if (fp)
			//{
			//	std::string str = m_pCurExeCode->GetCodeString();
			//	char strbuff[32] = { 0 };
			//	sprintf(strbuff, "[%d]\t", m_pCurExeCode->nCodeIndex);
			//	fputs(strbuff, fp);
			//	fputs(str.c_str(), fp);
			//	fputc('\n', fp);
			//}
			printf("run %s\n", m_pCurExeCode->GetCodeString().c_str());
			nResult = m_pCurExeCode->Run(this, &m_pCurExeCode);

			if (nResult != ERESULT_CONTINUE)
			{	
				break;
			}
		}
		//if (fp)
		//	fclose(fp);
		auto nowTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - oldTime);
		m_msRunningTime += duration;

		if (nResult == ERESULT_CONTINUE && m_pCurExeCode == nullptr)
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