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
#include "EScriptSentenceType.h"
#include "EMicroCodeType.h"
#include "EErrorType.h"
#include "ScriptCallBackFunion.h"
#include "ScriptCodeLoader.h"
#include "ScriptIntermediateCode.h"
#include "ScriptClassMgr.h"
#include "ScriptGlobalVarMgr.h"
#include "ScriptVarTypeMgr.h"

#include "ScriptExecBlock.h"
#include "ScriptPointInterface.h"
#include "ScriptVarAssignmentMgr.h"
#include "ScriptRunState.h"
namespace zlscript
{
#define AddErrorInfo(lv, pos, error) pCompiler->AddErrorInfo(lv,pos,error);
	bool CDefGlobalVarICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		if (pCompiler == nullptr)
		{
			return false;
		}

		GetNewWord(strType);
		unsigned short unVarType = CScriptVarTypeMgr::GetInstance()->GetVarType(strType.word);

		if (unVarType == EScriptVal_None)
		{
			AddErrorInfo(0,
				strType.nSourceWordsIndex,
				ERROR_VAR_TYPE_NONE);
			RevertAll();
			return false;
		}
		GetNewWord(strName);

		if (!CScriptCodeLoader::GetInstance()->CheckVarName(strName.word))
		{
			AddErrorInfo(0,
				strName.nSourceWordsIndex,
				ERROR_VAR_NAME_ILLEGAL);
			RevertAll();
			return false;
		}

		GetNewWord(strSign);

		if (strSign.word == ";")
		{
			if (!CScriptGlobalVarMgr::GetInstance()->New(strName.word, unVarType))
			{
				AddErrorInfo(
					1,
					strName.nSourceWordsIndex,
					ERROR_GLOBAL_VAR_CANNOT_ADD);
				RevertAll();
				return false;
			}
		}
		else if (strSign.word == "=")
		{
			if (!CScriptGlobalVarMgr::GetInstance()->New(strName.word, unVarType))
			{
				AddErrorInfo(1,
					strName.nSourceWordsIndex,
					ERROR_GLOBAL_VAR_CANNOT_ADD);
				RevertAll();
				return false;
			}
			int GVarIndex = CScriptGlobalVarMgr::GetInstance()->GetIndex(strName.word);
			if (GVarIndex == -1)
			{
				AddErrorInfo(1,
					strName.nSourceWordsIndex,
					ERROR_GLOBAL_VAR_CANNOT_SET);
				RevertAll();
				return false;
			}
			GetNewWord(strVal);

			if (strVal.word == "new")
			{
				GetNewWord(strClassType);
				CScriptPointInterface* pPoint =
					CScriptVarTypeMgr::GetInstance()->NewObject(
						CScriptVarTypeMgr::GetInstance()->GetVarType(strClassType.word));
				CPointVar temp;
				temp.Set(pPoint);
				CScriptGlobalVarMgr::GetInstance()->Set(GVarIndex, &temp);
			}
			else
			{
				//pGVar->pVar->Set(strVal.word);
				CStringVar temp;
				temp.Set(strVal.word);
				CScriptGlobalVarMgr::GetInstance()->Set(GVarIndex, &temp);
			}

			GetNewWord(NextWord);
			if (NextWord.word != ";")
			{
				AddErrorInfo(2,
					NextWord.nSourceWordsIndex,
					ESIGN_ILLEGAL_END);
				RevertAll();
				return false;
			}
		}
		else
		{
			RevertAll();
			return false;
		}

