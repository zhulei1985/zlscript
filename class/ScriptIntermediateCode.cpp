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
			AddErrorInfo(
				strType.nSourceWordsIndex,
				"CDefGlobalVarICode:type is none");
			RevertAll();
			return false;
		}
		GetNewWord(strName);

		if (!CScriptCodeLoader::GetInstance()->CheckVarName(strName.word))
		{
			AddErrorInfo(
				strName.nSourceWordsIndex,
				"CDefGlobalVarICode:Callback Function name already exists");
			RevertAll();
			return false;
		}

		GetNewWord(strSign);

		if (strSign.word == ";")
		{
			if (!CScriptGlobalVarMgr::GetInstance()->New(strName.word, unVarType))
			{
				AddErrorInfo(
					strName.nSourceWordsIndex,
					"CDefGlobalVarICode:global var cannot add");
				RevertAll();
				return false;
			}
		}
		else if (strSign.word == "=")
		{
			if (!CScriptGlobalVarMgr::GetInstance()->New(strName.word, unVarType))
			{
				AddErrorInfo(
					strName.nSourceWordsIndex,
					"CDefGlobalVarICode:global var cannot add");
				RevertAll();
				return false;
			}
			auto pGVar = CScriptGlobalVarMgr::GetInstance()->Get(strName.word);
			if (!pGVar)
			{
				AddErrorInfo(
					strName.nSourceWordsIndex,
					"CDefGlobalVarICode:global var cannot set");
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
				pGVar->pVar->Set(pPoint);
			}
			else
			{
				pGVar->pVar->Set(strVal.word);
			}

			CScriptGlobalVarMgr::GetInstance()->Revert(pGVar);

			GetNewWord(NextWord);
			if (NextWord.word != ";")
			{
				AddErrorInfo(
					NextWord.nSourceWordsIndex,
					"CDefGlobalVarICode : Format error , not end");
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
			AddErrorInfo(
				strType.nSourceWordsIndex,
				"CDefTempVarICode:type is none");
			RevertAll();
			return false;
		}
		GetNewWord(strName);

		//检测名字是否与回调函数名冲突
		if (!CScriptCodeLoader::GetInstance()->CheckVarName(strName.word))
		{
			AddErrorInfo(
				strName.nSourceWordsIndex,
				"CDefTempVarICode: name error");
			RevertAll();
			return false;
		}
		if (this->CheckTempVar(strName.word.c_str()))
		{
			AddErrorInfo(
				strName.nSourceWordsIndex,
				"CDefTempVarICode:name already exists");
			RevertAll();
			return false;
		}

		GetNewWord(strSign);
		if (strSign.word == ";")
		{
			if (!this->DefineTempVar(unVarType, strName.word, pCompiler))
			{
				AddErrorInfo(
					strName.nSourceWordsIndex,
					"CDefTempVarICode:global var cannot add");
				RevertAll();
				return false;
			}
		}
		else if (strSign.word == "=")
		{
			if (!this->DefineTempVar(unVarType, strName.word, pCompiler))
			{
				AddErrorInfo(
					strName.nSourceWordsIndex,
					"CDefTempVarICode:global var cannot add");
				RevertAll();
				return false;
			}
			RevertOne();
			RevertOne();
			return true;
		}
		else
		{
			AddErrorInfo(
				strName.nSourceWordsIndex,
				"CDefTempVarICode:format error");
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
			AddErrorInfo(
				strType.nSourceWordsIndex,
				"CFunICode:DefineTempVar(defining temporary variable error)");
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
		unsigned short unVarType = CScriptVarTypeMgr::GetInstance()->GetVarType(strType);
		if (unVarType == EScriptVal_None)
		{
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"CFunICode:type is none");
			RevertAll();
			return false;
		}
		GetNewWord(strName);

		//检测名字是否与回调函数名冲突
		if (CScriptCallBackFunion::GetFunIndex(strName.word) >= 0)
		{
			AddErrorInfo(
				strName.nSourceWordsIndex,
				"CFunICode:Callback Function name already exists");
			RevertAll();
			return false;
		}
		if (CScriptCodeLoader::GetInstance()->CheckCurCompileFunName(strName.word))
		{
			AddErrorInfo(
				strName.nSourceWordsIndex,
				"CFunICode:Function name already exists");
			RevertAll();
			return false;
		}
		funname = strName.word;
		GetNewWord(strSign);
		if (strSign.word != "(")
		{
			AddErrorInfo(
				strSign.nSourceWordsIndex,
				"CFunICode:format error,no bracket");
			RevertAll();
			return false;
		}
		pCompiler->ClearTempVarIndex();
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
					AddErrorInfo(
						nextWord.nSourceWordsIndex,
						"CFunICode:format error,FunctionParameter");
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
		CScriptCodeLoader::GetInstance()->AddCurCompileFunName(strName.word);
		CScriptCodeLoader::GetInstance()->SetFunICode(strName.word, this);
		return true;
	}

	void CFunICode::AddICode(int nType, CBaseICode* pCode)
	{
		vBodyCode.push_back(pCode);
		CBaseICode::AddICode(nType, pCode);
	}

	int CFunICode::Run(CScriptExecBlock* pBlock)
	{
		int& state = pBlock->GetRunState(m_nRunStateIndex);
		if (state == 0)
		{
			//注册变量
			auto it = m_mapTempVarInfo.begin();
			for (; it != m_mapTempVarInfo.end(); it++)
			{
				pBlock->RegisterLoaclVar(it->second.index, it->second.nType);
			}

			state++;
		}
		while (state > 0 && state <= vBodyCode.size())
		{
			int result = vBodyCode[state - 1]->Run(pBlock);
			
			if (result != CScriptExecBlock::ERESULT_CONTINUE)
			{
				return result;
			}
			state++;
		}

		state = 0;
		pBlock->RevertRunState(m_nRunStateIndex);
		return CScriptExecBlock::ERESULT_END;
	}

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
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"CBlockICode:format error");
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

	int CBlockICode::Run(CScriptExecBlock* pBlock)
	{
		int& state = pBlock->GetRunState(m_nRunStateIndex);
		if (state == 0)
		{
			//注册变量
			auto it = m_mapTempVarInfo.begin();
			for (; it != m_mapTempVarInfo.end(); it++)
			{
				pBlock->RegisterLoaclVar(it->second.index, it->second.nType);
			}

			state++;
		}
		while (state > 0 && state <= m_vICode.size())
		{
			int result = m_vICode[state - 1]->Run(pBlock);

			if (result != CScriptExecBlock::ERESULT_CONTINUE)
			{
				return result;
			}
			state++;
		}

		state = 0;
		pBlock->RevertRunState(m_nRunStateIndex);
		return CScriptExecBlock::ERESULT_CONTINUE;
	}

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
				LoaclVarIndex = pInfo->index;
			}
			else
			{
				//检查是否是全局变量
				stGlobalVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(nextWord.word);
				if (pGlobalVar)
				{
					nLoadType = E_VAR_SCOPE_GLOBAL;
					GlobalVarIndex = pGlobalVar->index;
					CScriptGlobalVarMgr::GetInstance()->Revert(pGlobalVar);
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

	int CLoadVarICode::Run(CScriptExecBlock* pBlock)
	{
		//向堆栈里压入一个数据

		switch (nLoadType)
		{
		case E_VAR_SCOPE_CONST:
		{
			STACK_PUSH_COPY(pBlock->registerStack, m_pConst);
		}
		break;
		case E_VAR_SCOPE_GLOBAL:
		{
			stGlobalVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(GlobalVarIndex);
			if (pGlobalVar)
			{
				STACK_PUSH_COPY(pBlock->registerStack, pGlobalVar->pVar);
				CScriptGlobalVarMgr::GetInstance()->Revert(pGlobalVar);
			}
		}
		break;
		case E_VAR_SCOPE_LOACL:
		{
			CBaseVar* pLoacllVar = pBlock->GetLoaclVar(LoaclVarIndex);
			if (pLoacllVar)
			{
				STACK_PUSH_COPY(pBlock->registerStack, pLoacllVar);
			}
		}
		break;

		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}

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
	int CSaveVarICode::Run(CScriptExecBlock* pBlock)
	{
		int& state = pBlock->GetRunState(m_nRunStateIndex);
		if (state == 0)
		{
			int result = pRightOperand->Run(pBlock);

			if (result != CScriptExecBlock::ERESULT_CONTINUE)
			{
				return result;
			}
			state++;
		}
		//从堆栈里取一个数据
		CBaseVar* pVar = nullptr;
		STACK_POP(pBlock->registerStack, pVar);
		if (nSaveType == E_VAR_SCOPE_GLOBAL)
		{
			stGlobalVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(GlobalVarIndex);
			if (pGlobalVar)
			{
				AssignVar(pGlobalVar->pVar, pVar);
				CScriptGlobalVarMgr::GetInstance()->Revert(pGlobalVar);
			}
		}
		else if (nSaveType == E_VAR_SCOPE_LOACL)
		{
			CBaseVar* pLoacllVar = pBlock->GetLoaclVar(LoaclVarIndex);
			AssignVar(pLoacllVar, pVar);
		}
		SCRIPTVAR_RELEASE(pVar);
		state = 0;
		pBlock->RevertRunState(m_nRunStateIndex);
		return CScriptExecBlock::ERESULT_CONTINUE;
	}

	bool CGetClassParamICode::Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler)
	{
		SignToPos();

		GetNewWord(nextWord);
		strClassVarName = nextWord.word;
		GetWord(nextWord);
		if (nextWord.word != "->" && nextWord.word != ".")
		{
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"CGetClassParamICode:format error ->");
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
			stGlobalVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(strClassVarName);
			if (pGlobalVar)
			{
				isGlobal = true;
				varIndex = pGlobalVar->index;
				CScriptGlobalVarMgr::GetInstance()->Revert(pGlobalVar);
				if (pGlobalVar->pVar)
					nClassType = pGlobalVar->pVar->GetType();
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
				}
			}
		}
		return true;
	}
	int CGetClassParamICode::Run(CScriptExecBlock* pBlock)
	{
		if (varIndex == -1)
		{
			return CScriptExecBlock::ERESULT_ERROR;
		}
		int result = CScriptExecBlock::ERESULT_CONTINUE;
		unsigned int nParamIndexRunTime = nParamIndex;

		if (isGlobal)
		{
			stGlobalVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(varIndex);
			if (pGlobalVar)
			{
				CPointVar* pPoint = dynamic_cast<CPointVar*>(pGlobalVar->pVar);
				if (pPoint)
				{
					if (pPoint->ToPoint())
					{
						if (nParamIndexRunTime < 0)
						{
							nParamIndexRunTime = pPoint->ToPoint()->GetAttributeIndex(strParamName);
						}
						auto param = pPoint->ToPoint()->GetAttribute(nParamIndexRunTime);
						if (param)
						{
							STACK_PUSH_COPY(pBlock->registerStack, param->ToScriptVal());
						}
						else
						{
							result = CScriptExecBlock::ERESULT_ERROR;
							//TODO 输出错误信息
						}
					}
					else
					{
						result = CScriptExecBlock::ERESULT_ERROR;
						//TODO 输出错误信息
					}
				}
				else
				{
					result = CScriptExecBlock::ERESULT_ERROR;
					//TODO 输出错误信息
				}
				CScriptGlobalVarMgr::GetInstance()->Revert(pGlobalVar);
			}
			else
			{
				result = CScriptExecBlock::ERESULT_ERROR;
				//TODO 输出错误信息
			}
		}
		else
		{
			CBaseVar* pLoacllVar = pBlock->GetLoaclVar(varIndex);
			if (pLoacllVar)
			{
				CPointVar* pPoint = dynamic_cast<CPointVar*>(pLoacllVar);
				if (pPoint)
				{
					if (pPoint->ToPoint())
					{
						if (nParamIndexRunTime < 0)
						{
							nParamIndexRunTime = pPoint->ToPoint()->GetAttributeIndex(strParamName);
						}
						auto param = pPoint->ToPoint()->GetAttribute(nParamIndexRunTime);
						if (param)
						{
							STACK_PUSH_COPY(pBlock->registerStack, param->ToScriptVal());
						}
						else
						{
							result = CScriptExecBlock::ERESULT_ERROR;
							//TODO 输出错误信息
						}
					}
					else
					{
						result = CScriptExecBlock::ERESULT_ERROR;
						//TODO 输出错误信息
					}
				}
				else
				{
					result = CScriptExecBlock::ERESULT_ERROR;
					//TODO 输出错误信息
				}
			}
			else
			{
				result = CScriptExecBlock::ERESULT_ERROR;
				//TODO 输出错误信息
			}
		}

		return result;
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
	int CSetClassParamICode::Run(CScriptExecBlock* pBlock)
	{
		int& state = pBlock->GetRunState(m_nRunStateIndex);
		if (state == 0)
		{	
			if (pRightOperand)
			{
				int result = pRightOperand->Run(pBlock);
				if (result != CScriptExecBlock::ERESULT_CONTINUE)
				{
					return result;
				}
			}

			state++;
			
			if (varIndex == -1)
			{
				return CScriptExecBlock::ERESULT_ERROR;
			}

		}
		int result = CScriptExecBlock::ERESULT_CONTINUE;
		unsigned int nParamIndexRunTime = nParamIndex;

		if (isGlobal)
		{
			stGlobalVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(varIndex);
			if (pGlobalVar)
			{
				CPointVar* pPoint = dynamic_cast<CPointVar*>(pGlobalVar->pVar);
				if (pPoint)
				{
					if (pPoint->ToPoint())
					{
						if (nParamIndexRunTime < 0)
						{
							nParamIndexRunTime = pPoint->ToPoint()->GetAttributeIndex(strParamName);
						}
						auto param = pPoint->ToPoint()->GetAttribute(nParamIndexRunTime);
						if (param)
						{
							CBaseVar* pVar = nullptr;
							STACK_POP(pBlock->registerStack, pVar);
							param->SetVal(pVar);
							SCRIPTVAR_RELEASE(pVar);
						}
						else
						{
							result = CScriptExecBlock::ERESULT_ERROR;
							//TODO 输出错误信息
						}
					}
					else
					{
						result = CScriptExecBlock::ERESULT_ERROR;
						//TODO 输出错误信息
					}
				}
				else
				{
					result = CScriptExecBlock::ERESULT_ERROR;
					//TODO 输出错误信息
				}
				CScriptGlobalVarMgr::GetInstance()->Revert(pGlobalVar);
			}
			else
			{
				result = CScriptExecBlock::ERESULT_ERROR;
				//TODO 输出错误信息
			}
		}
		else
		{
			CBaseVar* pLoacllVar = pBlock->GetLoaclVar(varIndex);
			if (pLoacllVar)
			{
				CPointVar* pPoint = dynamic_cast<CPointVar*>(pLoacllVar);
				if (pPoint)
				{
					if (pPoint->ToPoint())
					{
						if (nParamIndexRunTime < 0)
						{
							nParamIndexRunTime = pPoint->ToPoint()->GetAttributeIndex(strParamName);
						}
						auto param = pPoint->ToPoint()->GetAttribute(nParamIndexRunTime);
						if (param)
						{
							//从堆栈里取一个数据
							CBaseVar* pVar = nullptr;
							STACK_POP(pBlock->registerStack, pVar);
							param->SetVal(pVar);
							SCRIPTVAR_RELEASE(pVar);
						}
						else
						{
							result = CScriptExecBlock::ERESULT_ERROR;
							//TODO 输出错误信息
						}
					}
					else
					{
						result = CScriptExecBlock::ERESULT_ERROR;
						//TODO 输出错误信息
					}
				}
				else
				{
					result = CScriptExecBlock::ERESULT_ERROR;
					//TODO 输出错误信息
				}
			}
			else
			{
				result = CScriptExecBlock::ERESULT_ERROR;
				//TODO 输出错误信息
			}
		}
		state = 0;
		pBlock->RevertRunState(m_nRunStateIndex);
		return result;
	}
	//bool CMinusICode::MakeExeCode(tagCodeData& vOut)
	//{
	//	if (pRightOperand)
	//	{
	//		pRightOperand->SetRegisterIndex(R_A);
	//		pRightOperand->MakeExeCode(vOut);
	//	}

	//	CMinusExeCode* pCode = CExeCodeMgr::GetInstance()->New<CMinusExeCode>(m_unBeginSoureIndex);
	//	pCode->cResultRegister = cRegisterIndex;
	//	vOut.AddCode(pCode);

	//	return true;
	//}
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
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"CGetClassParamICode:format error ->");
			RevertAll();
			return false;
		}
		return true;
	}
	int CMinusICode::Run(CScriptExecBlock* pBlock)
	{
		int& state = pBlock->GetRunState(m_nRunStateIndex);
		if (state == 0)
		{
			if (pRightOperand)
			{
				int result = pRightOperand->Run(pBlock);
				if (result != CScriptExecBlock::ERESULT_CONTINUE)
				{
					return result;
				}
			}
			state++;
		}
		CBaseVar* pVar = nullptr;
		STACK_POP(pBlock->registerStack, pVar);
		if (pVar == nullptr)
		{
			//TODO 报错
			return CScriptExecBlock::ERESULT_ERROR;
		}
		if (pVar->GetType() == CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			CIntVar* pIntVar = (CIntVar*)pVar;
			pIntVar->Set(-pIntVar->ToInt());
		}
		else if (pVar->GetType() == CScriptClassInfo<CFloatVar>::GetInstance().nClassType)
		{
			CFloatVar* pFLoatVar = (CFloatVar*)pVar;
			pFLoatVar->Set(-pFLoatVar->ToFloat());
		}
		else
		{
			//TODO 报错
			return CScriptExecBlock::ERESULT_ERROR;
		}

		STACK_PUSH_MOVE(pBlock->registerStack, pVar);
		state = 0;
		pBlock->RevertRunState(m_nRunStateIndex);
		return CScriptExecBlock::ERESULT_CONTINUE;
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
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"COperatorICode:no Operator");
			RevertAll();
			return false;
		}
		strOperator = nextWord.word;
		nPriorityLv = nPRI;
		nOperatorCode = CScriptCodeLoader::GetInstance()->GetWordKey(strOperator);
		pOperGroup = CScriptVarOperatorMgr::GetInstance()->GetOperGroup(nOperatorCode);
		if (pOperGroup == nullptr)
		{
			//TODO 提示错误
			RevertAll();
			return false;
		}
		return true;
	}
	int COperatorICode::Run(CScriptExecBlock* pBlock)
	{
		int& state = pBlock->GetRunState(m_nRunStateIndex);

		CBaseVar* pLeftVar = nullptr;
		if (state == 0)
		{	
			if (pLeftOperand)
			{
				int result = pLeftOperand->Run(pBlock);
				if (result != CScriptExecBlock::ERESULT_CONTINUE)
				{
					return result;
				}
			}
			state++;
		}
		if (state == 1)
		{
			if (pLeftOperand)
			{
				STACK_POP(pBlock->registerStack, pLeftVar);
				if (pLeftVar == nullptr)
				{
					//TODO 报错
					return CScriptExecBlock::ERESULT_ERROR;
				}
			}
			state++;
		}
		CBaseVar* pRightVar = nullptr;
		if (state == 2)
		{
			if (pRightOperand)
			{
				int result = pRightOperand->Run(pBlock);
				if (result != CScriptExecBlock::ERESULT_CONTINUE)
				{
					return result;
				}
			}
			state++;
		}
		if (state == 3)
		{
			if (pRightOperand)
			{
				STACK_POP(pBlock->registerStack, pRightVar);
				if (pRightVar == nullptr)
				{
					//TODO 报错
					return CScriptExecBlock::ERESULT_ERROR;
				}
			}
			state++;
		}
		if (!CScriptVarOperatorMgr::GetInstance()->Operator(pOperGroup, pLeftVar, pRightVar))
		{
			//TODO 报错
			return CScriptExecBlock::ERESULT_ERROR;
		}
		state = 0;
		pBlock->RevertRunState(m_nRunStateIndex);
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
	//bool CCallBackFunICode::MakeExeCode(stCodeData& vOut)
	//{
	//	for (unsigned int i = 0; i < vParams.size(); i++)
	//	{
	//		CBaseICode* pCode = vParams[i];
	//		if (pCode)
	//		{
	//			pCode->MakeExeCode(vOut);
	//		}
	//	}
	//	CodeStyle code(m_unBeginSoureIndex);
	//	code.qwCode = 0;
	//	code.wInstruct = ECODE_CALL_CALLBACK;
	//	code.cSign = cRegisterIndex;
	//	code.cExtend = (char)vParams.size();
	//	code.dwPos = nFunIndex;
	//	vOut.push_back(code);
	//}
	//void CCallBackFunICode::AddICode(int nType, CBaseICode* pCode)
	//{
	//	if (nType == E_PARAM)
	//	{
	//		vParams.push_back(pCode);
	//	}
	//	CBaseICode::AddICode(nType, pCode);
	//}
	//bool CCallFunICode::MakeExeCode(tagCodeData& vOut)
	//{
	//	for (unsigned int i = 0; i < vParams.size(); i++)
	//	{
	//		CBaseICode* pCode = vParams[i];
	//		if (pCode)
	//		{
	//			pCode->SetRegisterIndex(R_A);
	//			pCode->MakeExeCode(vOut);
	//			CPushExeCode* pPushCode = CExeCodeMgr::GetInstance()->New<CPushExeCode>(m_unBeginSoureIndex);
	//			pPushCode->cType = ESIGN_REGISTER;
	//			pPushCode->dwPos = R_A;
	//			vOut.AddCode(pPushCode);
	//		}
	//	}
	//	int nCallBack = CScriptCallBackFunion::GetFunIndex(strFunName);
	//	if (nCallBack >= 0)
	//	{
	//		CCallBackExeCode* pCallCode = CExeCodeMgr::GetInstance()->New<CCallBackExeCode>(m_unBeginSoureIndex);
	//		pCallCode->cParmSize = (unsigned char)vParams.size();
	//		pCallCode->cResultRegister = cRegisterIndex;
	//		pCallCode->unFunIndex = nCallBack;
	//		vOut.AddCode(pCallCode);

	//		return true;
	//	}
	//	int nFunIndex = CScriptCodeLoader::GetInstance()->GetCodeIndex(strFunName.c_str());
	//	if (nFunIndex >= 0)
	//	{
	//		CCallScriptExeCode* pCallCode = CExeCodeMgr::GetInstance()->New<CCallScriptExeCode>(m_unBeginSoureIndex);
	//		pCallCode->cParmSize = (unsigned char)vParams.size();
	//		pCallCode->cResultRegister = cRegisterIndex;
	//		pCallCode->unFunIndex = nFunIndex;
	//		vOut.AddCode(pCallCode);

	//		return true;
	//	}
	//	return false;
	//}
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
			AddErrorInfo(nextWord.nSourceWordsIndex, "CCallFunICode:format error");
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
					AddErrorInfo(nextWord.nSourceWordsIndex, "CCallFunICode:param format error");
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
					AddErrorInfo(
						nextWord.nSourceWordsIndex,
						"CCallFunICode:format error,Parameter");
					RevertAll();
					return false;
				}
			}
		}
		nCallBack = CScriptCallBackFunion::GetFunIndex(strFunName);
		if (nCallBack < 0)
		{
			nFunIndex = CScriptCodeLoader::GetInstance()->GetCodeIndex(strFunName.c_str());
		}
		return true;
	}

	int CCallFunICode::Run(CScriptExecBlock* pBlock)
	{
		int& state = pBlock->GetRunState(m_nRunStateIndex);

		for (int i = state; i < vParams.size(); i++)
		{
			CBaseICode* pCode = vParams[i];
			if (pCode)
			{
				int result = pCode->Run(pBlock);
				if (result != CScriptExecBlock::ERESULT_CONTINUE)
				{
					return result;
				}
			}
			state++;
		}
		if (state == vParams.size())
		{
			state++;
			if (pBlock->registerStack.nIndex < vParams.size())
			{
				//TODO 输出错误
				return CScriptExecBlock::ERESULT_ERROR;
			}
			int nResult = CScriptExecBlock::ERESULT_CONTINUE;

			unsigned nBegin = pBlock->registerStack.nIndex - vParams.size();
			if (nCallBack >= 0)
			{
				CACHE_NEW(CScriptCallState, pCallState, pBlock->m_pMaster);
				if (pCallState == nullptr)
				{
					//TODO 输出错误
					return CScriptExecBlock::ERESULT_ERROR;
				}
				STACK_MOVE_ALL_BACK(pCallState->m_stackRegister, pBlock->registerStack, nBegin);
				switch (pBlock->m_pMaster->CallFun_CallBack(pBlock->m_pMaster->m_pMachine, nCallBack, pCallState))
				{
				case ECALLBACK_ERROR:
					nResult = CScriptExecBlock::ERESULT_ERROR;
					break;
				case ECALLBACK_WAITING:
					nResult = CScriptExecBlock::ERESULT_WAITING;
					break;
				case ECALLBACK_NEXTCONTINUE:
					nResult = CScriptExecBlock::ERESULT_NEXTCONTINUE;
					break;
				case ECALLBACK_FINISH:
					if (pCallState->GetResult())
					{
						STACK_PUSH_COPY(pBlock->registerStack, pCallState->GetResult());
					}
					nResult = CScriptExecBlock::ERESULT_CONTINUE;
					break;
				}
				CACHE_DELETE(pCallState);
			}
			else
			{
				if (nFunIndex < 0)
				{
					nFunIndex = CScriptCodeLoader::GetInstance()->GetCodeIndex(strFunName.c_str());
					if (nFunIndex < 0)
					{
						//TODO 输出错误
						return CScriptExecBlock::ERESULT_ERROR;
					}
				}
				switch (pBlock->m_pMaster->CallFun_Script(pBlock->m_pMaster->m_pMachine, nFunIndex, pBlock->registerStack, vParams.size()))
				{
				case ECALLBACK_ERROR:
					nResult = CScriptExecBlock::ERESULT_ERROR;
					break;
				case ECALLBACK_WAITING:
					nResult = CScriptExecBlock::ERESULT_WAITING;
					break;
				case ECALLBACK_NEXTCONTINUE:
					nResult = CScriptExecBlock::ERESULT_NEXTCONTINUE;
					break;
				case ECALLBACK_FINISH:
					nResult = CScriptExecBlock::ERESULT_CONTINUE;
					break;
				}
				for (unsigned char i = 0; i < vParams.size(); i++)
				{
					CBaseVar* pVar = nullptr;
					STACK_POP(pBlock->registerStack, pVar);
					SCRIPTVAR_RELEASE(pVar)
				}
			}
			if (nResult != CScriptExecBlock::ERESULT_CONTINUE)
				return nResult;
		}
		state = 0;
		pBlock->RevertRunState(m_nRunStateIndex);

		return CScriptExecBlock::ERESULT_CONTINUE;
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
			AddErrorInfo(nextWord.nSourceWordsIndex, "CCallClassFunICode:format error ->");
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
			stGlobalVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(strClassVarName);
			if (pGlobalVar)
			{
				isGlobal = true;
				varIndex = pGlobalVar->index;
				CScriptGlobalVarMgr::GetInstance()->Revert(pGlobalVar);
				if (pGlobalVar->pVar)
				{
					nClassType = pGlobalVar->pVar->GetType();
				}
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
			AddErrorInfo(nextWord.nSourceWordsIndex, "CCallFunICode:format error");
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
					AddErrorInfo(nextWord.nSourceWordsIndex, "CCallFunICode:param format error");
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
					AddErrorInfo(
						nextWord.nSourceWordsIndex,
						"CCallFunICode:format error,Parameter");
					RevertAll();
					return false;
				}
			}
		}
		return true;
	}

	int CCallClassFunICode::RunClassFun(CScriptExecBlock* pBlock,CScriptPointInterface* pPoint, int funIndex)
	{
		int result = CScriptExecBlock::ERESULT_ERROR;
		unsigned int nBegin = pBlock->registerStack.nIndex - vParams.size();
		CACHE_NEW(CScriptCallState, pCallState, pBlock->m_pMaster);
		if (pCallState)
		{
			STACK_MOVE_ALL_BACK(pCallState->m_stackRegister, pBlock->registerStack, nBegin);
			switch (pPoint->RunFun(funIndex, pCallState))
			{
			case ECALLBACK_ERROR:
				result = CScriptExecBlock::ERESULT_ERROR;
				break;
			case ECALLBACK_WAITING:
				result = CScriptExecBlock::ERESULT_WAITING;
				break;
			case ECALLBACK_NEXTCONTINUE:
				result = CScriptExecBlock::ERESULT_NEXTCONTINUE;
				break;
			case ECALLBACK_FINISH:
				//执行完将结果放入寄存器
				if (pCallState->GetResult())
				{
					STACK_PUSH_COPY(pBlock->registerStack, pCallState->GetResult());
				}
				result = CScriptExecBlock::ERESULT_CONTINUE;
				break;
			}
		}
		CACHE_DELETE(pCallState);
		return result;
	}

	int CCallClassFunICode::Run(CScriptExecBlock* pBlock)
	{
		int& state = pBlock->GetRunState(m_nRunStateIndex);

		for (int i = state; i < vParams.size(); i++)
		{	
			CBaseICode* pCode = vParams[i];
			if (pCode)
			{
				int result = pCode->Run(pBlock);
				if (result != CScriptExecBlock::ERESULT_CONTINUE)
				{
					return result;
				}
			}
			state++;
		}
		if (state == vParams.size())
		{
			state++;
			if (pBlock->registerStack.nIndex < vParams.size())
			{
				//TODO 输出错误
				return CScriptExecBlock::ERESULT_ERROR;
			}
			int result = CScriptExecBlock::ERESULT_CONTINUE;
			int nFunIndexRunTime = nFunIndex;

			if (isGlobal)
			{
				stGlobalVar* pGlobalVar = CScriptGlobalVarMgr::GetInstance()->Get(varIndex);
				if (pGlobalVar)
				{
					CPointVar* pPoint = dynamic_cast<CPointVar*>(pGlobalVar->pVar);
					if (pPoint)
					{
						if (pPoint->ToPoint())
						{
							if (nFunIndexRunTime < 0)
							{
								nFunIndexRunTime = pPoint->ToPoint()->GetClassFunIndex(strFunName);
							}
							result = RunClassFun(pBlock, pPoint->ToPoint(), nFunIndexRunTime);
						}
						else
						{
							result = CScriptExecBlock::ERESULT_ERROR;
							//TODO 输出错误信息
						}
					}
					else
					{
						result = CScriptExecBlock::ERESULT_ERROR;
						//TODO 输出错误信息
					}
					CScriptGlobalVarMgr::GetInstance()->Revert(pGlobalVar);
				}
				else
				{
					result = CScriptExecBlock::ERESULT_ERROR;
					//TODO 输出错误信息
				}
			}
			else
			{
				CBaseVar* pLoacllVar = pBlock->GetLoaclVar(varIndex);
				if (pLoacllVar)
				{
					CPointVar* pPoint = dynamic_cast<CPointVar*>(pLoacllVar);
					if (pPoint)
					{
						if (pPoint->ToPoint())
						{
							if (nFunIndexRunTime < 0)
							{
								nFunIndexRunTime = pPoint->ToPoint()->GetClassFunIndex(strFunName);
							}
							result = RunClassFun(pBlock, pPoint->ToPoint(), nFunIndexRunTime);
						}
						else
						{
							result = CScriptExecBlock::ERESULT_ERROR;
							//TODO 输出错误信息
						}
					}
					else
					{
						result = CScriptExecBlock::ERESULT_ERROR;
						//TODO 输出错误信息
					}
				}
				else
				{
					result = CScriptExecBlock::ERESULT_ERROR;
					//TODO 输出错误信息
				}
			}
			if (result != CScriptExecBlock::ERESULT_CONTINUE)
				return result;
		}
		state = 0;
		pBlock->RevertRunState(m_nRunStateIndex);
		return CScriptExecBlock::ERESULT_CONTINUE;
	}


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
					AddErrorInfo(nextWord.nSourceWordsIndex, "CSentenceICode:format error");
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
					AddErrorInfo(nextWord.nSourceWordsIndex, "CSentenceICode:format error");
					RevertAll();
					return false;
				}
			}
		}
		return true;
	}

	int CSentenceICode::Run(CScriptExecBlock* pBlock)
	{
		int& state = pBlock->GetRunState(m_nRunStateIndex);
		for (int i = state; i < vData.size(); i++)
		{
			CBaseICode* pCode = vData[i];
			if (pCode)
			{
				int result = pCode->Run(pBlock);
				if (result != CScriptExecBlock::ERESULT_CONTINUE)
				{
					return result;
				}
			}
			state++;
		}
		state = 0;
		pBlock->RevertRunState(m_nRunStateIndex);
		return CScriptExecBlock::ERESULT_CONTINUE;
	}

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
				pSetCode->AddICode(0, pOperNode->pRightOperand);
				pSetCode->SetFather(pOperNode->GetFather());
				*pNode = pSetCode;
				return true;

			}
			else if (pOperNode->pLeftOperand && pOperNode->pLeftOperand->GetType() == E_I_CODE_LOADVAR && pOperNode->pRightOperand)
			{
				CICodeMgr<CSaveVarICode> mgr;
				CLoadVarICode* pLeftOperand = (CLoadVarICode*)pOperNode->pLeftOperand;
				CSaveVarICode* pSaveCode = (CSaveVarICode*)mgr.New(pCompiler, pLeftOperand->m_unBeginSoureIndex);

				pSaveCode->nSaveType = pLeftOperand->nLoadType;
				pSaveCode->LoaclVarIndex = pLeftOperand->LoaclVarIndex;
				pSaveCode->GlobalVarIndex = pLeftOperand->GlobalVarIndex;
				if (CheckOperatorTree(&pOperNode->pRightOperand, pCompiler) == false)
				{
					return false;
				}
				pSaveCode->AddICode(0, pOperNode->pRightOperand);
				pSaveCode->SetFather(pOperNode->GetFather());
				*pNode = pSaveCode;
				return true;
			}
		}


		if (pOperNode->pLeftOperand && pOperNode->pLeftOperand->GetType() == E_I_CODE_OPERATOR)
		{
			if (CheckOperatorTree(&pOperNode->pLeftOperand,pCompiler) == false)
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
	int CExpressionICode::Run(CScriptExecBlock* pBlock)
	{
		if (m_pRoot)
		{
			return m_pRoot->Run(pBlock);
		}
		else if (pOperandCode)
		{
			return pOperandCode->Run(pBlock);
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
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
			AddErrorInfo(nextWord.nSourceWordsIndex, "CIfICode:if format error");
			RevertAll();
			return false;
		}

		if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_EXPRESSION, this, E_COND))
		{
			AddErrorInfo(nextWord.nSourceWordsIndex, "CIfICode:if condition format error");
			RevertAll();
			return false;
		}

		GetWord(nextWord);
		if (nextWord.word != ")")
		{
			AddErrorInfo(nextWord.nSourceWordsIndex, "CIfICode:if format error");
			RevertAll();
			return false;
		}

		if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_STATEMENT, this, E_TRUE))
		{
			AddErrorInfo(nextWord.nSourceWordsIndex, "CIfICode:if true block format error");
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
			AddErrorInfo(nextWord.nSourceWordsIndex, "CIfICode:if false block format error");
			RevertAll();
			return false;
		}

		return true;
	}

	bool CIfICode::CheckCondVar(CBaseVar* pVar)
	{
		if (pVar->GetType() == CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			CIntVar* pIntVar = dynamic_cast<CIntVar*>(pVar);
			if (pIntVar && pIntVar->ToInt() != 0)
			{
				return true;
			}
		}
		else if (pVar->GetType() == CScriptClassInfo<CStringVar>::GetInstance().nClassType)
		{

		}
		else
		{
			CPointVar* pPoint = dynamic_cast<CPointVar*>(pVar);
			if (pPoint && pPoint->ToPoint())
			{
				return true;
			}
		}
		return false;
	}

	int CIfICode::Run(CScriptExecBlock* pBlock)
	{
		int& state = pBlock->GetRunState(m_nRunStateIndex);
		if (state == 0)
		{	
			int result = pCondCode->Run(pBlock);
			if (result != CScriptExecBlock::ERESULT_CONTINUE)
			{
				return result;
			}
			state++;
		}
		if (state == 1)
		{
			state++;
			CBaseVar* pCondVar = nullptr;
			STACK_POP(pBlock->registerStack, pCondVar);
			if (CheckCondVar(pCondVar))
			{
				state = 2;
			}
			else
			{
				state = 3;
			}
			SCRIPTVAR_RELEASE(pCondVar);
		}
		int result = CScriptExecBlock::ERESULT_CONTINUE;
		if (state == 2)
		{
			result = pTureCode->Run(pBlock);
			if (result != CScriptExecBlock::ERESULT_CONTINUE)
			{
				return result;
			}
			state = 4;
		}
		if (state == 3)
		{
			result = pFalseCode->Run(pBlock);
			if (result != CScriptExecBlock::ERESULT_CONTINUE)
			{
				return result;
			}
			state = 4;
		}
		
		state = 0;
		pBlock->RevertRunState(m_nRunStateIndex);
		return result;
	}

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
			AddErrorInfo(nextWord.nSourceWordsIndex, "CWhileICode:if format error");
			RevertAll();
			return false;
		}

		if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_EXPRESSION, this, E_COND))
		{
			AddErrorInfo(nextWord.nSourceWordsIndex, "CWhileICode:if condition format error");
			RevertAll();
			return false;
		}

		GetWord(nextWord);
		if (nextWord.word != ")")
		{
			AddErrorInfo(nextWord.nSourceWordsIndex, "CWhileICode:while format error");
			RevertAll();
			return false;
		}

		if (!pCompiler->RunCompileState(vIn, CScriptCompiler::E_CODE_SCOPE_STATEMENT, this, E_BLOCK))
		{
			AddErrorInfo(nextWord.nSourceWordsIndex, "CWhileICode:while block format error");
			RevertAll();
			return false;
		}

		return true;
	}
	bool CWhileICode::CheckCondVar(CBaseVar* pVar)
	{
		if (pVar->GetType() == CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			CIntVar* pIntVar = dynamic_cast<CIntVar*>(pVar);
			if (pIntVar && pIntVar->ToInt() != 0)
			{
				return true;
			}
		}
		else if (pVar->GetType() == CScriptClassInfo<CStringVar>::GetInstance().nClassType)
		{

		}
		else
		{
			CPointVar* pPoint = dynamic_cast<CPointVar*>(pVar);
			if (pPoint && pPoint->ToPoint())
			{
				return true;
			}
		}
		return false;
	}
	int CWhileICode::Run(CScriptExecBlock* pBlock)
	{
		int& state = pBlock->GetRunState(m_nRunStateIndex);
		while (true)
		{
			if (state == 0)
			{
				int result = pCondCode->Run(pBlock);
				if (result != CScriptExecBlock::ERESULT_CONTINUE)
				{
					return result;
				}
				state++;
			}
			if (state == 1)
			{
				CBaseVar* pCondVar = nullptr;
				STACK_POP(pBlock->registerStack, pCondVar);
				if (CheckCondVar(pCondVar))
				{
					state = 2;
				}
				else
				{
					state = 3;
				}
				SCRIPTVAR_RELEASE(pCondVar);
			}
			if (state == 2)
			{
				int result = pBodyCode->Run(pBlock);
				switch (result)
				{
				case CScriptExecBlock::ERESULT_LOOP_HEAD:
					state = 0;
					break;
				case CScriptExecBlock::ERESULT_LOOP_END:
					state = 3;
					break;
				case CScriptExecBlock::ERESULT_CONTINUE:
					state = 4;
					break;
				default:
					return result;
				}
				state = 4;
			}
			if (state == 3)
			{
				break;
			}
			state = 0;
		}

		pBlock->RevertRunState(m_nRunStateIndex);
		return CScriptExecBlock::ERESULT_CONTINUE;
	}
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
			pCompiler->AddErrorInfo(nextWord.nSourceWordsIndex, "CContinueICode:format error");
			RevertAll();
			return false;
		}
		return true;
	}
	int CContinueICode::Run(CScriptExecBlock* pBlock)
	{
		return CScriptExecBlock::ERESULT_LOOP_HEAD;
	}
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
			pCompiler->AddErrorInfo(nextWord.nSourceWordsIndex, "CBreakICode:format error");
			RevertAll();
			return false;
		}
		return true;
	}
	int CBreakICode::Run(CScriptExecBlock* pBlock)
	{
		return CScriptExecBlock::ERESULT_LOOP_END;
	}
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
			AddErrorInfo(nextWord.nSourceWordsIndex, "CReturnICode:expression format error");
			RevertAll();
			return false;
		}

		GetWord(nextWord);
		if (nextWord.word != ";")
		{
			pCompiler->AddErrorInfo(nextWord.nSourceWordsIndex, "CReturnICode:format error");
			RevertAll();
			return false;
		}
		return true;
	}

	int CReturnICode::Run(CScriptExecBlock* pBlock)
	{
		int& state = pBlock->GetRunState(m_nRunStateIndex);
		if (state == 0)
		{
			int result = pBodyCode->Run(pBlock);
			if (result != CScriptExecBlock::ERESULT_CONTINUE)
			{
				return result;
			}
			state++;
		}
		state = 0;
		pBlock->RevertRunState(m_nRunStateIndex);
		return CScriptExecBlock::ERESULT_END;
	}


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

	int CNewICode::Run(CScriptExecBlock* pBlock)
	{
		if (nClassType < 0)
		{
			nClassType = CScriptVarTypeMgr::GetInstance()->GetVarType(strClassType);
		}
		auto pPoint = CScriptVarTypeMgr::GetInstance()->NewObject(nClassType);
		CBaseVar* pVar = CScriptVarTypeMgr::GetInstance()->GetVar(nClassType);
		if (pVar->Set(pPoint))
		{
			STACK_PUSH_MOVE(pBlock->registerStack, pVar);
		}
		else
		{
			CScriptVarTypeMgr::GetInstance()->ReleaseVar(pVar);
			CScriptVarTypeMgr::GetInstance()->ReleaseObject(pPoint);
		}
		return CScriptExecBlock::ERESULT_CONTINUE;
	}

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
			pCompiler->AddErrorInfo(nextWord.nSourceWordsIndex, "CBracketsICode:expression error");
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		if (nextWord.word != ")")
		{
			pCompiler->AddErrorInfo(nextWord.nSourceWordsIndex, "CBracketsICode:format error");
			RevertAll();
			return false;
		}
		return true;
	}
	int CBracketsICode::Run(CScriptExecBlock* pBlock)
	{
		return m_pBody->Run(pBlock);
	}
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