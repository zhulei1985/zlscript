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

namespace zlscript
{
	CBaseICode* CBaseICode::GetFather()
	{
		return m_pFather;
	}
	void CBaseICode::SetFather(CBaseICode* pCode)
	{
		m_pFather = pCode;
	}
	CScriptCodeLoader* CBaseICode::GetLoader()
	{
		return m_pLoader;
	}
	void CBaseICode::SetLoader(CScriptCodeLoader* pLoader)
	{
		m_pLoader = pLoader;
	}
	bool CBaseICode::DefineTempVar(std::string VarType, std::string VarName)
	{
		if (m_pFather)
		{
			return m_pFather->DefineTempVar(VarType, VarName);
		}
		return false;
	}
	bool CBaseICode::CheckTempVar(const char* pVarName)
	{
		if (GetTempVarIndex(pVarName) != g_nTempVarIndexError)
		{
			return true;
		}

		return false;
	}
	unsigned int CBaseICode::GetTempVarIndex(const char* pVarName)
	{
		if (GetFather())
		{
			return GetFather()->GetTempVarIndex(pVarName);
		}
		return g_nTempVarIndexError;
	}

	VarInfo* CBaseICode::GetTempVarInfo(const char* pVarName)
	{
		if (GetFather())
		{
			return GetFather()->GetTempVarInfo(pVarName);
		}
		return nullptr;
	}