		return true;
	}
	bool CDefTempVarICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		if (pCompiler == nullptr)
		{
			return false;
		}

		GetNewWord(strType);
		unsigned short unVarType = CScriptVarTypeMgr::GetInstance()->GetVarType(strType.word);
		if (unVarType == EScriptVal_None)
		{
			AddErrorInfo(0,
				strType.nSourceWordsIndex,
				ERROR_VAR_TYPE_NONE);
			RevertAll();
			return false;
		}
		GetNewWord(strName);

		//检测名字是否与回调函数名冲突
		if (!CScriptCodeLoader::GetInstance()->CheckVarName(strName.word))
		{
			AddErrorInfo(0,
				strName.nSourceWordsIndex,
				ERROR_VAR_NAME_ILLEGAL);
			RevertAll();
			return false;
		}
		if (this->CheckTempVar(strName.word.c_str()))
		{
			AddErrorInfo(0,
				strName.nSourceWordsIndex,
				ERROR_VAR_NAME_EXISTS);
			RevertAll();
			return false;
		}

		GetNewWord(strSign);
		if (strSign.word == ";")
		{
			if (!this->DefineTempVar(unVarType, strName.word, pCompiler))
			{
				AddErrorInfo(1,
					strName.nSourceWordsIndex,
					ERROR_TEMP_VAR_CANNOT_ADD);
				RevertAll();
				return false;
			}
		}
		else if (strSign.word == "=")
		{
			if (!this->DefineTempVar(unVarType, strName.word, pCompiler))
			{
				AddErrorInfo(1,
					strName.nSourceWordsIndex,
					ERROR_TEMP_VAR_CANNOT_ADD);
				RevertAll();
				return false;
			}
			RevertOne();
			RevertOne();
			return true;
		}
		else
		{
			AddErrorInfo(1,
				strName.nSourceWordsIndex,
				ESIGN_ILLEGAL_END);
			RevertAll();
			return false;
		}
		return true;
	}

	bool CFunICode::DefineTempVar(int type, std::string VarName, CScriptCompiler* pCompiler)
	{
		if (m_mapTempVarInfo.find(VarName) != m_mapTempVarInfo.end())
			return false;
		VarInfo& info = m_mapTempVarInfo[VarName];
		info.nType = type;
		info.index = pCompiler->AddTempVarIndex();
		return true;
	}



	VarInfo* CFunICode::GetTempVarInfo(const char* pVarName)
	{
		auto it = m_mapTempVarInfo.find(pVarName);
		if (it != m_mapTempVarInfo.end())
		{
			return &it->second;
		}
		return nullptr;
	}


	bool CFunICode::LoadDefineFunctionParameter(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();
		GetNewWord(strType);
		GetNewWord(strName);
		int ntype = CScriptVarTypeMgr::GetInstance()->GetVarType(strType.word);
		if (!DefineTempVar(ntype, strName.word, pCompiler))
		{
			AddErrorInfo(10,
				strType.nSourceWordsIndex,
				ERROR_FUN_PARAM);
			RevertAll();
			return false;
		}
		return true;
	}

	bool CFunICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		if (pCompiler == nullptr)
		{
			return false;
		}

		GetNewWord(nextWord);

		std::string strType = nextWord.word;
		nReturnType = CScriptVarTypeMgr::GetInstance()->GetVarType(strType);
		if (strType != "void" && nReturnType == EScriptVal_None)
		{
			AddErrorInfo(0,
				nextWord.nSourceWordsIndex,
				ERROR_VAR_TYPE_NONE);
			RevertAll();
			return false;
		}
		GetNewWord(strName);

		if (!CScriptCodeLoader::GetInstance()->CheckVarName(strName.word))
		{
			AddErrorInfo(0,
				strName.nSourceWordsIndex,
				ERROR_FUN_NAME_ILLEGAL);
			RevertAll();
			return false;
		}
		//检测名字是否与回调函数名冲突
		if (CScriptCallBackFunion::GetFunIndex(strName.word) >= 0)
		{
			AddErrorInfo(0,
				strName.nSourceWordsIndex,
				ERROR_FUN_NAME_EXISTS);
			RevertAll();
			return false;
		}

		if (pCompiler->GetFunICode(strName.word) != nullptr)
		{
			AddErrorInfo(0,
				strName.nSourceWordsIndex,
				ERROR_FUN_NAME_EXISTS);
			RevertAll();
			return false;
		}

		funname = strName.word;
		GetNewWord(strSign);
		if (strSign.word != "(")
		{
			AddErrorInfo(0,
				strSign.nSourceWordsIndex,
				ERROR_FUN_FORMAT_NO_BRACKET);
			RevertAll();
			return false;
		}
		pCompiler->ClearTempVarIndex();

		if (pCompiler->SetFunICode(strName.word, this) == false)
		{
			return false;
		}

		GetWord(nextWord);
		if (nextWord.word != ")")
		{
			RevertOne();
			while (true)
			{
				if (!LoadDefineFunctionParameter(vIn, pCompiler))
				{
					RevertAll();
					return false;
				}
				nPramSize++;
				GetWord(nextWord);
				if (nextWord.word == ",")
				{
					continue;
				}
				else if (nextWord.word == ")")
				{
					break;
				}
				else
				{
					AddErrorInfo(1,
						nextWord.nSourceWordsIndex,
						ERROR_FUN_FORMAT_PARAM);
					RevertAll();
					return false;
				}
			}
		}
		GetWord(nextWord);
		if (nextWord.word == ";")
		{
			//只定义没实现
		}
		else if (nextWord.word == "{")
		{
			while (true)
			{
				//有定义
				if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_STATEMENT, this, 0))
				{
					RevertAll();
					return false;
				}

				GetWord(nextWord);
				if (nextWord.word == "}")
				{
					break;
				}
				else
				{
					RevertOne();
				}
			}
		}

		return true;
	}

	void CFunICode::AddICode(int nType, CBaseICode* pCode)
	{
		vBodyCode.push_back(pCode);
		CBaseICode::AddICode(nType, pCode);
	}

	bool CFunICode::MakeExeCode(CExeCodeData& vOut)
	{
		for (auto it = m_mapTempVarInfo.begin(); it != m_mapTempVarInfo.end(); it++)
		{
			if (it->second.index >= vOut.vLocalVarType.size())
			{
				vOut.vLocalVarType.resize(it->second.index + 1);
			}
			vOut.vLocalVarType[it->second.index] = it->second.nType;

		}
		vOut.nDefaultReturnType = nReturnType;
		vOut.nFunParamNums = nPramSize;
		//首先是参数的赋值

		for (unsigned int i = 0; i < vBodyCode.size(); i++)
		{
			if (vBodyCode[i])
			{
				if (vBodyCode[i]->MakeExeCode(vOut) == false)
				{
					return false;
				}
			}
		}

		return true;
	}

	void CFunICode::SetMaxRunState(int val)
	{
		if (MaxStateIndex < val)
		{
			MaxStateIndex = val;
		}
	}

	//int CFunICode::Run(CScriptExecBlock* pBlock)
	//{
	//	pBlock->RegisterRunState(MaxStateIndex + 1);
	//	int& state = pBlock->GetRunState(m_nRunStateIndex);
	//	if (state == 0)
	//	{
	//		//注册变量
	//		auto it = m_mapTempVarInfo.begin();
	//		for (; it != m_mapTempVarInfo.end(); it++)
	//		{
	//			pBlock->RegisterLoaclVar(it->second.index, it->second.nType);
	//		}
	//		for (int i = 0; i < nPramSize; i++)
	//		{
	//			CBaseVar *pLoaclVar = pBlock->GetLoaclVar(i);
	//			CBaseVar* pVar = nullptr;
	//			STACK_GET_INDEX(pBlock->registerStack, pVar, i);
	//			AssignVar(pLoaclVar, pVar);
	//		}
	//		STACK_POP_FRONT(pBlock->registerStack, nPramSize);
	//		state++;
	//	}
	//	while (state > 0 && state <= vBodyCode.size())
	//	{
	//		int result = vBodyCode[state - 1]->Run(pBlock);
	//		
	//		if (result != CScriptExecBlock::ERESULT_CONTINUE)
	//		{
	//			return result;
	//		}
	//		state++;
	//	}

	//	state = 0;
	//	pBlock->RevertRunState(m_nRunStateIndex);
	//	return CScriptExecBlock::ERESULT_END;
	//}

	bool CBlockICode::DefineTempVar(int type, std::string VarName, CScriptCompiler* pCompiler)
	{
		if (m_mapTempVarInfo.find(VarName) != m_mapTempVarInfo.end())
			return false;
		VarInfo& info = m_mapTempVarInfo[VarName];
		info.nType = type;
		info.index = pCompiler->AddTempVarIndex();
		return true;
	}

	bool CBlockICode::CheckMyTempVar(std::string varName)
	{
		if (m_mapTempVarInfo.find(varName) != m_mapTempVarInfo.end())
		{
			return true;
		}
		return false;
	}


	VarInfo* CBlockICode::GetTempVarInfo(const char* pVarName)
	{
		auto it = m_mapTempVarInfo.find(pVarName);
		if (it != m_mapTempVarInfo.end())
		{
			return &it->second;
		}
		if (GetFather())
		{
			return GetFather()->GetTempVarInfo(pVarName);
		}
		return nullptr;
	}

	bool CBlockICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		if (pCompiler == nullptr)
		{
			return false;
		}

		GetNewWord(nextWord);
		if (nextWord.word != "{")
		{
			AddErrorInfo(0,
				nextWord.nSourceWordsIndex,
				ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}

		while (true)
		{
			//有定义
			if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_STATEMENT, this, 0))
			{
				RevertAll();
				return false;
			}

			GetWord(nextWord);
			if (nextWord.word == "}")
			{
				break;
			}
			else
			{
				RevertOne();
			}
		}

		return true;
	}

	void CBlockICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (pCode)
			m_vICode.push_back(pCode);
		CBaseICode::AddICode(nType, pCode);
	}

	bool CBlockICode::MakeExeCode(CExeCodeData& vOut)
	{
		for (auto it = m_vICode.begin(); it != m_vICode.end(); it++)
		{
			bool bSuccess = false;
			CBaseICode* pICode = *it;
			if (pICode != nullptr)
			{
				bSuccess = pICode->MakeExeCode(vOut);
			}
			if (!bSuccess)
			{
				return false;
			}
		}
		return true;
	}

	//int CBlockICode::Run(CScriptExecBlock* pBlock)
	//{
	//	int& state = pBlock->GetRunState(m_nRunStateIndex);
	//	if (state == 0)
	//	{
	//		//注册变量
	//		auto it = m_mapTempVarInfo.begin();
	//		for (; it != m_mapTempVarInfo.end(); it++)
	//		{
	//			pBlock->RegisterLoaclVar(it->second.index, it->second.nType);
	//		}

	//		state++;
	//	}
	//	while (state > 0 && state <= m_vICode.size())
	//	{
	//		int result = m_vICode[state - 1]->Run(pBlock);

	//		if (result != CScriptExecBlock::ERESULT_CONTINUE)
	//		{
	//			return result;
	//		}
	//		state++;
	//	}

	//	state = 0;
	//	pBlock->RevertRunState(m_nRunStateIndex);
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}

	CLoadVarICode::~CLoadVarICode()
	{
		if (m_pConst)
		{
			delete m_pConst;
		}
	}

	bool CLoadVarICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		GetNewWord(nextWord);
		if (nextWord.nFlag == E_WORD_FLAG_STRING)
		{
			CStringVar* pStrVar = new CStringVar();
			pStrVar->Set(nextWord.word);
			m_pConst = pStrVar;
		}
		else
		{
			//检查是否是临时变量
			VarInfo* pInfo = GetTempVarInfo(nextWord.word.c_str());
			if (pInfo != nullptr)
			{
				nLoadType = E_VAR_SCOPE_LOACL;
				VarIndex = pInfo->index;
				nResultVarType = pInfo->nType;
			}
			else
			{
				//检查是否是全局变量
				int nGVarIndex = CScriptGlobalVarMgr::GetInstance()->GetIndex(nextWord.word);
				if (nGVarIndex != -1)
				{
					const CBaseVar* pGVar = CScriptGlobalVarMgr::GetInstance()->Get(nGVarIndex);
					if (pGVar)
					{
						nResultVarType = pGVar->GetType();
					}
					CScriptGlobalVarMgr::GetInstance()->Revert(nGVarIndex);
					nLoadType = E_VAR_SCOPE_GLOBAL;
					VarIndex = nGVarIndex;
				}
				else
				{
					if (nextWord.word == "nullptr")
					{
						CPointVar* pVar = new CPointVar();
						m_pConst = pVar;
					}
					else
					{
						bool isFloat = false;
						for (unsigned int i = 0; i < nextWord.word.size(); i++)
						{
							if ((nextWord.word[i] >= '0' && nextWord.word[i] <= '9'))
							{
								//是数字
							}
							else if (nextWord.word[i] == '.')
							{
								//是点
								isFloat = true;
							}
						}
						if (isFloat)
						{
							CFloatVar* pFloatVar = new CFloatVar();
							pFloatVar->Set(nextWord.word);
							m_pConst = pFloatVar;
						}
						else
						{
							CIntVar* pIntVar = new CIntVar();
							pIntVar->Set(nextWord.word);
							m_pConst = pIntVar;
						}
					}
				}
			}
		}
		return true;
	}

	bool CLoadVarICode::MakeExeCode(CExeCodeData& vOut)
	{
		bool bResult = true;
		if (m_pConst)
		{
			nResultVarType = m_pConst->GetType();
			nLoadType = E_VAR_SCOPE_CONST;
			VarIndex = vOut.GetConstVarIndex(m_pConst);
		}
		return bResult;
	}

	//int CLoadVarICode::Run(CScriptExecBlock* pBlock)
	//{
	//	//向堆栈里压入一个数据

	//	switch (nLoadType)
	//	{
	//	case E_VAR_SCOPE_CONST:
	//	{
	//		STACK_PUSH_COPY(pBlock->registerStack, m_pConst);
	//	}
	//	break;
	//	case E_VAR_SCOPE_GLOBAL:
	//	{
	//		stGlobalVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(GlobalVarIndex);
	//		if (pGlobalVar)
	//		{
	//			STACK_PUSH_COPY(pBlock->registerStack, pGlobalVar->pVar);
	//			CScriptGlobalVarMgr::GetInstance()->Revert(pGlobalVar);
	//		}
	//	}
	//	break;
	//	case E_VAR_SCOPE_LOACL:
	//	{
	//		CBaseVar* pLoacllVar = pBlock->GetLoaclVar(LoaclVarIndex);
	//		if (pLoacllVar)
	//		{
	//			STACK_PUSH_COPY(pBlock->registerStack, pLoacllVar);
	//		}
	//	}
	//	break;

	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}

	bool CSaveVarICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		//此类并不直接参与编译,表达式树整理时按情况替换LoadVar
		//SignToPos();

		//GetNewWord(nextWord);
		//m_word = nextWord;
		return true;
	}
	void CSaveVarICode::AddICode(int nType, CBaseICode* pCode)
	{
		pRightOperand = pCode;
		CBaseICode::AddICode(0, pCode);
	}
	bool CSaveVarICode::MakeExeCode(CExeCodeData& vOut)
	{
		if (pRightOperand == nullptr)
		{
			return false;
		}

		pRightOperand->SetRegisterIndex(R_A);
		if (pRightOperand->MakeExeCode(vOut) == false)
		{
			return false;
		}
		CVoluationExeCode* pCode = CExeCodeMgr::GetInstance()->New<CVoluationExeCode>(m_unBeginSoureIndex);

		if (pRightOperand->GetType() == E_I_CODE_LOADVAR)
		{
			CLoadVarICode* pLoadCode = (CLoadVarICode*)pRightOperand;
			pCode->param.nType = pLoadCode->nLoadType;
			pCode->param.dwPos = pLoadCode->VarIndex;
		}
		else
		{
			if (pRightOperand->GetRegisterIndex() < R_SIZE)
			{
				pCode->param.nType = E_VAR_SCOPE_REGISTER;
				pCode->param.dwPos = pRightOperand->GetRegisterIndex();
			}
			else
			{
				pCode->param.nType = E_VAR_SCOPE_REGISTER_STACK;
			}
		}
		pCode->cType = nSaveType;
		pCode->dwPos = VarIndex;
		vOut.AddCode(pCode);
		return true;
	}
	//int CSaveVarICode::Run(CScriptExecBlock* pBlock)
	//{
	//	int& state = pBlock->GetRunState(m_nRunStateIndex);
	//	if (state == 0)
	//	{
	//		int result = pRightOperand->Run(pBlock);

	//		if (result != CScriptExecBlock::ERESULT_CONTINUE)
	//		{
	//			return result;
	//		}
	//		state++;
	//	}
	//	//从堆栈里取一个数据
	//	CBaseVar* pVar = nullptr;
	//	STACK_POP(pBlock->registerStack, pVar);
	//	if (nSaveType == E_VAR_SCOPE_GLOBAL)
	//	{
	//		stGlobalVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(GlobalVarIndex);
	//		if (pGlobalVar)
	//		{
	//			AssignVar(pGlobalVar->pVar, pVar);
	//			CScriptGlobalVarMgr::GetInstance()->Revert(pGlobalVar);
	//		}
	//	}
	//	else if (nSaveType == E_VAR_SCOPE_LOACL)
	//	{
	//		CBaseVar* pLoacllVar = pBlock->GetLoaclVar(LoaclVarIndex);
	//		AssignVar(pLoacllVar, pVar);
	//	}
	//	SCRIPTVAR_RELEASE(pVar);
	//	state = 0;
	//	pBlock->RevertRunState(m_nRunStateIndex);
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}

	bool CGetClassParamICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		GetNewWord(nextWord);
		strClassVarName = nextWord.word;
		GetWord(nextWord);
		if (nextWord.word != "->" && nextWord.word != ".")
		{
			AddErrorInfo(0,
				nextWord.nSourceWordsIndex,
				ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		strParamName = nextWord.word;

		int nClassType = 0;
		VarInfo* pInfo = GetTempVarInfo(strClassVarName.c_str());
		if (pInfo != nullptr)
		{
			isGlobal = false;
			varIndex = pInfo->index;
			nClassType = pInfo->nType;
		}
		else
		{
			//检查是否是全局变量
			int index = CScriptGlobalVarMgr::GetInstance()->GetIndex(strClassVarName);
			if (index != -1)
			{
				isGlobal = true;
				varIndex = index;
				const CBaseVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(index);
				if (pGlobalVar)
					nClassType = pGlobalVar->GetType();
				CScriptGlobalVarMgr::GetInstance()->Revert(index);
			}
			else
			{
				RevertAll();
				//TODO 提示错误
				return false;
			}
		}
		if (nClassType > 0)
		{
			CBaseScriptClassInfo* pInfo = CScriptVarTypeMgr::GetInstance()->GetTypeInfo(nClassType);
			if (pInfo)
			{
				auto it = pInfo->mapDicString2ParamInfo.find(strParamName);
				if (it != pInfo->mapDicString2ParamInfo.end())
				{
					nParamIndex = it->second.m_index;
					nResultVarType = it->second.nVarType;
				}
				else
				{
					RevertAll();
					//TODO 提示错误
					return false;
				}
			}
			else
			{
				RevertAll();
				//TODO 提示错误
				return false;
			}
		}
		else
		{
			RevertAll();
			//TODO 提示错误
			return false;
		}
		return true;
	}
	bool CGetClassParamICode::MakeExeCode(CExeCodeData& vOut)
	{
		CGetClassParamExeCode* pGetCode = CExeCodeMgr::GetInstance()->New<CGetClassParamExeCode>(m_unBeginSoureIndex);
		pGetCode->registerIndex = this->GetRegisterIndex();
		if (isGlobal)
			pGetCode->param.nType = E_VAR_SCOPE_GLOBAL;
		else
			pGetCode->param.nType = E_VAR_SCOPE_LOACL;
		pGetCode->param.dwPos = varIndex;
		pGetCode->dwPos = nParamIndex;
		vOut.AddCode(pGetCode);
		return true;
	}


	void CSetClassParamICode::AddICode(int nType, CBaseICode* pCode)
	{
		pRightOperand = pCode;
		CBaseICode::AddICode(0, pCode);
	}
	bool CSetClassParamICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		//此类并不直接参与编译,表达式树整理时按情况替换GetClassParamICode
		return true;
	}
	bool CSetClassParamICode::MakeExeCode(CExeCodeData& vOut)
	{
		CSetClassParamExeCode* pSetCode = CExeCodeMgr::GetInstance()->New<CSetClassParamExeCode>(m_unBeginSoureIndex);
		pRightOperand->SetRegisterIndex(R_A);
		if (pRightOperand->MakeExeCode(vOut) == false)
		{
			return false;
		}
		if (pRightOperand->GetType() == E_I_CODE_LOADVAR)
		{
			CLoadVarICode* pLoadCode = (CLoadVarICode*)pRightOperand;
			pSetCode->valInfo.nType = pLoadCode->nLoadType;
			pSetCode->valInfo.dwPos = pLoadCode->VarIndex;
		}
		else
		{
			if (pRightOperand->GetRegisterIndex() < R_SIZE)
			{
				pSetCode->valInfo.nType = E_VAR_SCOPE_REGISTER;
				pSetCode->valInfo.dwPos = pRightOperand->GetRegisterIndex();
			}
			else
			{
				pSetCode->valInfo.nType = E_VAR_SCOPE_REGISTER_STACK;
			}
		}

		if (isGlobal)
			pSetCode->param.nType = E_VAR_SCOPE_GLOBAL;
		else
			pSetCode->param.nType = E_VAR_SCOPE_LOACL;
		pSetCode->param.dwPos = varIndex;
		pSetCode->dwPos = nParamIndex;
		vOut.AddCode(pSetCode);
		return true;
	}

	void CMinusICode::AddICode(int nType, CBaseICode* pCode)
	{
		pRightOperand = pCode;
		CBaseICode::AddICode(0, pCode);
	}
	bool CMinusICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		GetNewWord(nextWord);
		if (nextWord.word != "-")
		{
			RevertAll();
			return false;
		}
		if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_MEMBER, this, 0))
		{
			AddErrorInfo(0,
				nextWord.nSourceWordsIndex,
				ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}
		return true;
	}
	bool CMinusICode::MakeExeCode(CExeCodeData& vOut)
	{
		pRightOperand->SetRegisterIndex(R_A);
		if (pRightOperand->MakeExeCode(vOut) == false)
		{
			return false;
		}
		auto group = CScriptVarOperatorMgr::GetInstance()->GetUnaryOperGroup(ECODE_MINUS);
		if (group == nullptr)
		{
			//TODO 报错
			return false;
		}
		nResultVarType = pRightOperand->GetResultVarType();
		if (pRightOperand->GetResultVarType() == -1)
		{
			CUnaryOperGroupExeCode* pCode = CExeCodeMgr::GetInstance()->New<CUnaryOperGroupExeCode>(m_unBeginSoureIndex);
			pCode->registerIndex = this->GetRegisterIndex();
			if (pRightOperand->GetType() == E_I_CODE_LOADVAR)
			{
				CLoadVarICode* pLoadCode = (CLoadVarICode*)pRightOperand;
				pCode->param.nType = pLoadCode->nLoadType;
				pCode->param.dwPos = pLoadCode->VarIndex;
			}
			else
			{
				if (pRightOperand->GetRegisterIndex() < R_SIZE)
				{
					pCode->param.nType = E_VAR_SCOPE_REGISTER;
					pCode->param.dwPos = pRightOperand->GetRegisterIndex();
				}
				else
				{
					pCode->param.nType = E_VAR_SCOPE_REGISTER_STACK;
				}
			}

			pCode->operGroup = group;

			vOut.AddCode(pCode);			
		}
		else
		{
			CUnaryOperExeCode* pCode = CExeCodeMgr::GetInstance()->New<CUnaryOperExeCode>(m_unBeginSoureIndex);
			pCode->registerIndex = this->GetRegisterIndex();
			if (pRightOperand->GetType() == E_I_CODE_LOADVAR)
			{
				CLoadVarICode* pLoadCode = (CLoadVarICode*)pRightOperand;
				pCode->param.nType = pLoadCode->nLoadType;
				pCode->param.dwPos = pLoadCode->VarIndex;
			}
			else
			{
				if (pRightOperand->GetRegisterIndex() < R_SIZE)
				{
					pCode->param.nType = E_VAR_SCOPE_REGISTER;
					pCode->param.dwPos = pRightOperand->GetRegisterIndex();
				}
				else
				{
					pCode->param.nType = E_VAR_SCOPE_REGISTER_STACK;
				}
			}
			auto it = group->find(pRightOperand->GetResultVarType());
			if (it != group->end())
			{
				pCode->oper = it->second;
			}
			else
			{
				return false;
			}

			vOut.AddCode(pCode);
		}
		return true;
	}

	void COperatorICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (pCode && pCode->GetType() == E_I_CODE_OPERATOR)
		{
			COperatorICode* pOperCode = (COperatorICode*)pCode;
			//pOperCode->nOperatorFlag = nType;
		}
		if (nType == E_LEFT_OPERAND)
		{
			pLeftOperand = pCode;

		}
		else if (nType == E_RIGHT_OPERAND)
		{
			pRightOperand = pCode;
		}
		else if (pCode)
		{
			m_OtherOperand.push_back(pCode);
		}
		CBaseICode::AddICode(nType, pCode);
	}
	CBaseICode* COperatorICode::GetICode(int nType, int index)
	{
		if (nType == E_LEFT_OPERAND)
		{
			return pLeftOperand;
		}
		else if (nType == E_RIGHT_OPERAND)
		{
			return pRightOperand;
		}
		else if (nType == E_OTHER_OPERAND)
		{
			if ((unsigned int)index < m_OtherOperand.size())
			{
				return m_OtherOperand[index];
			}
		}
		return nullptr;
	}
	bool COperatorICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		if (pCompiler == nullptr)
		{
			return false;
		}

		GetNewWord(nextWord);
		int nPRI = CScriptCodeLoader::GetInstance()->GetSignPRI(nextWord.word);
		if (nPRI == 0xffff)
		{
			AddErrorInfo(0,
				nextWord.nSourceWordsIndex,
				ERROR_NO_PRI_SIGN);
			RevertAll();
			return false;
		}
		strOperator = nextWord.word;
		nPriorityLv = nPRI;
		nOperatorCode = CScriptCodeLoader::GetInstance()->GetWordKey(strOperator);
		pOperGroup = CScriptVarOperatorMgr::GetInstance()->GetBinaryOperGroup(nOperatorCode);
		if (pOperGroup == nullptr)
		{
			//TODO 提示错误
			AddErrorInfo(2,
				nextWord.nSourceWordsIndex,
				ERROR_NO_OPER_SIGN);
			RevertAll();
			return false;
		}
		return true;
	}
	bool COperatorICode::MakeExeCode(CExeCodeData& vOut)
	{
		pLeftOperand->SetRegisterIndex(R_A);

		pRightOperand->SetRegisterIndex(R_B);
		if (pRightOperand->GetType() != E_I_CODE_LOADVAR)
		{
			pLeftOperand->SetRegisterIndex(0xffffffff);
		}
		if (pLeftOperand->MakeExeCode(vOut) == false)
		{
			return false;
		}
		if (pRightOperand->MakeExeCode(vOut) == false)
		{
			return false;
		}
		CBaseExeCode *pExeCode = nullptr;
		nResultVarType = pLeftOperand->GetResultVarType();
		if (pLeftOperand->GetResultVarType() == -1 || pRightOperand->GetResultVarType() == -1)
		{
			CBinaryOperGroupExeCode* pCode = CExeCodeMgr::GetInstance()->New<CBinaryOperGroupExeCode>(m_unBeginSoureIndex);
			pCode->registerIndex = this->GetRegisterIndex();
			nResultVarType = pLeftOperand->GetResultVarType();
			if (pRightOperand->GetType() == E_I_CODE_LOADVAR)
			{
				CLoadVarICode* pLoadCode = (CLoadVarICode*)pRightOperand;
				pCode->rightParam.nType = pLoadCode->nLoadType;
				pCode->rightParam.dwPos = pLoadCode->VarIndex;
			}
			else
			{
				if (pRightOperand->GetRegisterIndex() < R_SIZE)
				{
					pCode->rightParam.nType = E_VAR_SCOPE_REGISTER;
					pCode->rightParam.dwPos = pRightOperand->GetRegisterIndex();
				}
				else
				{
					pCode->rightParam.nType = E_VAR_SCOPE_REGISTER_STACK;
				}
			}

			if (pLeftOperand->GetType() == E_I_CODE_LOADVAR)
			{
				CLoadVarICode* pLoadCode = (CLoadVarICode*)pLeftOperand;
				pCode->leftParam.nType = pLoadCode->nLoadType;
				pCode->leftParam.dwPos = pLoadCode->VarIndex;
			}
			else
			{
				if (pLeftOperand->GetRegisterIndex() < R_SIZE)
				{
					pCode->leftParam.nType = E_VAR_SCOPE_REGISTER;
					pCode->leftParam.dwPos = pLeftOperand->GetRegisterIndex();
				}
				else
				{
					pCode->leftParam.nType = E_VAR_SCOPE_REGISTER_STACK;
				}
			}

			pCode->operGroup = pOperGroup;

			pExeCode = pCode;
		}
		else
		{
			CBinaryOperExeCode* pCode = CExeCodeMgr::GetInstance()->New<CBinaryOperExeCode>(m_unBeginSoureIndex);
			pCode->registerIndex = this->GetRegisterIndex();
			nResultVarType = pLeftOperand->GetResultVarType();

			if (pRightOperand->GetType() == E_I_CODE_LOADVAR)
			{
				CLoadVarICode* pLoadCode = (CLoadVarICode*)pRightOperand;
				pCode->rightParam.nType = pLoadCode->nLoadType;
				pCode->rightParam.dwPos = pLoadCode->VarIndex;
			}
			else
			{
				pLeftOperand->SetRegisterIndex(0xffffffff);
				if (pRightOperand->GetRegisterIndex() < R_SIZE)
				{
					pCode->rightParam.nType = E_VAR_SCOPE_REGISTER;
					pCode->rightParam.dwPos = pRightOperand->GetRegisterIndex();
				}
				else
				{
					pCode->rightParam.nType = E_VAR_SCOPE_REGISTER_STACK;
				}
			}

			if (pLeftOperand->GetType() == E_I_CODE_LOADVAR)
			{
				CLoadVarICode* pLoadCode = (CLoadVarICode*)pLeftOperand;
				pCode->leftParam.nType = pLoadCode->nLoadType;
				pCode->leftParam.dwPos = pLoadCode->VarIndex;
			}
			else
			{
				if (pLeftOperand->GetRegisterIndex() < R_SIZE)
				{
					pCode->leftParam.nType = E_VAR_SCOPE_REGISTER;
					pCode->leftParam.dwPos = pLeftOperand->GetRegisterIndex();
				}
				else
				{
					pCode->leftParam.nType = E_VAR_SCOPE_REGISTER_STACK;
				}
			}

			union {
				struct
				{
					int type1;
					int type2;
				};
				__int64 index;
			} trans;
			trans.type1 = pLeftOperand->GetResultVarType();
			trans.type2 = pRightOperand->GetResultVarType();
			auto it = pOperGroup->find(trans.index);
			if (it != pOperGroup->end())
			{
				pCode->oper = it->second;
			}
			else
			{
				return false;
			}
			pExeCode = pCode;
		}
		if (pExeCode == nullptr)
		{
			return false;
		}
		vOut.AddCode(pExeCode);
		return true;
	}

	void CCallFunICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (nType == E_PARAM)
		{
			vParams.push_back(pCode);
		}
		CBaseICode::AddICode(nType, pCode);
	}
	bool CCallFunICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		if (pCompiler == nullptr)
		{
			return false;
		}

		GetNewWord(nextWord);
		strFunName = nextWord.word;
		GetWord(nextWord);
		if (nextWord.word != "(")
		{
			AddErrorInfo(0, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		if (nextWord.word != ")")
		{
			RevertOne();
			while (true)
			{
				if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_EXPRESSION, this, E_PARAM))
				{
					AddErrorInfo(1, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
					RevertAll();
					return false;
				}
				GetWord(nextWord);
				if (nextWord.word == ",")
				{
					continue;
				}
				else if (nextWord.word == ")")
				{
					break;
				}
				else
				{
					AddErrorInfo(1,
						nextWord.nSourceWordsIndex,
						ERROR_FORMAT_NOT);
					RevertAll();
					return false;
				}
			}
		}

		return true;
	}

	bool CCallFunICode::MakeExeCode(CExeCodeData& vOut)
	{
		std::vector<CExeParamInfo> params;
		for (int i = 0; i < vParams.size(); i++)
		{
			CExeParamInfo info;
			CBaseICode* pCode = vParams[i];
			if (pCode)
			{
				if (pCode->MakeExeCode(vOut) == false)
				{
					return false;
				}
				if (pCode->GetType() == E_I_CODE_EXPRESSION)
				{
					CExpressionICode* pExpressCode = (CExpressionICode*)pCode;
					if (pExpressCode->pOperandCode && pExpressCode->pOperandCode->GetType() == E_I_CODE_LOADVAR)
					{
						CLoadVarICode* pLoadCode = (CLoadVarICode*)pExpressCode->pOperandCode;
						info.nType = pLoadCode->nLoadType;
						info.dwPos = pLoadCode->VarIndex;
					}
					else
					{
						info.nType = E_VAR_SCOPE_REGISTER_STACK;
					}
				}

			}
			params.push_back(info);
		}
		nCallBack = CScriptCallBackFunion::GetFunIndex(strFunName);
		if (nCallBack >= 0)
		{
			CCallBackExeCode* pFunCode = CExeCodeMgr::GetInstance()->New<CCallBackExeCode>(m_unBeginSoureIndex);
			pFunCode->registerIndex = this->GetRegisterIndex();
			pFunCode->unFunIndex = nCallBack;
			pFunCode->params = params;
			vOut.AddCode(pFunCode);
		}
		else
		{
			nFunIndex = CScriptCodeLoader::GetInstance()->GetCodeIndex(strFunName.c_str());
			if (nFunIndex < 0)
			{
				return false;
			}
			CCallScriptExeCode* pFunCode = CExeCodeMgr::GetInstance()->New<CCallScriptExeCode>(m_unBeginSoureIndex);
			pFunCode->registerIndex = this->GetRegisterIndex();
			pFunCode->unFunIndex = nFunIndex;
			pFunCode->params = params;
			vOut.AddCode(pFunCode);
		}
		return true;
	}


	void CCallClassFunICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (nType == E_PARAM)
		{
			vParams.push_back(pCode);
		}
		CBaseICode::AddICode(nType, pCode);
	}

	bool CCallClassFunICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		if (pCompiler == nullptr)
		{
			return false;
		}

		GetNewWord(nextWord);
		strClassVarName = nextWord.word;

		GetWord(nextWord);
		if (nextWord.word != "->" && nextWord.word != ".")
		{
			AddErrorInfo(0, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}
		int nClassType = 0;
		VarInfo* pInfo = GetTempVarInfo(strClassVarName.c_str());
		if (pInfo != nullptr)
		{
			isGlobal = false;
			varIndex = pInfo->index;
			nClassType = pInfo->nType;
		}
		else
		{
			//检查是否是全局变量
			int index = CScriptGlobalVarMgr::GetInstance()->GetIndex(strClassVarName);
			if (index != -1)
			{
				isGlobal = true;
				varIndex = index;
				const CBaseVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(index);
				if (pGlobalVar)
					nClassType = pGlobalVar->GetType();
				CScriptGlobalVarMgr::GetInstance()->Revert(index);
			}
			else
			{
				RevertAll();
				//TODO 提示错误
				return false;
			}
		}

		GetWord(nextWord);
		strFunName = nextWord.word;
		if (nClassType > 0)
		{
			CBaseScriptClassInfo* pInfo = CScriptVarTypeMgr::GetInstance()->GetTypeInfo(nClassType);
			if (pInfo)
			{
				auto it = pInfo->mapDicFunString2Index.find(strFunName);
				if (it != pInfo->mapDicFunString2Index.end())
				{
					nFunIndex = it->second;
				}
			}
		}

		GetWord(nextWord);
		if (nextWord.word != "(")
		{
			AddErrorInfo(1, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		if (nextWord.word != ")")
		{
			RevertOne();
			while (true)
			{
				if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_EXPRESSION, this, E_PARAM))
				{
					AddErrorInfo(2, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
					RevertAll();
					return false;
				}
				GetWord(nextWord);
				if (nextWord.word == ",")
				{
					continue;
				}
				else if (nextWord.word == ")")
				{
					break;
				}
				else
				{
					AddErrorInfo(2,
						nextWord.nSourceWordsIndex,
						ERROR_FORMAT_NOT);
					RevertAll();
					return false;
				}
			}
		}
		return true;
	}

	bool CCallClassFunICode::MakeExeCode(CExeCodeData& vOut)
	{
		CCallClassFunExeCode* pCallCode = CExeCodeMgr::GetInstance()->New<CCallClassFunExeCode>(m_unBeginSoureIndex);
		pCallCode->registerIndex = this->GetRegisterIndex();
		if (isGlobal)
			pCallCode->object.nType = E_VAR_SCOPE_GLOBAL;
		else
			pCallCode->object.nType = E_VAR_SCOPE_LOACL;
		pCallCode->object.dwPos = varIndex;
		pCallCode->funIndex = nFunIndex;
		for (int i = 0; i < vParams.size(); i++)
		{
			CExeParamInfo info;
			CBaseICode* pCode = vParams[i];
			if (pCode)
			{
				pCode->MakeExeCode(vOut);
				if (pCode->GetType() == E_I_CODE_EXPRESSION)
				{
					CExpressionICode* pExpressCode = (CExpressionICode*)pCode;
					if (pExpressCode->pOperandCode && pExpressCode->pOperandCode->GetType() == E_I_CODE_LOADVAR)
					{
						CLoadVarICode* pLoadCode = (CLoadVarICode*)pExpressCode->pOperandCode;
						info.nType = pLoadCode->nLoadType;
						info.dwPos = pLoadCode->VarIndex;
					}
					else
					{
						info.nType = E_VAR_SCOPE_REGISTER_STACK;
					}
				}
			}
			pCallCode->params.push_back(info);
		}
		vOut.AddCode(pCallCode);
		return true;
	}

	//int CCallClassFunICode::RunClassFun(CScriptExecBlock* pBlock, CScriptPointInterface* pPoint, int funIndex)
	//{
	//	int result = CScriptExecBlock::ERESULT_ERROR;
	//	unsigned int nBegin = pBlock->registerStack.nIndex - vParams.size();
	//	CACHE_NEW(CScriptCallState, pCallState, pBlock->m_pMaster);
	//	if (pCallState)
	//	{
	//		STACK_MOVE_ALL_BACK(pCallState->m_stackRegister, pBlock->registerStack, nBegin);
	//		switch (pPoint->RunFun(funIndex, pCallState))
	//		{
	//		case ECALLBACK_ERROR:
	//			result = CScriptExecBlock::ERESULT_ERROR;
	//			break;
	//		case ECALLBACK_WAITING:
	//			result = CScriptExecBlock::ERESULT_WAITING;
	//			break;
	//		case ECALLBACK_NEXTCONTINUE:
	//			result = CScriptExecBlock::ERESULT_NEXTCONTINUE;
	//			break;
	//		case ECALLBACK_FINISH:
	//			//执行完将结果放入寄存器
	//			if (pCallState->GetResult())
	//			{
	//				STACK_PUSH_COPY(pBlock->registerStack, pCallState->GetResult());
	//			}
	//			result = CScriptExecBlock::ERESULT_CONTINUE;
	//			break;
	//		}
	//	}
	//	CACHE_DELETE(pCallState);
	//	return result;
	//}

	//int CCallClassFunICode::Run(CScriptExecBlock* pBlock)
	//{
	//	int& state = pBlock->GetRunState(m_nRunStateIndex);

	//	for (int i = state; i < vParams.size(); i++)
	//	{
	//		CBaseICode* pCode = vParams[i];
	//		if (pCode)
	//		{
	//			int result = pCode->Run(pBlock);
	//			if (result != CScriptExecBlock::ERESULT_CONTINUE)
	//			{
	//				return result;
	//			}
	//		}
	//		state++;
	//	}
	//	if (state == vParams.size())
	//	{
	//		state++;
	//		if (pBlock->registerStack.nIndex < vParams.size())
	//		{
	//			//TODO 输出错误
	//			return CScriptExecBlock::ERESULT_ERROR;
	//		}
	//		int result = CScriptExecBlock::ERESULT_CONTINUE;
	//		int nFunIndexRunTime = nFunIndex;

	//		if (isGlobal)
	//		{
	//			stGlobalVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(varIndex);
	//			if (pGlobalVar)
	//			{
	//				CPointVar* pPoint = dynamic_cast<CPointVar*>(pGlobalVar->pVar);
	//				if (pPoint)
	//				{
	//					if (pPoint->ToPoint())
	//					{
	//						if (nFunIndexRunTime < 0)
	//						{
	//							nFunIndexRunTime = pPoint->ToPoint()->GetClassFunIndex(strFunName);
	//						}
	//						result = RunClassFun(pBlock, pPoint->ToPoint(), nFunIndexRunTime);
	//					}
	//					else
	//					{
	//						result = CScriptExecBlock::ERESULT_ERROR;
	//						//TODO 输出错误信息
	//					}
	//				}
	//				else
	//				{
	//					result = CScriptExecBlock::ERESULT_ERROR;
	//					//TODO 输出错误信息
	//				}
	//				CScriptGlobalVarMgr::GetInstance()->Revert(pGlobalVar);
	//			}
	//			else
	//			{
	//				result = CScriptExecBlock::ERESULT_ERROR;
	//				//TODO 输出错误信息
	//			}
	//		}
	//		else
	//		{
	//			CBaseVar* pLoacllVar = pBlock->GetLoaclVar(varIndex);
	//			if (pLoacllVar)
	//			{
	//				CPointVar* pPoint = dynamic_cast<CPointVar*>(pLoacllVar);
	//				if (pPoint)
	//				{
	//					if (pPoint->ToPoint())
	//					{
	//						if (nFunIndexRunTime < 0)
	//						{
	//							nFunIndexRunTime = pPoint->ToPoint()->GetClassFunIndex(strFunName);
	//						}
	//						result = RunClassFun(pBlock, pPoint->ToPoint(), nFunIndexRunTime);
	//					}
	//					else
	//					{
	//						result = CScriptExecBlock::ERESULT_ERROR;
	//						//TODO 输出错误信息
	//					}
	//				}
	//				else
	//				{
	//					result = CScriptExecBlock::ERESULT_ERROR;
	//					//TODO 输出错误信息
	//				}
	//			}
	//			else
	//			{
	//				result = CScriptExecBlock::ERESULT_ERROR;
	//				//TODO 输出错误信息
	//			}
	//		}
	//		if (result != CScriptExecBlock::ERESULT_CONTINUE)
	//			return result;
	//	}
	//	state = 0;
	//	pBlock->RevertRunState(m_nRunStateIndex);
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}


	void CSentenceICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (pCode)
		{
			vData.push_back(pCode);
		}
	}

	bool CSentenceICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		if (pCompiler == nullptr)
		{
			return false;
		}
		GetNewWord(nextWord);
		if (nextWord.word != ";")
		{
			RevertOne();
			while (true)
			{
				if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_EXPRESSION, this, 0))
				{
					AddErrorInfo(0, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
					RevertAll();
					return false;
				}
				GetWord(nextWord);
				if (nextWord.word == ";")
				{
					break;
				}
				else if (nextWord.word == ",")
				{
					continue;
				}
				else
				{
					AddErrorInfo(0, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
					RevertAll();
					return false;
				}
			}
		}
		return true;
	}

	bool CSentenceICode::MakeExeCode(CExeCodeData& vOut)
	{
		for (int i = 0; i < vData.size(); i++)
		{
			CBaseICode* pCode = vData[i];
			if (pCode)
			{
				if (pCode->MakeExeCode(vOut) == false)
				{
					return false;
				}
			}
		}
		return true;
	}

	//int CSentenceICode::Run(CScriptExecBlock* pBlock)
	//{
	//	int& state = pBlock->GetRunState(m_nRunStateIndex);
	//	for (int i = state; i < vData.size(); i++)
	//	{
	//		CBaseICode* pCode = vData[i];
	//		if (pCode)
	//		{
	//			int result = pCode->Run(pBlock);
	//			if (result != CScriptExecBlock::ERESULT_CONTINUE)
	//			{
	//				return result;
	//			}
	//		}
	//		state++;
	//	}
	//	state = 0;
	//	pBlock->RevertRunState(m_nRunStateIndex);
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}

	//bool CExpressionICode::MakeExeCode(tagCodeData& vOut)
	//{
	//	if (m_pRoot)
	//	{
	//		return m_pRoot->MakeExeCode(vOut);
	//	}
	//	else if (pOperandCode)
	//	{
	//		return pOperandCode->MakeExeCode(vOut);
	//	}
	//	return true;
	//}
	void CExpressionICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (pCode == nullptr)
		{
			return;
		}
		if (pCode->GetType() == E_I_CODE_OPERATOR)
		{
			COperatorICode* pCurOperCode = (COperatorICode*)pCode;
			if (m_pRoot == nullptr)
			{
				m_pRoot = pCurOperCode;
				if (pOperandCode)
				{
					pCurOperCode->AddICode(COperatorICode::E_LEFT_OPERAND, pOperandCode);
					pOperandCode = nullptr;
				}
			}
			else
			{
				COperatorICode* pCurNode = m_pRoot;
				while (pCurNode)
				{
					if (pCurNode->nPriorityLv >= pCurOperCode->nPriorityLv)
					{
						break;
					}
					COperatorICode* pNode = dynamic_cast<COperatorICode*>(pCurNode->GetICode(COperatorICode::E_RIGHT_OPERAND, 0));
					if (pNode == nullptr)
					{
						break;
					}
					pCurNode = pNode;
				}

				if (pCurNode->nPriorityLv >= pCurOperCode->nPriorityLv)
				{
					if (pCurNode == m_pRoot)
					{
						m_pRoot = pCurOperCode;
					}

					COperatorICode* pFather = dynamic_cast<COperatorICode*>(pCurNode->GetFather());
					if (pFather)
					{
						pFather->AddICode(COperatorICode::E_RIGHT_OPERAND, pCurOperCode);
					}

					pCurOperCode->AddICode(COperatorICode::E_LEFT_OPERAND, pCurNode);
				}
				else
				{
					auto pOperand = pCurNode->GetICode(COperatorICode::E_RIGHT_OPERAND, 0);
					pCurOperCode->AddICode(COperatorICode::E_LEFT_OPERAND, pOperand);
					pCurNode->AddICode(COperatorICode::E_RIGHT_OPERAND, pCurOperCode);
				}
			}
		}
		else
		{
			COperatorICode* pCurNode = m_pRoot;
			while (pCurNode)
			{
				COperatorICode* pNode = dynamic_cast<COperatorICode*>(pCurNode->GetICode(COperatorICode::E_RIGHT_OPERAND, 0));
				if (pNode == nullptr)
				{
					break;
				}
				pCurNode = pNode;
			}
			if (pCurNode)
			{
				pCurNode->AddICode(COperatorICode::E_RIGHT_OPERAND, pCode);
			}
			else
			{
				pOperandCode = pCode;
			}
		}
	}
	bool CExpressionICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		if (pCompiler == nullptr)
		{
			return false;
		}
		int nState = E_STATE_OPERAND;
		int nNeedOperand = 1;
		while (true)
		{
			CBaseICode* pCurICode = nullptr;
			if (nState == E_STATE_OPERATOR)
			{
				if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_OPERATOR, this, 0))
				{
					if (nNeedOperand <= 0)
					{
						break;
					}
					RevertAll();
					return false;
				}
				nNeedOperand++;
			}
			else if (nState == E_STATE_OPERAND)
			{
				if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_MEMBER, this, 0))
				{
					RevertAll();
					return false;
				}
				nNeedOperand--;
			}
			nState++;
			nState = nState % E_STATE_SIZE;
		}
		CheckOperatorTree((CBaseICode**)&m_pRoot, pCompiler);
		return true;
	}
	bool CExpressionICode::MakeExeCode(CExeCodeData& vOut)
	{
		if (m_pRoot)
		{
			m_pRoot->SetRegisterIndex(this->GetRegisterIndex());
			return m_pRoot->MakeExeCode(vOut);
		}
		else if (pOperandCode)
		{
			pOperandCode->SetRegisterIndex(this->GetRegisterIndex());
			return pOperandCode->MakeExeCode(vOut);
		}
		return false;
	}
	bool CExpressionICode::CheckOperatorTree(CBaseICode** pNode, CScriptCompiler* pCompiler)
	{
		if (pNode == nullptr)
			return false;
		COperatorICode* pOperNode = dynamic_cast<COperatorICode*>(*pNode);
		if (pOperNode == nullptr)
			return true;

		if (pOperNode->strOperator == "=")
		{
			if (pOperNode->pLeftOperand && pOperNode->pLeftOperand->GetType() == E_I_CODE_GET_CLASS_PARAM)
			{
				CICodeMgr<CSetClassParamICode> mgr;
				CGetClassParamICode* pLeftOperand = (CGetClassParamICode*)pOperNode->pLeftOperand;
				//设置类变量值
				CSetClassParamICode* pSetCode = (CSetClassParamICode*)mgr.New(pCompiler, pLeftOperand->m_unBeginSoureIndex);
				pSetCode->strClassVarName = pLeftOperand->strClassVarName;
				pSetCode->strParamName = pLeftOperand->strParamName;
				pSetCode->isGlobal = pLeftOperand->isGlobal;
				pSetCode->varIndex = pLeftOperand->varIndex;
				pSetCode->nParamIndex = pLeftOperand->nParamIndex;

				if (CheckOperatorTree(&pOperNode->pRightOperand, pCompiler) == false)
				{
					return false;
				}
				pSetCode->SetFather(pOperNode->GetFather());
				pSetCode->AddICode(0, pOperNode->pRightOperand);
				*pNode = pSetCode;
				return true;

			}
			else if (pOperNode->pLeftOperand && pOperNode->pLeftOperand->GetType() == E_I_CODE_LOADVAR && pOperNode->pRightOperand)
			{
				CICodeMgr<CSaveVarICode> mgr;
				CLoadVarICode* pLeftOperand = (CLoadVarICode*)pOperNode->pLeftOperand;
				CSaveVarICode* pSaveCode = (CSaveVarICode*)mgr.New(pCompiler, pLeftOperand->m_unBeginSoureIndex);

				pSaveCode->nSaveType = pLeftOperand->nLoadType;
				pSaveCode->VarIndex = pLeftOperand->VarIndex;

				if (CheckOperatorTree(&pOperNode->pRightOperand, pCompiler) == false)
				{
					return false;
				}
				pSaveCode->SetFather(pOperNode->GetFather());
				pSaveCode->AddICode(0, pOperNode->pRightOperand);
				*pNode = pSaveCode;
				return true;
			}
		}


		if (pOperNode->pLeftOperand && pOperNode->pLeftOperand->GetType() == E_I_CODE_OPERATOR)
		{
			if (CheckOperatorTree(&pOperNode->pLeftOperand, pCompiler) == false)
			{
				return false;
			}
		}
		if (pOperNode->pRightOperand && pOperNode->pRightOperand->GetType() == E_I_CODE_OPERATOR)
		{
			if (CheckOperatorTree(&pOperNode->pRightOperand, pCompiler) == false)
			{
				return false;
			}
		}
		return true;
	}
	//int CExpressionICode::Run(CScriptExecBlock* pBlock)
	//{
	//	if (m_pRoot)
	//	{
	//		return m_pRoot->Run(pBlock);
	//	}
	//	else if (pOperandCode)
	//	{
	//		return pOperandCode->Run(pBlock);
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//bool CIfICode::MakeExeCode(tagCodeData& vOut)
	//{
	//	//先压入条件
	//	if (pCondCode)
	//	{
	//		pCondCode->SetRegisterIndex(cRegisterIndex);
	//		pCondCode->MakeExeCode(vOut);
	//	}
	//	CJumpFalseExeCode* pIfCode = CExeCodeMgr::GetInstance()->New<CJumpFalseExeCode>(m_unBeginSoureIndex);
	//	pIfCode->cVarRegister = cRegisterIndex;
	//	vOut.AddCode(pIfCode);
	//	if (pTureCode)
	//		pTureCode->MakeExeCode(vOut);

	//	if (pFalseCode)
	//	{
	//		CJumpExeCode* pElseCode = CExeCodeMgr::GetInstance()->New<CJumpExeCode>(m_unBeginSoureIndex);
	//		vOut.AddCode(pElseCode);
	//		pFalseCode->MakeExeCode(vOut);
	//		pIfCode->pJumpCode = pElseCode->m_pNext;
	//		CSignExeCode* pEndCode = CExeCodeMgr::GetInstance()->New<CSignExeCode>(m_unBeginSoureIndex);
	//		vOut.AddCode(pEndCode);
	//		pElseCode->pJumpCode = pEndCode;
	//	}
	//	else
	//	{
	//		CSignExeCode* pEndCode = CExeCodeMgr::GetInstance()->New<CSignExeCode>(m_unBeginSoureIndex);
	//		vOut.AddCode(pEndCode);
	//		pIfCode->pJumpCode = pEndCode;
	//	}
	//	return true;
	//}

	void CIfICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (nType == E_COND)
		{
			pCondCode = pCode;
		}
		else if (nType == E_TRUE)
		{
			pTureCode = pCode;
		}
		else if (nType == E_FALSE)
		{
			pFalseCode = pCode;
		}
		CBaseICode::AddICode(nType, pCode);
	}

	bool CIfICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		if (pCompiler == nullptr)
		{
			return false;
		}

		GetNewWord(nextWord);
		if (nextWord.word != "if")
		{
			RevertAll();
			return false;
		}

		GetWord(nextWord);
		if (nextWord.word != "(")
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}

		if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_EXPRESSION, this, E_COND))
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}

		GetWord(nextWord);
		if (nextWord.word != ")")
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}

		if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_STATEMENT, this, E_TRUE))
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}

		GetWord(nextWord);
		if (nextWord.word != "else")
		{
			RevertOne();
			return true;
		}

		if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_STATEMENT, this, E_FALSE))
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}

		return true;
	}

	bool CIfICode::MakeExeCode(CExeCodeData& vOut)
	{
		//先压入条件
		CJumpFalseExeCode* pIfCode = CExeCodeMgr::GetInstance()->New<CJumpFalseExeCode>(m_unBeginSoureIndex);
		if (pCondCode)
		{
			pCondCode->SetRegisterIndex(R_A);
			if (pCondCode->MakeExeCode(vOut) == false)
			{
				return false;
			}
			if (pCondCode->GetType() == E_I_CODE_EXPRESSION)
			{
				CExpressionICode* pExpressCode = (CExpressionICode*)pCondCode;
				if (pExpressCode->pOperandCode && pExpressCode->pOperandCode->GetType() == E_I_CODE_LOADVAR)
				{
					CLoadVarICode* pLoadCode = (CLoadVarICode*)pExpressCode->pOperandCode;
					pIfCode->condParam.nType = pLoadCode->nLoadType;
					pIfCode->condParam.dwPos = pLoadCode->VarIndex;
				}
				else
				{
					if (pCondCode->GetRegisterIndex() < R_SIZE)
					{
						pIfCode->condParam.nType = E_VAR_SCOPE_REGISTER;
						pIfCode->condParam.dwPos = pCondCode->GetRegisterIndex();
					}
					else
					{
						pIfCode->condParam.nType = E_VAR_SCOPE_REGISTER_STACK;
					}
				}
			}
		}
		
		vOut.AddCode(pIfCode);
		if (pTureCode)
		{
			if (pTureCode->MakeExeCode(vOut) == false)
			{
				return false;
			}
		}

		if (pFalseCode)
		{
			CJumpExeCode* pElseCode = CExeCodeMgr::GetInstance()->New<CJumpExeCode>(m_unBeginSoureIndex);
			vOut.AddCode(pElseCode);
			if (pFalseCode->MakeExeCode(vOut) == false)
			{
				return false;
			}
			pIfCode->pJumpCode = pElseCode->m_pNext;
			CSignExeCode* pEndCode = CExeCodeMgr::GetInstance()->New<CSignExeCode>(m_unBeginSoureIndex);
			vOut.AddCode(pEndCode);
			pElseCode->pJumpCode = pEndCode;
		}
		else
		{
			CSignExeCode* pEndCode = CExeCodeMgr::GetInstance()->New<CSignExeCode>(m_unBeginSoureIndex);
			vOut.AddCode(pEndCode);
			pIfCode->pJumpCode = pEndCode;
		}
		return true;
	}

	//bool CIfICode::CheckCondVar(CBaseVar* pVar)
	//{
	//	if (pVar->GetType() == CScriptClassInfo<CIntVar>::GetInstance().nClassType)
	//	{
	//		CIntVar* pIntVar = dynamic_cast<CIntVar*>(pVar);
	//		if (pIntVar && pIntVar->ToInt() != 0)
	//		{
	//			return true;
	//		}
	//	}
	//	else if (pVar->GetType() == CScriptClassInfo<CStringVar>::GetInstance().nClassType)
	//	{

	//	}
	//	else
	//	{
	//		CPointVar* pPoint = dynamic_cast<CPointVar*>(pVar);
	//		if (pPoint && pPoint->ToPoint())
	//		{
	//			return true;
	//		}
	//	}
	//	return false;
	//}

	//int CIfICode::Run(CScriptExecBlock* pBlock)
	//{
	//	int& state = pBlock->GetRunState(m_nRunStateIndex);
	//	if (state == 0)
	//	{
	//		int result = pCondCode->Run(pBlock);
	//		if (result != CScriptExecBlock::ERESULT_CONTINUE)
	//		{
	//			return result;
	//		}
	//		state++;
	//	}
	//	if (state == 1)
	//	{
	//		state++;
	//		CBaseVar* pCondVar = nullptr;
	//		STACK_POP(pBlock->registerStack, pCondVar);
	//		if (CheckCondVar(pCondVar))
	//		{
	//			state = 2;
	//		}
	//		else
	//		{
	//			state = 3;
	//		}
	//		SCRIPTVAR_RELEASE(pCondVar);
	//	}
	//	int result = CScriptExecBlock::ERESULT_CONTINUE;
	//	if (state == 2)
	//	{
	//		result = pTureCode->Run(pBlock);
	//		if (result != CScriptExecBlock::ERESULT_CONTINUE)
	//		{
	//			return result;
	//		}
	//		state = 4;
	//	}
	//	if (state == 3)
	//	{
	//		result = pFalseCode->Run(pBlock);
	//		if (result != CScriptExecBlock::ERESULT_CONTINUE)
	//		{
	//			return result;
	//		}
	//		state = 4;
	//	}

	//	state = 0;
	//	pBlock->RevertRunState(m_nRunStateIndex);
	//	return result;
	//}

	//bool CWhileICode::MakeExeCode(tagCodeData& vOut)
	//{
	//	if (pCondCode == nullptr)
	//	{
	//		return false;
	//	}
	//	if (pBodyCode == nullptr)
	//	{
	//		return false;
	//	}
	//	CBaseExeCode* pBegin = vOut.pEndCode;
	//	pCondCode->MakeExeCode(vOut);

	//	CJumpFalseExeCode* pIfCode = CExeCodeMgr::GetInstance()->New<CJumpFalseExeCode>(m_unBeginSoureIndex);
	//	pIfCode->cVarRegister = cRegisterIndex;
	//	vOut.AddCode(pIfCode);

	//	pBodyCode->MakeExeCode(vOut);
	//	//要在块尾加入返回块头的指令
	//	CJumpExeCode* pJumpBeginCode = CExeCodeMgr::GetInstance()->New<CJumpExeCode>(m_unBeginSoureIndex);
	//	if (pBegin)
	//		pJumpBeginCode->pJumpCode = pBegin->m_pNext;
	//	else
	//		pJumpBeginCode->pJumpCode = vOut.pBeginCode;
	//	vOut.AddCode(pJumpBeginCode);

	//	CSignExeCode* pEndCode = CExeCodeMgr::GetInstance()->New<CSignExeCode>(m_unBeginSoureIndex);
	//	vOut.AddCode(pEndCode);
	//	pIfCode->pJumpCode = pEndCode;

	//	for (CBaseExeCode* pCheckCode = pIfCode; pCheckCode && pCheckCode != pJumpBeginCode; pCheckCode = pCheckCode->m_pNext)
	//	{
	//		auto pBreakCode = dynamic_cast<CBreakExeCode*>(pCheckCode);
	//		if (pBreakCode)
	//		{
	//			if (pBreakCode->pJumpCode == nullptr)
	//			{
	//				pBreakCode->pJumpCode = pEndCode;
	//			}
	//		}
	//		else
	//		{
	//			auto pContinueCode = dynamic_cast<CContinueExeCode*>(pCheckCode);
	//			if (pContinueCode)
	//			{
	//				if (pContinueCode->pJumpCode == nullptr)
	//				{
	//					pContinueCode->pJumpCode = pBegin->m_pNext;;
	//				}
	//			}
	//		}

	//	}
	//	return true;
	//}

	void CWhileICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (nType == 0)
		{
			pCondCode = pCode;
		}
		else if (nType == 1)
		{
			pBodyCode = pCode;
		}
		CBaseICode::AddICode(nType, pCode);
	}
	bool CWhileICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		if (pCompiler == nullptr)
		{
			return false;
		}

		GetNewWord(nextWord);
		if (nextWord.word != "while")
		{
			RevertAll();
			return false;
		}

		GetWord(nextWord);
		if (nextWord.word != "(")
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}

		if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_EXPRESSION, this, E_COND))
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}

		GetWord(nextWord);
		if (nextWord.word != ")")
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}

		if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_STATEMENT, this, E_BLOCK))
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}

		return true;
	}
	bool CWhileICode::MakeExeCode(CExeCodeData& vOut)
	{
		if (pCondCode == nullptr)
		{
			return false;
		}
		if (pBodyCode == nullptr)
		{
			return false;
		}
		CBaseExeCode* pBegin = vOut.pEndCode;
		CJumpFalseExeCode* pIfCode = CExeCodeMgr::GetInstance()->New<CJumpFalseExeCode>(m_unBeginSoureIndex);
		pCondCode->SetRegisterIndex(R_A);
		if (pCondCode->MakeExeCode(vOut) == false)
		{
			return false;
		}
		if (pCondCode->GetType() == E_I_CODE_EXPRESSION)
		{
			CExpressionICode* pExpressCode = (CExpressionICode*)pCondCode;
			if (pExpressCode->pOperandCode && pExpressCode->pOperandCode->GetType() == E_I_CODE_LOADVAR)
			{
				CLoadVarICode* pLoadCode = (CLoadVarICode*)pExpressCode->pOperandCode;
				pIfCode->condParam.nType = pLoadCode->nLoadType;
				pIfCode->condParam.dwPos = pLoadCode->VarIndex;
			}
			else
			{
				if (pCondCode->GetRegisterIndex() < R_SIZE)
				{
					pIfCode->condParam.nType = E_VAR_SCOPE_REGISTER;
					pIfCode->condParam.dwPos = pCondCode->GetRegisterIndex();
				}
				else
				{
					pIfCode->condParam.nType = E_VAR_SCOPE_REGISTER_STACK;
				}
			}
		}

		
		vOut.AddCode(pIfCode);

		if (pBodyCode->MakeExeCode(vOut) == false)
		{
			return false;
		}
		//要在块尾加入返回块头的指令
		CJumpExeCode* pJumpBeginCode = CExeCodeMgr::GetInstance()->New<CJumpExeCode>(m_unBeginSoureIndex);
		if (pBegin)
			pJumpBeginCode->pJumpCode = pBegin->m_pNext;
		else
			pJumpBeginCode->pJumpCode = vOut.pBeginCode;
		vOut.AddCode(pJumpBeginCode);

		CSignExeCode* pEndCode = CExeCodeMgr::GetInstance()->New<CSignExeCode>(m_unBeginSoureIndex);
		vOut.AddCode(pEndCode);
		pIfCode->pJumpCode = pEndCode;

		for (CBaseExeCode* pCheckCode = pIfCode; pCheckCode && pCheckCode != pJumpBeginCode; pCheckCode = pCheckCode->m_pNext)
		{
			auto pBreakCode = dynamic_cast<CBreakExeCode*>(pCheckCode);
			if (pBreakCode)
			{
				if (pBreakCode->pJumpCode == nullptr)
				{
					pBreakCode->pJumpCode = pEndCode;
				}
			}
			else
			{
				auto pContinueCode = dynamic_cast<CContinueExeCode*>(pCheckCode);
				if (pContinueCode)
				{
					if (pContinueCode->pJumpCode == nullptr)
					{
						pContinueCode->pJumpCode = pBegin->m_pNext;;
					}
				}
			}

		}
		return true;
	}
	//bool CWhileICode::CheckCondVar(CBaseVar* pVar)
	//{
	//	if (pVar->GetType() == CScriptClassInfo<CIntVar>::GetInstance().nClassType)
	//	{
	//		CIntVar* pIntVar = dynamic_cast<CIntVar*>(pVar);
	//		if (pIntVar && pIntVar->ToInt() != 0)
	//		{
	//			return true;
	//		}
	//	}
	//	else if (pVar->GetType() == CScriptClassInfo<CStringVar>::GetInstance().nClassType)
	//	{

	//	}
	//	else
	//	{
	//		CPointVar* pPoint = dynamic_cast<CPointVar*>(pVar);
	//		if (pPoint && pPoint->ToPoint())
	//		{
	//			return true;
	//		}
	//	}
	//	return false;
	//}
	//int CWhileICode::Run(CScriptExecBlock* pBlock)
	//{
	//	int& state = pBlock->GetRunState(m_nRunStateIndex);
	//	while (true)
	//	{
	//		if (state == 0)
	//		{
	//			int result = pCondCode->Run(pBlock);
	//			if (result != CScriptExecBlock::ERESULT_CONTINUE)
	//			{
	//				return result;
	//			}
	//			state++;
	//		}
	//		if (state == 1)
	//		{
	//			CBaseVar* pCondVar = nullptr;
	//			STACK_POP(pBlock->registerStack, pCondVar);
	//			if (CheckCondVar(pCondVar))
	//			{
	//				state = 2;
	//			}
	//			else
	//			{
	//				state = 3;
	//			}
	//			SCRIPTVAR_RELEASE(pCondVar);
	//		}
	//		if (state == 2)
	//		{
	//			int result = pBodyCode->Run(pBlock);
	//			switch (result)
	//			{
	//			case CScriptExecBlock::ERESULT_LOOP_HEAD:
	//				state = 0;
	//				break;
	//			case CScriptExecBlock::ERESULT_LOOP_END:
	//				state = 3;
	//				break;
	//			case CScriptExecBlock::ERESULT_CONTINUE:
	//				state = 4;
	//				break;
	//			default:
	//				return result;
	//			}
	//			state = 4;
	//		}
	//		if (state == 3)
	//		{
	//			break;
	//		}
	//		state = 0;
	//	}

	//	pBlock->RevertRunState(m_nRunStateIndex);
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}
	//bool CContinueICode::MakeExeCode(tagCodeData& vOut)
	//{
	//	CContinueExeCode* pCode = CExeCodeMgr::GetInstance()->New<CContinueExeCode>(m_unBeginSoureIndex);
	//	vOut.AddCode(pCode);

	//	return true;
	//}
	bool CContinueICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();
		if (pCompiler == nullptr)
		{
			return false;
		}
		GetNewWord(nextWord);
		if (nextWord.word != "continue")
		{
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		if (nextWord.word != ";")
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}
		return true;
	}
	bool CContinueICode::MakeExeCode(CExeCodeData& vOut)
	{
		CBreakExeCode* pCode = CExeCodeMgr::GetInstance()->New<CBreakExeCode>(m_unBeginSoureIndex);
		vOut.AddCode(pCode);
		return true;
	}
	//int CContinueICode::Run(CScriptExecBlock* pBlock)
	//{
	//	return CScriptExecBlock::ERESULT_LOOP_HEAD;
	//}
	//bool CBreakICode::MakeExeCode(tagCodeData& vOut)
	//{
	//	CBreakExeCode* pCode = CExeCodeMgr::GetInstance()->New<CBreakExeCode>(m_unBeginSoureIndex);
	//	vOut.AddCode(pCode);
	//	return true;
	//}
	bool CBreakICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();
		if (pCompiler == nullptr)
		{
			return false;
		}
		GetNewWord(nextWord);
		if (nextWord.word != "break")
		{
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		if (nextWord.word != ";")
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}
		return true;
	}
	bool CBreakICode::MakeExeCode(CExeCodeData& vOut)
	{
		CContinueExeCode* pCode = CExeCodeMgr::GetInstance()->New<CContinueExeCode>(m_unBeginSoureIndex);
		vOut.AddCode(pCode);
		return true;
	}
	//int CBreakICode::Run(CScriptExecBlock* pBlock)
	//{
	//	return CScriptExecBlock::ERESULT_LOOP_END;
	//}
	//bool CReturnICode::MakeExeCode(tagCodeData& vOut)
	//{
	//	if (pBodyCode)
	//	{
	//		pBodyCode->SetRegisterIndex(cRegisterIndex);
	//		pBodyCode->MakeExeCode(vOut);
	//	}

	//	CReturnExeCode* pCode = CExeCodeMgr::GetInstance()->New<CReturnExeCode>(m_unBeginSoureIndex);
	//	pCode->cVarRegister = cRegisterIndex;
	//	vOut.AddCode(pCode);

	//	return true;
	//}

	void CReturnICode::AddICode(int nType, CBaseICode* pCode)
	{
		pBodyCode = pCode;
		CBaseICode::AddICode(nType, pCode);
	}

	bool CReturnICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();
		if (pCompiler == nullptr)
		{
			return false;
		}
		GetNewWord(nextWord);
		if (nextWord.word != "return")
		{
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		if (nextWord.word == ";")
		{
			//不返回
			return true;
		}
		RevertOne();
		if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_EXPRESSION, this, 0))
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}

		GetWord(nextWord);
		if (nextWord.word != ";")
		{
			AddErrorInfo(10, nextWord.nSourceWordsIndex, ESIGN_ILLEGAL_END);
			RevertAll();
			return false;
		}
		return true;
	}

	bool CReturnICode::MakeExeCode(CExeCodeData& vOut)
	{
		CReturnExeCode* pCode = CExeCodeMgr::GetInstance()->New<CReturnExeCode>(m_unBeginSoureIndex);
		if (vOut.nDefaultReturnType != EScriptVal_None)
		{
			pCode->bNeedReturnVar = true;
		}

		if (pBodyCode)
		{
			if (pBodyCode->MakeExeCode(vOut) == false)
			{
				return false;
			}

			if (pBodyCode->GetType() == E_I_CODE_EXPRESSION)
			{
				CExpressionICode* pExpressCode = (CExpressionICode*)pBodyCode;
				if (pExpressCode->pOperandCode && pExpressCode->pOperandCode->GetType() == E_I_CODE_LOADVAR)
				{
					CLoadVarICode* pLoadCode = (CLoadVarICode*)pExpressCode->pOperandCode;
					pCode->returnParam.nType = pLoadCode->nLoadType;
					pCode->returnParam.dwPos = pLoadCode->VarIndex;
				}
				else
				{
					pCode->returnParam.nType = E_VAR_SCOPE_REGISTER_STACK;
				}
			}
		}
		vOut.AddCode(pCode);

		return true;
	}

	//int CReturnICode::Run(CScriptExecBlock* pBlock)
	//{
	//	int& state = pBlock->GetRunState(m_nRunStateIndex);
	//	if (state == 0)
	//	{
	//		int result = pBodyCode->Run(pBlock);
	//		if (result != CScriptExecBlock::ERESULT_CONTINUE)
	//		{
	//			return result;
	//		}
	//		state++;
	//	}
	//	state = 0;
	//	pBlock->RevertRunState(m_nRunStateIndex);
	//	return CScriptExecBlock::ERESULT_END;
	//}


	//bool CNewICode::MakeExeCode(tagCodeData& vOut)
	//{
	//	unsigned short nClassType = 0;
	//	char cType = CScriptCodeLoader::GetInstance()->GetVarType(strClassType, nClassType);
	//	if (cType != EScriptVal_ClassPoint)
	//	{
	//		return false;
	//	}
	//	CNewClassExeCode* pCode = CExeCodeMgr::GetInstance()->New<CNewClassExeCode>(m_unBeginSoureIndex);
	//	pCode->cResultRegister = cRegisterIndex;
	//	pCode->dwClassIndex = nClassType;
	//	vOut.AddCode(pCode);

	//	return true;
	//}

	void CNewICode::AddICode(int nType, CBaseICode* pCode)
	{
		CBaseICode::AddICode(nType, pCode);
	}

	bool CNewICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();
		if (pCompiler == nullptr)
		{
			return false;
		}
		GetNewWord(nextWord);
		if (nextWord.word != "new")
		{
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		strClassType = nextWord.word;
		nClassType = CScriptVarTypeMgr::GetInstance()->GetVarType(strClassType);
		return true;
	}

	bool CNewICode::MakeExeCode(CExeCodeData& vOut)
	{
		CNewClassExeCode* pCode = CExeCodeMgr::GetInstance()->New<CNewClassExeCode>(m_unBeginSoureIndex);
		pCode->dwClassIndex = nClassType;
		vOut.AddCode(pCode);

		return true;
	}

	//int CNewICode::Run(CScriptExecBlock* pBlock)
	//{
	//	if (nClassType < 0)
	//	{
	//		nClassType = CScriptVarTypeMgr::GetInstance()->GetVarType(strClassType);
	//	}
	//	auto pPoint = CScriptVarTypeMgr::GetInstance()->NewObject(nClassType);
	//	CBaseVar* pVar = CScriptVarTypeMgr::GetInstance()->GetVar(nClassType);
	//	if (pVar->Set(pPoint))
	//	{
	//		STACK_PUSH_MOVE(pBlock->registerStack, pVar);
	//	}
	//	else
	//	{
	//		CScriptVarTypeMgr::GetInstance()->ReleaseVar(pVar);
	//		CScriptVarTypeMgr::GetInstance()->ReleaseObject(pPoint);
	//	}
	//	return CScriptExecBlock::ERESULT_CONTINUE;
	//}

	//bool CDeleteICode::MakeExeCode(tagCodeData& vOut)
	//{
	//	VarInfo* pVarInfo = GetTempVarInfo(m_VarName.c_str());
	//	if (pVarInfo == nullptr)
	//	{
	//		//检查是否是全局变量
	//		pVarInfo = CScriptCodeLoader::GetInstance()->GetGlobalVarInfo(m_VarName);
	//	}
	//	if (pVarInfo == nullptr)
	//	{
	//		return false;
	//	}
	//	if (pVarInfo->cType != EScriptVal_ClassPoint)
	//	{
	//		return false;
	//	}
	//	CLoadExeCode* pLoadCode = CExeCodeMgr::GetInstance()->New<CLoadExeCode>(m_unBeginSoureIndex);
	//	pLoadCode->cType = pVarInfo->cGlobal == 1 ? ESIGN_POS_GLOBAL_VAR : ESIGN_POS_LOACL_VAR;
	//	pLoadCode->cResultRegister = R_C;
	//	pLoadCode->dwPos = pVarInfo->dwPos;
	//	vOut.AddCode(pLoadCode);

	//	CReleaseClassExeCode* pCode = CExeCodeMgr::GetInstance()->New<CReleaseClassExeCode>(m_unBeginSoureIndex);
	//	pCode->cVarRegister = R_C;
	//	vOut.AddCode(pCode);
	//	return true;
	//}

	//void CDeleteICode::AddICode(int nType, CBaseICode* pCode)
	//{
	//	CBaseICode::AddICode(nType, pCode);
	//}

	//bool CDeleteICode::Compile(SentenceSourceCode& vIn)
	//{
	//	SignToPos();
	//	if (m_pCompiler == nullptr)
	//	{
	//		return false;
	//	}
	//	GetNewWord(nextWord);
	//	if (nextWord.word != "delete")
	//	{
	//		RevertAll();
	//		return false;
	//	}
	//	GetWord(nextWord);
	//	m_VarName = nextWord.word;
	//	GetWord(nextWord);
	//	if (nextWord.word != ";")
	//	{
	//		m_pCompiler->AddErrorInfo(nextWord.nSourceWordsIndex, "CDeleteICode:format error");
	//		RevertAll();
	//		return false;
	//	}
	//	return true;
	//}
	//bool CBracketsICode::MakeExeCode(tagCodeData& vOut)
	//{
	//	if (m_pBody)
	//	{
	//		if (!m_pBody->MakeExeCode(vOut))
	//		{
	//			return false;
	//		}
	//		CMoveExeCode* pCode = CExeCodeMgr::GetInstance()->New<CMoveExeCode>(m_unBeginSoureIndex);
	//		pCode->cVarRegister = R_A;
	//		pCode->cType = ESIGN_REGISTER;
	//		pCode->dwPos = cRegisterIndex;
	//		vOut.AddCode(pCode);
	//	}
	//	return true;
	//}

	void CBracketsICode::AddICode(int nType, CBaseICode* pCode)
	{
		m_pBody = pCode;
	}

	bool CBracketsICode::MakeExeCode(CExeCodeData& vOut)
	{
		if (m_pBody)
		{
			if (!m_pBody->MakeExeCode(vOut))
			{
				return false;
			}
		}
		return true;
	}

	bool CBracketsICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();
		if (pCompiler == nullptr)
		{
			return false;
		}
		GetNewWord(nextWord);
		if (nextWord.word != "(")
		{
			RevertAll();
			return false;
		}
		if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_EXPRESSION, this, 0))
		{
			AddErrorInfo(2, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		if (nextWord.word != ")")
		{
			AddErrorInfo(2, nextWord.nSourceWordsIndex, ERROR_FORMAT_NOT);
			RevertAll();
			return false;
		}
		return true;
	}
	//int CBracketsICode::Run(CScriptExecBlock* pBlock)
	//{
	//	return m_pBody->Run(pBlock);
	//}
	//bool CTestSignICode::MakeExeCode(tagCodeData& vOut)
	//{
	//	for (int i = 0; i < nNum / 2; i++)
	//	{
	//		//CSignExeCode *pCode = CExeCodeMgr::GetInstance()->New<CSignExeCode>(m_unBeginSoureIndex);
	//		//vOut.AddCode(pCode);

	//		//CAddExeCode* pCode = CExeCodeMgr::GetInstance()->New<CAddExeCode>(m_unBeginSoureIndex);
	//		//pCode->cResultRegister = cRegisterIndex;
	//		//pCode->cType = 0;
	//		//pCode->dwPos = 1;
	//		//vOut.AddCode(pCode);

	//		CCmpLessExeCode* pCode = CExeCodeMgr::GetInstance()->New<CCmpLessExeCode>(m_unBeginSoureIndex);
	//		pCode->cResultRegister = cRegisterIndex;
	//		pCode->cType = 0;
	//		pCode->dwPos = 1;
	//		vOut.AddCode(pCode);

	//		//CPushExeCode* pCode = CExeCodeMgr::GetInstance()->New<CPushExeCode>(m_unBeginSoureIndex);
	//		////pCode->cVarRegister = cRegisterIndex;
	//		//pCode->cType = 2;
	//		//pCode->dwPos = 1;
	//		//vOut.AddCode(pCode);
	//		//CPopExeCode* pCode2 = CExeCodeMgr::GetInstance()->New<CPopExeCode>(m_unBeginSoureIndex);
	//		////pCode->cVarRegister = cRegisterIndex;
	//		//pCode2->cType = 6;
	//		//pCode2->dwPos = 0;
	//		//vOut.AddCode(pCode2);
	//	}
	//	return true;
	//}
	//void CTestSignICode::AddICode(int nType, CBaseICode* pCode)
	//{
	//	CBaseICode::AddICode(nType, pCode);
	//}
	//bool CTestSignICode::Compile(SentenceSourceCode& vIn)
	//{
	//	SignToPos();
	//	if (m_pCompiler == nullptr)
	//	{
	//		return false;
	//	}
	//	GetNewWord(nextWord);
	//	if (nextWord.word != "testsign")
	//	{
	//		RevertAll();
	//		return false;
	//	}
	//	GetWord(nextWord);
	//	nNum = atoi(nextWord.word.c_str());
	//	GetWord(nextWord);
	//	if (nextWord.word != ";")
	//	{
	//		m_pCompiler->AddErrorInfo(nextWord.nSourceWordsIndex, "CTestSignICode:format error");
	//		RevertAll();
	//		return false;
	//	}
	//	return true;
	//}
	//void CICodeMgr::Release(CBaseICode* pPoint)
	//{
	//}
	//void CICodeMgr<T>::Clear()
	//{
	//	for (auto it = m_listICode.begin(); it != m_listICode.end(); it++)
	//	{
	//		CBaseICode* pCode = *it;
	//		if (pCode)
	//		{
	//			delete pCode;
	//		}
	//	}
	//	m_listICode.clear();
	//}



}