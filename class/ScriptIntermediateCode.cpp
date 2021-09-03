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

	bool CDefGlobalVarICode::Compile(SentenceSourceCode& vIn)
	{

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

	bool CFunICode::MakeExeCode(stCodeData& vOut)
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
		if (pBodyCode)
		{
			pBodyCode->MakeExeCode(vOut);
		}

		return true;
	}

	void CFunICode::AddICode(int nType, CBaseICode* pCode)
	{
		pBodyCode = dynamic_cast<CBlockICode*>(pCode);
		CBaseICode::AddICode(nType, pBodyCode);
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

	bool CBlockICode::MakeExeCode(stCodeData& vOut)
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

	void CBlockICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (pCode)
			m_vICode.push_back(pCode);
		CBaseICode::AddICode(nType, pCode);
	}

	char CLoadVarICode::AnalysisVar(stCodeData& vOut, unsigned int& pos)
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
		return cType;
	}

	bool CLoadVarICode::MakeExeCode(stCodeData& vOut)
	{
		CLoadExeCode* pCode = CExeCodeMgr::GetInstance()->New<CLoadExeCode>(m_unBeginSoureIndex);
		pCode->cResultRegister = cRegisterIndex;
		pCode->cType = AnalysisVar(vOut, pCode->dwPos);

		vOut.AddCode(pCode);
		return true;
	}
	bool CSaveVarICode::MakeExeCode(stCodeData& vOut)
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
		pCode->cResultRegister = cRegisterIndex;
		pCode->cType = cType;
		pCode->dwPos = pos;
		vOut.AddCode(pCode);
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
	bool CGetClassParamICode::MakeExeCode(stCodeData& vOut)
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
		int nParamIndex = CScriptSuperPointerMgr::GetInstance()->GetClassParamIndex(pVarInfo->wExtend, strParamName);
		if (nParamIndex < 0)
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
		pGetCode->dwPos = nParamIndex;
		vOut.AddCode(pGetCode);
		return true;
	}
	bool CSetClassParamICode::MakeExeCode(stCodeData& vOut)
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
		int nParamIndex = CScriptSuperPointerMgr::GetInstance()->GetClassParamIndex(pVarInfo->wExtend, strParamName);
		if (nParamIndex < 0)
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
		pGetCode->dwPos = nParamIndex;
		vOut.AddCode(pGetCode);

		return true;
	}
	void CSetClassParamICode::AddICode(int nType, CBaseICode* pCode)
	{
		pRightOperand = pCode;
		CBaseICode::AddICode(0, pCode);
	}
	bool CMinusICode::MakeExeCode(stCodeData& vOut)
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
	bool COperatorICode::MakeExeCode(stCodeData& vOut)
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
			if (pRightOperand->GetType() != E_I_CODE_OPERAND)
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
			pOperCode->nOperatorFlag = nType;
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
	bool CCallFunICode::MakeExeCode(stCodeData& vOut)
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
	bool CCallClassFunICode::MakeExeCode(stCodeData& vOut)
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


	bool CIfICode::MakeExeCode(stCodeData& vOut)
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

	bool CWhileICode::MakeExeCode(stCodeData& vOut)
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
		pJumpBeginCode->pJumpCode = pBegin->m_pNext;
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
	bool CContinueICode::MakeExeCode(stCodeData& vOut)
	{
		CContinueExeCode* pCode = CExeCodeMgr::GetInstance()->New<CContinueExeCode>(m_unBeginSoureIndex);
		vOut.AddCode(pCode);

		return true;
	}
	bool CBreakICode::MakeExeCode(stCodeData& vOut)
	{
		CBreakExeCode* pCode = CExeCodeMgr::GetInstance()->New<CBreakExeCode>(m_unBeginSoureIndex);
		vOut.AddCode(pCode);
		return true;
	}
	bool CReturnICode::MakeExeCode(stCodeData& vOut)
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


	bool CNewICode::MakeExeCode(stCodeData& vOut)
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

	bool CDeleteICode::MakeExeCode(stCodeData& vOut)
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