	void CBaseICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (pCode)
		{
			pCode->SetFather(this);
		}
	}

	CBaseICode* CBaseICode::GetICode(int nType, int index)
	{
		return nullptr;
	}

	void CBaseICode::AddErrorInfo(unsigned int pos, std::string error)
	{
		if (m_pLoader)
			m_pLoader->AddErrorInfo(pos, error);
	}

	bool CDefGlobalVarICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		if (m_pLoader == nullptr)
		{
			return false;
		}

		GetNewWord(strType);
		unsigned short unVarType = 0;
		unsigned short unClassType = 0;
		unVarType = m_pLoader->GetVarType(strType.word, unClassType);
		if (unVarType == EScriptVal_None)
		{
			AddErrorInfo(
				strType.nSourceWordsIndex,
				"CDefGlobalVarICode:type is none");
			RevertAll();
			return false;
		}
		GetNewWord(strName);

		if (!m_pLoader->CheckVarName(strName.word))
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
			if (!m_pLoader->AddGlobalVar(strName.word, unVarType, unClassType))
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
			GetNewWord(strVal);
			StackVarInfo defVar;//默认值
			defVar.cType = (char)unVarType;
			switch (unVarType)
			{
			case EScriptVal_Int:
			{
				defVar.Int64 = _atoi64(strVal.word.c_str());
			}
			break;
			case EScriptVal_Double:
			{
				defVar.Double = atof(strVal.word.c_str());
			}
			break;
			case EScriptVal_String:
			{
				defVar.Int64 = StackVarInfo::s_strPool.NewString(strVal.word.c_str());
			}
			break;
			case EScriptVal_ClassPoint:
			{
				if (strVal.word == "nullptr")
				{
					defVar.Int64 = 0;
				}
				else if (strVal.word == "new")
				{
					GetNewWord(strType);
					int dwClassIndex = CScriptSuperPointerMgr::GetInstance()->GetClassType(strType.word);
					CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(dwClassIndex);
					if (pMgr)
					{
						auto pNewPoint = pMgr->New(SCRIPT_NO_USED_AUTO_RELEASE);
						defVar = pNewPoint;
					}
					else
					{
						AddErrorInfo(
							strName.nSourceWordsIndex,
							"CDefGlobalVarICode:global new class error");
						RevertAll();
						return false;
					}
				}
			}
			}
			if (!m_pLoader->AddGlobalVar(strName.word, unVarType, unClassType))
			{
				AddErrorInfo(
					strName.nSourceWordsIndex,
					"CDefGlobalVarICode:global var cannot add");
				RevertAll();
				return false;
			}
			if (!m_pLoader->SetGlobalVar(strName.word, defVar))
			{
				AddErrorInfo(
					strName.nSourceWordsIndex,
					"CDefGlobalVarICode:global var cannot set");
				RevertAll();
				return false;
			}
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
	bool CDefTempVarICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		if (m_pLoader == nullptr)
		{
			return false;
		}

		GetNewWord(strType);
		unsigned short unVarType = 0;
		unsigned short unClassType = 0;
		unVarType = m_pLoader->GetVarType(strType.word, unClassType);
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
		if (!m_pLoader->CheckVarName(strName.word))
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
			if (!this->DefineTempVar(strType.word, strName.word))
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
			if (!this->DefineTempVar(strType.word, strName.word))
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
	bool CFunICode::DefineTempVar(std::string VarType, std::string VarName)
	{
		if (m_mapVarNameAndType.find(VarName) != m_mapVarNameAndType.end())
			return false;
		m_mapVarNameAndType[VarName] = VarType;
		m_vecTempVarOrder.push_back(VarName);
		return true;
	}

	//void CFunICode::SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex)
	//{
	//	if (pVarName == nullptr)
	//	{
	//		return;
	//	}
	//	if (m_mapTempVarIndex.find(pVarName) == m_mapTempVarIndex.end())
	//	{
	//		auto& var = m_mapTempVarIndex[pVarName];
	//		var.cGlobal = 0;//局部变量
	//		var.cType = nType;
	//		var.dwPos = nIndex;
	//		var.wExtend = ClassIndex;
	//		m_vecTempVarOrder.push_back(pVarName);
	//	}

	//}
	unsigned int CFunICode::GetTempVarIndex(const char* pVarName)
	{
		auto it = m_mapTempVarIndex.find(pVarName);
		if (it != m_mapTempVarIndex.end())
		{
			return it->second.dwPos;
		}
		return g_nTempVarIndexError;
	}

	VarInfo* CFunICode::GetTempVarInfo(const char* pVarName)
	{
		auto it = m_mapTempVarIndex.find(pVarName);
		if (it != m_mapTempVarIndex.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	bool CFunICode::MakeExeCode(tagCodeData& vOut)
	{
		//注册临时变量
		for (unsigned int i = 0; i < m_vecTempVarOrder.size(); i++)
		{
			unsigned int index = vOut.vNumVar.size();
			StackVarInfo varInfo;
			vOut.vNumVar.push_back(varInfo);
			auto& var = m_mapTempVarIndex[m_vecTempVarOrder[i]];
			var.cGlobal = 0;//局部变量
			var.dwPos = index;
			var.cType = CScriptCodeLoader::GetInstance()->GetVarType(m_mapVarNameAndType[m_vecTempVarOrder[i]], var.wExtend);
		}

		//首先是参数的赋值
		//for (int i = 0; i < m_vecTempVarOrder.size(); i++)
		//{
		//	CodeStyle code(m_unBeginSoureIndex);
		//	code.qwCode = 0;
		//	code.cSign = ESIGN_POS_LOACL_VAR;
		//	code.wInstruct = ECODE_POP;
		//	code.cExtend = 0;
		//	code.dwPos = GetTempVarIndex(m_vecTempVarOrder[i].c_str());
		//	vOut.vCodeData.push_back(code);
		//}
		for (unsigned int i = 0; i < vBodyCode.size(); i++)
		{
			if (vBodyCode[i])
			{
				vBodyCode[i]->MakeExeCode(vOut);
			}
		}

		return true;
	}

	bool CFunICode::LoadAttribute(SentenceSourceCode& vIn)
	{
		SignToPos();
		GetNewWord(nextWord);
		if (m_mapFunAttribute.find(nextWord.word) != m_mapFunAttribute.end())
		{
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"CFunICode:Attribute already exists");
			RevertAll();
			return false;
		}
		StackVarInfo& var = m_mapFunAttribute[nextWord.word];
		GetWord(nextWord);
		if (nextWord.word == "(")
		{
			GetWord(nextWord);
			switch (m_pLoader->GetVarType(nextWord))
			{
			case EScriptVal_Int:
			{
				var.Int64 = _atoi64(nextWord.word.c_str());
			}
			break;
			case EScriptVal_Double:
			{
				var.Double = atof(nextWord.word.c_str());
			}
			break;
			case EScriptVal_String:
			{
				var.Int64 = StackVarInfo::s_strPool.NewString(nextWord.word.c_str());
			}
			break;
			default:
				AddErrorInfo(
					nextWord.nSourceWordsIndex,
					"CFunICode:Attribute var type error");
				RevertAll();
				return false;
			}
			GetWord(nextWord);
			if (nextWord.word != ")")
			{
				AddErrorInfo(
					nextWord.nSourceWordsIndex,
					"CFunICode:Attribute format error");
				RevertAll();
				return false;
			}
		}
		else
		{
			RevertOne();
		}
		return true;
	}

	bool CFunICode::LoadDefineFunctionParameter(SentenceSourceCode& vIn)
	{
		SignToPos();
		GetNewWord(strType);
		GetNewWord(strName);
		if (!DefineTempVar(strType.word, strName.word))
		{
			AddErrorInfo(
				strType.nSourceWordsIndex,
				"CFunICode:DefineTempVar(defining temporary variable error)");
			RevertAll();
			return false;
		}
		return true;
	}

	bool CFunICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		if (m_pLoader == nullptr)
		{
			return false;
		}

		GetNewWord(nextWord);
		while (nextWord.word == "@")
		{
			if (!LoadAttribute(vIn))
			{
				RevertAll();
				return false;
			}

			GetWord(nextWord);
		}

		std::string strType = nextWord.word;
		unsigned short unVarType = 0;
		unsigned short unClassType = 0;
		unVarType = m_pLoader->GetVarType(strType, unClassType);
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
		GetWord(nextWord);
		if (nextWord.word != ")")
		{
			RevertOne();
			while (true)
			{
				if (!LoadDefineFunctionParameter(vIn))
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
				if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_STATEMENT, this, 0))
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
		m_pLoader->SetFunICode(strName.word, this);
		return true;
	}

	void CFunICode::AddICode(int nType, CBaseICode* pCode)
	{
		vBodyCode.push_back(pCode);
		CBaseICode::AddICode(nType, pCode);
	}

	bool CBlockICode::DefineTempVar(std::string VarType, std::string VarName)
	{
		if (m_mapVarNameAndType.find(VarName) != m_mapVarNameAndType.end())
			return false;
		m_mapVarNameAndType[VarName] = VarType;
		return true;
	}

	bool CBlockICode::CheckTempVar(const char* pVarName)
	{
		auto it = m_mapTempVarIndex.find(pVarName);
		if (it != m_mapTempVarIndex.end())
		{
			return true;
		}
		return false;
	}

	//void CBlockICode::SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex)
	//{
	//	if (pVarName == nullptr)
	//	{
	//		return;
	//	}
	//	if (m_mapTempVarIndex.find(pVarName) == m_mapTempVarIndex.end())
	//	{
	//		auto &var = m_mapTempVarIndex[pVarName];
	//		var.cGlobal = 0;//局部变量
	//		var.cType = nType;
	//		var.dwPos = nIndex;
	//		var.wExtend = ClassIndex;
	//	}
	//}

	unsigned int CBlockICode::GetTempVarIndex(const char* pVarName)
	{
		auto it = m_mapTempVarIndex.find(pVarName);
		if (it != m_mapTempVarIndex.end())
		{
			return it->second.dwPos;
		}
		if (GetFather())
		{
			return GetFather()->GetTempVarIndex(pVarName);
		}
		return g_nTempVarIndexError;
	}

	VarInfo* CBlockICode::GetTempVarInfo(const char* pVarName)
	{
		auto it = m_mapTempVarIndex.find(pVarName);
		if (it != m_mapTempVarIndex.end())
		{
			return &it->second;
		}
		if (GetFather())
		{
			return GetFather()->GetTempVarInfo(pVarName);
		}
		return nullptr;
	}

	bool CBlockICode::MakeExeCode(tagCodeData& vOut)
	{
		//注册临时变量
		for (auto it = m_mapVarNameAndType.begin(); it != m_mapVarNameAndType.end(); it++)
		{
			unsigned int index = vOut.vNumVar.size();
			StackVarInfo varInfo;
			vOut.vNumVar.push_back(varInfo);
			auto& var = m_mapTempVarIndex[it->first];
			var.cGlobal = 0;//局部变量
			var.dwPos = index;
			var.cType = CScriptCodeLoader::GetInstance()->GetVarType(it->second, var.wExtend);
		}
		for (auto it = m_vICode.begin(); it != m_vICode.end(); it++)
		{
			if (*it)
			{
				(*it)->MakeExeCode(vOut);
			}
		}
		return true;
	}

	bool CBlockICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		if (m_pLoader == nullptr)
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
			if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_STATEMENT, this, 0))
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

	char CLoadVarICode::AnalysisVar(tagCodeData& vOut, unsigned int& pos)
	{
		char cType = 0;
		if (m_word.nFlag == E_WORD_FLAG_STRING)
		{
			cType = ESIGN_POS_CONST_STRING;
			pos = vOut.vStrConst.size();
			vOut.vStrConst.push_back(m_word.word);
		}
		else
		{
			//检查是否是临时变量
			unsigned int nIndex = GetTempVarIndex(m_word.word.c_str());
			if (nIndex != g_nTempVarIndexError)
			{
				cType = ESIGN_POS_LOACL_VAR;
				pos = nIndex;
			}
			else
			{
				//检查是否是全局变量
				VarInfo* pGlobalVar = CScriptCodeLoader::GetInstance()->GetGlobalVarInfo(m_word.word);
				if (pGlobalVar)
				{
					cType = ESIGN_POS_GLOBAL_VAR;
					pos = pGlobalVar->dwPos;
				}
				else
				{
					if (m_word.word == "nullptr")
					{
						cType = ESIGN_POINT_NULLPTR;
					}
					else
					{
						bool isFloat = false;
						for (unsigned int i = 0; i < m_word.word.size(); i++)
						{
							if ((m_word.word[i] >= '0' && m_word.word[i] <= '9'))
							{
								//是数字
							}
							else if (m_word.word[i] == '.')
							{
								//是点
								isFloat = true;
							}
						}
						if (isFloat)
						{
							cType = ESIGN_POS_CONST_FLOAT;
							pos = vOut.vFloatConst.size();
							vOut.vFloatConst.push_back(stod(m_word.word));
						}
						else
						{
							__int64 nVal = _atoi64(m_word.word.c_str());
							if ((nVal & 0xffffffff) == nVal)
							{
								cType = ESIGN_VALUE_INT;
								pos = nVal;
							}
							else
							{
								cType = ESIGN_POS_CONST_INT64;
								pos = vOut.vInt64Const.size();
								vOut.vInt64Const.push_back(nVal);
							}
						}
					}
				}
			}
		}
		return cType;
	}

	bool CLoadVarICode::MakeExeCode(tagCodeData& vOut)
	{
		CLoadExeCode* pCode = CExeCodeMgr::GetInstance()->New<CLoadExeCode>(m_unBeginSoureIndex);
		pCode->cResultRegister = cRegisterIndex;
		pCode->cType = AnalysisVar(vOut, pCode->dwPos);

		vOut.AddCode(pCode);
		return true;
	}
	bool CLoadVarICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		GetNewWord(nextWord);
		m_word = nextWord;
		return true;
	}
	bool CSaveVarICode::MakeExeCode(tagCodeData& vOut)
	{
		char cType = 0;
		unsigned int pos = 0;
		unsigned int nIndex = GetTempVarIndex(m_word.word.c_str());
		if (nIndex != g_nTempVarIndexError)
		{
			cType = ESIGN_POS_LOACL_VAR;
			pos = nIndex;
		}
		else
		{
			//检查是否是全局变量
			VarInfo* pGlobalVar = CScriptCodeLoader::GetInstance()->GetGlobalVarInfo(m_word.word);
			if (pGlobalVar)
			{
				cType = ESIGN_POS_GLOBAL_VAR;
				pos = pGlobalVar->dwPos;
			}
		}
		if (cType == 0)
		{
			return false;
		}
		pRightOperand->SetRegisterIndex(cRegisterIndex);
		pRightOperand->MakeExeCode(vOut);

		CMoveExeCode* pCode = CExeCodeMgr::GetInstance()->New<CMoveExeCode>(m_unBeginSoureIndex);
		pCode->cVarRegister = cRegisterIndex;
		pCode->cType = cType;
		pCode->dwPos = pos;
		vOut.AddCode(pCode);
		return true;
	}
	bool CSaveVarICode::Compile(SentenceSourceCode& vIn)
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
	//void CPush2StackICode::MakeExeCode(stCodeData& vOut)
	//{
	//	CodeStyle code(m_unBeginSoureIndex);
	//	code.qwCode = 0;
	//	code.wInstruct = ECODE_PUSH;
	//	code.cSign = cSource;
	//	code.cExtend = 0;
	//	code.dwPos = nPos;
	//	vOut.push_back(code);
	//}
	//void CPop4StackICode::MakeExeCode(stCodeData& vOut)
	//{
	//	CodeStyle code(m_unBeginSoureIndex);
	//	code.qwCode = 0;
	//	code.wInstruct = ECODE_POP;
	//	code.cSign = cDestination;
	//	code.cExtend = cRegisterIndex;
	//	code.dwPos = nPos;
	//	vOut.push_back(code);
	//}
	bool CGetClassParamICode::MakeExeCode(tagCodeData& vOut)
	{
		VarInfo* pVarInfo = GetTempVarInfo(strClassVarName.c_str());
		if (pVarInfo == nullptr)
		{
			//检查是否是全局变量
			pVarInfo = CScriptCodeLoader::GetInstance()->GetGlobalVarInfo(strClassVarName);
		}
		if (pVarInfo == nullptr)
		{
			return false;
		}
		if (pVarInfo->cType != EScriptVal_ClassPoint)
		{
			return false;
		}
		auto pParamInfo = CScriptSuperPointerMgr::GetInstance()->GetClassParamInfo(pVarInfo->wExtend, strParamName);
		if (pParamInfo == nullptr)
		{
			return false;
		}
		CLoadExeCode* pLoadCode = CExeCodeMgr::GetInstance()->New<CLoadExeCode>(m_unBeginSoureIndex);
		pLoadCode->cType = pVarInfo->cGlobal == 1 ? ESIGN_POS_GLOBAL_VAR : ESIGN_POS_LOACL_VAR;
		pLoadCode->cResultRegister = R_C;
		pLoadCode->dwPos = pVarInfo->dwPos;
		vOut.AddCode(pLoadCode);

		CGetClassParamExeCode* pGetCode = CExeCodeMgr::GetInstance()->New<CGetClassParamExeCode>(m_unBeginSoureIndex);
		pGetCode->cClassRegIndex = R_C;
		pGetCode->cResultRegister = cRegisterIndex;
		pGetCode->dwPos = pParamInfo->m_index;
		vOut.AddCode(pGetCode);
		return true;
	}
	bool CGetClassParamICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		GetNewWord(nextWord);
		strClassVarName = nextWord.word;
		GetWord(nextWord);
		if (nextWord.word != "->")
		{
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"CGetClassParamICode:format error ->");
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		strParamName = nextWord.word;
		return true;
	}
	bool CSetClassParamICode::MakeExeCode(tagCodeData& vOut)
	{
		VarInfo* pVarInfo = GetTempVarInfo(strClassVarName.c_str());
		if (pVarInfo == nullptr)
		{
			//检查是否是全局变量
			pVarInfo = CScriptCodeLoader::GetInstance()->GetGlobalVarInfo(strClassVarName);
		}
		if (pVarInfo == nullptr)
		{
			return false;
		}
		if (pVarInfo->cType != EScriptVal_ClassPoint)
		{
			return false;
		}
		auto pParamInfo = CScriptSuperPointerMgr::GetInstance()->GetClassParamInfo(pVarInfo->wExtend, strParamName);
		if (pParamInfo == nullptr)
		{
			return false;
		}
		if (pRightOperand)
		{
			pRightOperand->SetRegisterIndex(cRegisterIndex);
			pRightOperand->MakeExeCode(vOut);
		}
		CLoadExeCode* pLoadCode = CExeCodeMgr::GetInstance()->New<CLoadExeCode>(m_unBeginSoureIndex);
		pLoadCode->cType = pVarInfo->cGlobal == 1 ? ESIGN_POS_GLOBAL_VAR : ESIGN_POS_LOACL_VAR;
		pLoadCode->cResultRegister = R_C;
		pLoadCode->dwPos = pVarInfo->dwPos;
		vOut.AddCode(pLoadCode);

		CSetClassParamExeCode* pGetCode = CExeCodeMgr::GetInstance()->New<CSetClassParamExeCode>(m_unBeginSoureIndex);
		pGetCode->cClassRegIndex = R_C;
		pGetCode->cVarRegister = cRegisterIndex;
		pGetCode->dwPos = pParamInfo->m_index;
		vOut.AddCode(pGetCode);

		return true;
	}
	void CSetClassParamICode::AddICode(int nType, CBaseICode* pCode)
	{
		pRightOperand = pCode;
		CBaseICode::AddICode(0, pCode);
	}
	bool CSetClassParamICode::Compile(SentenceSourceCode& vIn)
	{
		//此类并不直接参与编译,表达式树整理时按情况替换GetClassParamICode
		return true;
	}
	bool CMinusICode::MakeExeCode(tagCodeData& vOut)
	{
		if (pRightOperand)
		{
			pRightOperand->SetRegisterIndex(R_A);
			pRightOperand->MakeExeCode(vOut);
		}

		CMinusExeCode* pCode = CExeCodeMgr::GetInstance()->New<CMinusExeCode>(m_unBeginSoureIndex);
		pCode->cResultRegister = cRegisterIndex;
		vOut.AddCode(pCode);

		return true;
	}
	void CMinusICode::AddICode(int nType, CBaseICode* pCode)
	{
		pRightOperand = pCode;
		CBaseICode::AddICode(0, pCode);
	}
	bool CMinusICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		GetNewWord(nextWord);
		if (nextWord.word != "-")
		{
			RevertAll();
			return false;
		}
		if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_MEMBER, this, 0))
		{
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"CGetClassParamICode:format error ->");
			RevertAll();
			return false;
		}
		return true;
	}
	bool COperatorICode::MakeExeCode(tagCodeData& vOut)
	{
		nOperatorCode = CScriptCodeLoader::GetInstance()->GetWordKey(strOperator);
		char cType = 0;
		unsigned int dwPos = 0;
		if (pLeftOperand)
		{
			pLeftOperand->SetRegisterIndex(R_A);
			pLeftOperand->MakeExeCode(vOut);
		}
		if (pRightOperand)
		{
			pRightOperand->SetRegisterIndex(R_B);
			if (pRightOperand->GetType() != E_I_CODE_LOADVAR)
			{
				CPushExeCode* pPushCode = CExeCodeMgr::GetInstance()->New<CPushExeCode>(m_unBeginSoureIndex);
				pPushCode->cType = ESIGN_REGISTER;
				pPushCode->dwPos = R_A;
				vOut.AddCode(pPushCode);

				pRightOperand->MakeExeCode(vOut);

				CPopExeCode* pPopCode = CExeCodeMgr::GetInstance()->New<CPopExeCode>(m_unBeginSoureIndex);
				pPopCode->cType = ESIGN_REGISTER;
				pPopCode->dwPos = R_A;
				vOut.AddCode(pPopCode);

				cType = ESIGN_REGISTER;
				dwPos = pRightOperand->cRegisterIndex;
			}
			else
			{
				CLoadVarICode* pVarCode = (CLoadVarICode*)pRightOperand;
				cType = pVarCode->AnalysisVar(vOut, dwPos);
			}
		}

		switch (nOperatorCode)
		{
		case ECODE_ADD:
		{
			CAddExeCode* pCode = CExeCodeMgr::GetInstance()->New<CAddExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_SUM:
		{
			CSumExeCode* pCode = CExeCodeMgr::GetInstance()->New<CSumExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_MUL:
		{
			CMulExeCode* pCode = CExeCodeMgr::GetInstance()->New<CMulExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_DIV:
		{
			CDivExeCode* pCode = CExeCodeMgr::GetInstance()->New<CDivExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_MOD:
		{
			CModExeCode* pCode = CExeCodeMgr::GetInstance()->New<CModExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_CMP_EQUAL:
		{
			CCmpEqualExeCode* pCode = CExeCodeMgr::GetInstance()->New<CCmpEqualExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_CMP_NOTEQUAL:
		{
			CCmpNotEqualExeCode* pCode = CExeCodeMgr::GetInstance()->New<CCmpNotEqualExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_CMP_BIG:
		{
			CCmpBigExeCode* pCode = CExeCodeMgr::GetInstance()->New<CCmpBigExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_CMP_BIGANDEQUAL:
		{
			CCmpBigAndEqualExeCode* pCode = CExeCodeMgr::GetInstance()->New<CCmpBigAndEqualExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_CMP_LESS:
		{
			CCmpLessExeCode* pCode = CExeCodeMgr::GetInstance()->New<CCmpLessExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_CMP_LESSANDEQUAL:
		{
			CCmpLessAndEqualExeCode* pCode = CExeCodeMgr::GetInstance()->New<CCmpLessAndEqualExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_BIT_AND:
		{
			CBitAndExeCode* pCode = CExeCodeMgr::GetInstance()->New<CBitAndExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_BIT_OR:
		{
			CBitOrExeCode* pCode = CExeCodeMgr::GetInstance()->New<CBitOrExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		case ECODE_BIT_XOR:
		{
			CBitXorExeCode* pCode = CExeCodeMgr::GetInstance()->New<CBitXorExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = cType;
			pCode->dwPos = dwPos;
			vOut.AddCode(pCode);
		}
		break;
		default:
			return false;
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
	bool COperatorICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		if (m_pLoader == nullptr)
		{
			return false;
		}

		GetNewWord(nextWord);
		int nPRI = m_pLoader->GetSignPRI(nextWord.word);
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
		return true;
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
	bool CCallFunICode::MakeExeCode(tagCodeData& vOut)
	{
		for (unsigned int i = 0; i < vParams.size(); i++)
		{
			CBaseICode* pCode = vParams[i];
			if (pCode)
			{
				pCode->SetRegisterIndex(R_A);
				pCode->MakeExeCode(vOut);
				CPushExeCode* pPushCode = CExeCodeMgr::GetInstance()->New<CPushExeCode>(m_unBeginSoureIndex);
				pPushCode->cType = ESIGN_REGISTER;
				pPushCode->dwPos = R_A;
				vOut.AddCode(pPushCode);
			}
		}
		int nCallBack = CScriptCallBackFunion::GetFunIndex(strFunName);
		if (nCallBack >= 0)
		{
			CCallBackExeCode* pCallCode = CExeCodeMgr::GetInstance()->New<CCallBackExeCode>(m_unBeginSoureIndex);
			pCallCode->cParmSize = (unsigned char)vParams.size();
			pCallCode->cResultRegister = cRegisterIndex;
			pCallCode->unFunIndex = nCallBack;
			vOut.AddCode(pCallCode);

			return true;
		}
		int nFunIndex = CScriptCodeLoader::GetInstance()->GetCodeIndex(strFunName.c_str());
		if (nFunIndex >= 0)
		{
			CCallScriptExeCode* pCallCode = CExeCodeMgr::GetInstance()->New<CCallScriptExeCode>(m_unBeginSoureIndex);
			pCallCode->cParmSize = (unsigned char)vParams.size();
			pCallCode->cResultRegister = cRegisterIndex;
			pCallCode->unFunIndex = nFunIndex;
			vOut.AddCode(pCallCode);

			return true;
		}
		return false;
	}
	void CCallFunICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (nType == E_PARAM)
		{
			vParams.push_back(pCode);
		}
		CBaseICode::AddICode(nType, pCode);
	}
	bool CCallFunICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		if (m_pLoader == nullptr)
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
				if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_EXPRESSION, this, E_PARAM))
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
	bool CCallClassFunICode::MakeExeCode(tagCodeData& vOut)
	{
		VarInfo* pVarInfo = GetTempVarInfo(strClassVarName.c_str());
		if (pVarInfo == nullptr)
		{
			//检查是否是全局变量
			pVarInfo = CScriptCodeLoader::GetInstance()->GetGlobalVarInfo(strClassVarName);
		}
		if (pVarInfo == nullptr)
		{
			return false;
		}
		if (pVarInfo->cType != EScriptVal_ClassPoint)
		{
			return false;
		}
		int nFunIndex = CScriptSuperPointerMgr::GetInstance()->GetClassFunIndex(pVarInfo->wExtend, strFunName);
		if (nFunIndex < 0)
		{
			return false;
		}
		for (unsigned int i = 0; i < vParams.size(); i++)
		{
			CBaseICode* pCode = vParams[i];
			if (pCode)
			{
				pCode->SetRegisterIndex(R_A);

				pCode->MakeExeCode(vOut);
				CPushExeCode* pPushCode = CExeCodeMgr::GetInstance()->New<CPushExeCode>(m_unBeginSoureIndex);
				pPushCode->cType = ESIGN_REGISTER;
				pPushCode->dwPos = R_A;
				vOut.AddCode(pPushCode);
			}
		}
		CLoadExeCode* pLoadCode = CExeCodeMgr::GetInstance()->New<CLoadExeCode>(m_unBeginSoureIndex);
		pLoadCode->cType = pVarInfo->cGlobal == 1 ? ESIGN_POS_GLOBAL_VAR : ESIGN_POS_LOACL_VAR;
		pLoadCode->cResultRegister = R_C;
		pLoadCode->dwPos = pVarInfo->dwPos;
		vOut.AddCode(pLoadCode);

		CCallClassFunExeCode* pCallCode = CExeCodeMgr::GetInstance()->New<CCallClassFunExeCode>(m_unBeginSoureIndex);
		pCallCode->cParmSize = (unsigned char)vParams.size();
		pCallCode->cResultRegister = cRegisterIndex;
		pCallCode->cClassRegIndex = R_C;
		pCallCode->dwPos = nFunIndex;
		vOut.AddCode(pCallCode);

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

	bool CCallClassFunICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		if (m_pLoader == nullptr)
		{
			return false;
		}

		GetNewWord(nextWord);
		strClassVarName = nextWord.word;

		GetWord(nextWord);
		if (nextWord.word != "->")
		{
			AddErrorInfo(nextWord.nSourceWordsIndex, "CCallClassFunICode:format error ->");
			RevertAll();
			return false;
		}

		GetWord(nextWord);
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
				if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_EXPRESSION, this, E_PARAM))
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

	bool CSentenceICode::MakeExeCode(tagCodeData& vOut)
	{
		for (auto it = vData.begin(); it != vData.end(); it++)
		{
			CBaseICode* pCode = *it;
			if (pCode)
			{
				if (!pCode->MakeExeCode(vOut))
				{
					return false;
				}
			}
		}
		return true;
	}

	void CSentenceICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (pCode)
		{
			vData.push_back(pCode);
		}
	}

	bool CSentenceICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		if (m_pLoader == nullptr)
		{
			return false;
		}
		GetNewWord(nextWord);
		if (nextWord.word != ";")
		{
			RevertOne();
			while (true)
			{
				if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_EXPRESSION, this, 0))
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

	bool CExpressionICode::MakeExeCode(tagCodeData& vOut)
	{
		if (m_pRoot)
		{
			return m_pRoot->MakeExeCode(vOut);
		}
		else if(pOperandCode)
		{
			return pOperandCode->MakeExeCode(vOut);
		}
		return true;
	}
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
	bool CExpressionICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		if (m_pLoader == nullptr)
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
				if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_OPERATOR, this, 0))
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
				if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_MEMBER, this, 0))
				{
					RevertAll();
					return false;
				}
				nNeedOperand--;
			}
			nState++;
			nState = nState % E_STATE_SIZE;
		}
		CheckOperatorTree((CBaseICode**)&m_pRoot);
		return true;
	}
	bool CExpressionICode::CheckOperatorTree(CBaseICode** pNode)
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
				CSetClassParamICode* pSetCode = (CSetClassParamICode*)mgr.New(m_pLoader, pLeftOperand->m_unBeginSoureIndex);
				pSetCode->strClassVarName = pLeftOperand->strClassVarName;
				pSetCode->strParamName = pLeftOperand->strParamName;

				if (CheckOperatorTree(&pOperNode->pRightOperand) == false)
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
				CSaveVarICode* pSaveCode = (CSaveVarICode*)mgr.New(m_pLoader, pLeftOperand->m_unBeginSoureIndex);

				pSaveCode->m_word = pLeftOperand->m_word;
				if (CheckOperatorTree(&pOperNode->pRightOperand) == false)
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
			if (CheckOperatorTree(&pOperNode->pLeftOperand) == false)
			{
				return false;
			}
		}
		if (pOperNode->pRightOperand && pOperNode->pRightOperand->GetType() == E_I_CODE_OPERATOR)
		{
			if (CheckOperatorTree(&pOperNode->pRightOperand) == false)
			{
				return false;
			}
		}
		return true;
	}
	bool CIfICode::MakeExeCode(tagCodeData& vOut)
	{
		//先压入条件
		if (pCondCode)
		{
			pCondCode->SetRegisterIndex(cRegisterIndex);
			pCondCode->MakeExeCode(vOut);
		}
		CJumpFalseExeCode* pIfCode = CExeCodeMgr::GetInstance()->New<CJumpFalseExeCode>(m_unBeginSoureIndex);
		pIfCode->cVarRegister = cRegisterIndex;
		vOut.AddCode(pIfCode);
		if (pTureCode)
			pTureCode->MakeExeCode(vOut);

		if (pFalseCode)
		{
			CJumpExeCode* pElseCode = CExeCodeMgr::GetInstance()->New<CJumpExeCode>(m_unBeginSoureIndex);
			vOut.AddCode(pElseCode);
			pFalseCode->MakeExeCode(vOut);
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

	bool CIfICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		if (m_pLoader == nullptr)
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

		if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_EXPRESSION, this, E_COND))
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

		if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_STATEMENT, this, E_TRUE))
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

		if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_STATEMENT, this, E_FALSE))
		{
			AddErrorInfo(nextWord.nSourceWordsIndex, "CIfICode:if false block format error");
			RevertAll();
			return false;
		}

		return true;
	}

	bool CWhileICode::MakeExeCode(tagCodeData& vOut)
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
		pCondCode->MakeExeCode(vOut);

		CJumpFalseExeCode* pIfCode = CExeCodeMgr::GetInstance()->New<CJumpFalseExeCode>(m_unBeginSoureIndex);
		pIfCode->cVarRegister = cRegisterIndex;
		vOut.AddCode(pIfCode);

		pBodyCode->MakeExeCode(vOut);
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
	bool CWhileICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();

		if (m_pLoader == nullptr)
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

		if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_EXPRESSION, this, E_COND))
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

		if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_STATEMENT, this, E_BLOCK))
		{
			AddErrorInfo(nextWord.nSourceWordsIndex, "CWhileICode:while block format error");
			RevertAll();
			return false;
		}

		return true;
	}
	bool CContinueICode::MakeExeCode(tagCodeData& vOut)
	{
		CContinueExeCode* pCode = CExeCodeMgr::GetInstance()->New<CContinueExeCode>(m_unBeginSoureIndex);
		vOut.AddCode(pCode);

		return true;
	}
	bool CContinueICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();
		if (m_pLoader == nullptr)
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
			m_pLoader->AddErrorInfo(nextWord.nSourceWordsIndex, "CContinueICode:format error");
			RevertAll();
			return false;
		}
		return true;
	}
	bool CBreakICode::MakeExeCode(tagCodeData& vOut)
	{
		CBreakExeCode* pCode = CExeCodeMgr::GetInstance()->New<CBreakExeCode>(m_unBeginSoureIndex);
		vOut.AddCode(pCode);
		return true;
	}
	bool CBreakICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();
		if (m_pLoader == nullptr)
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
			m_pLoader->AddErrorInfo(nextWord.nSourceWordsIndex, "CBreakICode:format error");
			RevertAll();
			return false;
		}
		return true;
	}
	bool CReturnICode::MakeExeCode(tagCodeData& vOut)
	{
		if (pBodyCode)
		{
			pBodyCode->SetRegisterIndex(cRegisterIndex);
			pBodyCode->MakeExeCode(vOut);
		}

		CReturnExeCode* pCode = CExeCodeMgr::GetInstance()->New<CReturnExeCode>(m_unBeginSoureIndex);
		pCode->cVarRegister = cRegisterIndex;
		vOut.AddCode(pCode);

		return true;
	}

	void CReturnICode::AddICode(int nType, CBaseICode* pCode)
	{
		pBodyCode = pCode;
		CBaseICode::AddICode(nType, pCode);
	}

	bool CReturnICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();
		if (m_pLoader == nullptr)
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
		if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_EXPRESSION, this, 0))
		{
			AddErrorInfo(nextWord.nSourceWordsIndex, "CReturnICode:expression format error");
			RevertAll();
			return false;
		}

		GetWord(nextWord);
		if (nextWord.word != ";")
		{
			m_pLoader->AddErrorInfo(nextWord.nSourceWordsIndex, "CReturnICode:format error");
			RevertAll();
			return false;
		}
		return true;
	}


	bool CNewICode::MakeExeCode(tagCodeData& vOut)
	{
		unsigned short nClassType = 0;
		char cType = CScriptCodeLoader::GetInstance()->GetVarType(strClassType, nClassType);
		if (cType != EScriptVal_ClassPoint)
		{
			return false;
		}
		CNewClassExeCode* pCode = CExeCodeMgr::GetInstance()->New<CNewClassExeCode>(m_unBeginSoureIndex);
		pCode->cResultRegister = cRegisterIndex;
		pCode->dwClassIndex = nClassType;
		vOut.AddCode(pCode);

		return true;
	}

	void CNewICode::AddICode(int nType, CBaseICode* pCode)
	{
		CBaseICode::AddICode(nType, pCode);
	}

	bool CNewICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();
		if (m_pLoader == nullptr)
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
		return true;
	}

	bool CDeleteICode::MakeExeCode(tagCodeData& vOut)
	{
		VarInfo* pVarInfo = GetTempVarInfo(m_VarName.c_str());
		if (pVarInfo == nullptr)
		{
			//检查是否是全局变量
			pVarInfo = CScriptCodeLoader::GetInstance()->GetGlobalVarInfo(m_VarName);
		}
		if (pVarInfo == nullptr)
		{
			return false;
		}
		if (pVarInfo->cType != EScriptVal_ClassPoint)
		{
			return false;
		}
		CLoadExeCode* pLoadCode = CExeCodeMgr::GetInstance()->New<CLoadExeCode>(m_unBeginSoureIndex);
		pLoadCode->cType = pVarInfo->cGlobal == 1 ? ESIGN_POS_GLOBAL_VAR : ESIGN_POS_LOACL_VAR;
		pLoadCode->cResultRegister = R_C;
		pLoadCode->dwPos = pVarInfo->dwPos;
		vOut.AddCode(pLoadCode);

		CReleaseClassExeCode* pCode = CExeCodeMgr::GetInstance()->New<CReleaseClassExeCode>(m_unBeginSoureIndex);
		pCode->cVarRegister = R_C;
		vOut.AddCode(pCode);
		return true;
	}

	void CDeleteICode::AddICode(int nType, CBaseICode* pCode)
	{
		CBaseICode::AddICode(nType, pCode);
	}

	bool CDeleteICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();
		if (m_pLoader == nullptr)
		{
			return false;
		}
		GetNewWord(nextWord);
		if (nextWord.word != "delete")
		{
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		m_VarName = nextWord.word;
		GetWord(nextWord);
		if (nextWord.word != ";")
		{
			m_pLoader->AddErrorInfo(nextWord.nSourceWordsIndex, "CDeleteICode:format error");
			RevertAll();
			return false;
		}
		return true;
	}
	bool CBracketsICode::MakeExeCode(tagCodeData& vOut)
	{
		if (m_pBody)
		{
			if (!m_pBody->MakeExeCode(vOut))
			{
				return false;
			}
			CMoveExeCode* pCode = CExeCodeMgr::GetInstance()->New<CMoveExeCode>(m_unBeginSoureIndex);
			pCode->cVarRegister = R_A;
			pCode->cType = ESIGN_REGISTER;
			pCode->dwPos = cRegisterIndex;
			vOut.AddCode(pCode);
		}
		return true;
	}

	void CBracketsICode::AddICode(int nType, CBaseICode* pCode)
	{
		m_pBody = pCode;
	}

	bool CBracketsICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();
		if (m_pLoader == nullptr)
		{
			return false;
		}
		GetNewWord(nextWord);
		if (nextWord.word != "(")
		{
			RevertAll();
			return false;
		}
		if (!m_pLoader->RunCompileState(vIn, CScriptCodeLoader::E_CODE_SCOPE_EXPRESSION, this, 0))
		{
			m_pLoader->AddErrorInfo(nextWord.nSourceWordsIndex, "CBracketsICode:expression error");
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		if (nextWord.word != ")")
		{
			m_pLoader->AddErrorInfo(nextWord.nSourceWordsIndex, "CBracketsICode:format error");
			RevertAll();
			return false;
		}
		return true;
	}
	bool CTestSignICode::MakeExeCode(tagCodeData& vOut)
	{
		for (int i = 0; i < nNum/2; i++)
		{
			//CSignExeCode *pCode = CExeCodeMgr::GetInstance()->New<CSignExeCode>(m_unBeginSoureIndex);
			//vOut.AddCode(pCode);

			//CAddExeCode* pCode = CExeCodeMgr::GetInstance()->New<CAddExeCode>(m_unBeginSoureIndex);
			//pCode->cResultRegister = cRegisterIndex;
			//pCode->cType = 0;
			//pCode->dwPos = 1;
			//vOut.AddCode(pCode);

			CCmpLessExeCode* pCode = CExeCodeMgr::GetInstance()->New<CCmpLessExeCode>(m_unBeginSoureIndex);
			pCode->cResultRegister = cRegisterIndex;
			pCode->cType = 0;
			pCode->dwPos = 1;
			vOut.AddCode(pCode);

			//CPushExeCode* pCode = CExeCodeMgr::GetInstance()->New<CPushExeCode>(m_unBeginSoureIndex);
			////pCode->cVarRegister = cRegisterIndex;
			//pCode->cType = 2;
			//pCode->dwPos = 1;
			//vOut.AddCode(pCode);
			//CPopExeCode* pCode2 = CExeCodeMgr::GetInstance()->New<CPopExeCode>(m_unBeginSoureIndex);
			////pCode->cVarRegister = cRegisterIndex;
			//pCode2->cType = 6;
			//pCode2->dwPos = 0;
			//vOut.AddCode(pCode2);
		}
		return true;
	}
	void CTestSignICode::AddICode(int nType, CBaseICode* pCode)
	{
		CBaseICode::AddICode(nType, pCode);
	}
	bool CTestSignICode::Compile(SentenceSourceCode& vIn)
	{
		SignToPos();
		if (m_pLoader == nullptr)
		{
			return false;
		}
		GetNewWord(nextWord);
		if (nextWord.word != "testsign")
		{
			RevertAll();
			return false;
		}
		GetWord(nextWord);
		nNum = atoi(nextWord.word.c_str());
		GetWord(nextWord);
		if (nextWord.word != ";")
		{
			m_pLoader->AddErrorInfo(nextWord.nSourceWordsIndex, "CTestSignICode:format error");
			RevertAll();
			return false;
		}
		return true;
	}
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


	std::list<CBaseICode*> CBaseICodeMgr::m_listICode;

	bool CBaseICodeMgr::Release(CBaseICode* pCode)
	{
		if (pCode)
		{
			for (auto it = m_listICode.begin(); it != m_listICode.end(); it++)
			{
				if (*it == pCode)
				{
					m_listICode.erase(it);
					break;
				}
			}
			delete pCode;
			return true;
		}
		return false;
	}

	void CBaseICodeMgr::Clear()
	{
		for (auto it = m_listICode.begin(); it != m_listICode.end(); it++)
		{
			CBaseICode* pCode = *it;
			if (pCode)
			{
				delete pCode;
			}
		}
		m_listICode.clear();
	}

}