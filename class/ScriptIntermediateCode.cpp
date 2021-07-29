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

	VarInfo *CBaseICode::GetTempVarInfo(const char* pVarName)
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

	bool CFunICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
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

	bool CBlockICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
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

	char CLoadVarICode::AnalysisVar(CScriptCodeLoader::tagCodeData& vOut, unsigned int& pos)
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

	bool CLoadVarICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	{
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_LOAD;
		code.cExtend = cRegisterIndex;
		code.cSign = AnalysisVar(vOut, code.dwPos);

		vOut.vCodeData.push_back(code);
		return true;
	}
	bool CSaveVarICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
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
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_MOVE;
		code.cSign = cType;
		code.cExtend = cRegisterIndex;
		code.dwPos = pos;
		vOut.vCodeData.push_back(code);
		return true;
	}
	void CSaveVarICode::AddICode(int nType, CBaseICode* pCode)
	{
		pRightOperand = pCode;
		CBaseICode::AddICode(0,pCode);
	}
	//void CPush2StackICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	//{
	//	CodeStyle code(m_unBeginSoureIndex);
	//	code.qwCode = 0;
	//	code.wInstruct = ECODE_PUSH;
	//	code.cSign = cSource;
	//	code.cExtend = 0;
	//	code.dwPos = nPos;
	//	vOut.push_back(code);
	//}
	//void CPop4StackICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	//{
	//	CodeStyle code(m_unBeginSoureIndex);
	//	code.qwCode = 0;
	//	code.wInstruct = ECODE_POP;
	//	code.cSign = cDestination;
	//	code.cExtend = cRegisterIndex;
	//	code.dwPos = nPos;
	//	vOut.push_back(code);
	//}
	bool CGetClassParamICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	{
		VarInfo *pVarInfo = GetTempVarInfo(strClassVarName.c_str());
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
		CodeStyle classcode(m_unBeginSoureIndex);
		classcode.qwCode = 0;
		classcode.wInstruct = ECODE_LOAD;
		classcode.cSign = pVarInfo->cGlobal==1? ESIGN_POS_GLOBAL_VAR: ESIGN_POS_LOACL_VAR;
		classcode.cExtend = R_C;
		classcode.dwPos = pVarInfo->dwPos;
		vOut.vCodeData.push_back(classcode);

		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_GET_CLASS_PARAM;
		code.cSign = R_C;
		code.cExtend = cRegisterIndex;
		code.dwPos = nParamIndex;
		vOut.vCodeData.push_back(code);
		return true;
	}
	bool CSetClassParamICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
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
		CodeStyle classcode(m_unBeginSoureIndex);
		classcode.qwCode = 0;
		classcode.wInstruct = ECODE_LOAD;
		classcode.cSign = pVarInfo->cGlobal == 1 ? ESIGN_POS_GLOBAL_VAR : ESIGN_POS_LOACL_VAR;
		classcode.cExtend = R_C;
		classcode.dwPos = pVarInfo->dwPos;
		vOut.vCodeData.push_back(classcode);

		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_SET_CLASS_PARAM;
		code.cSign = R_C;
		code.cExtend = cRegisterIndex;
		code.dwPos = nParamIndex;
		vOut.vCodeData.push_back(code);
		return true;
	}
	void CSetClassParamICode::AddICode(int nType, CBaseICode* pCode)
	{
		pRightOperand = pCode;
		CBaseICode::AddICode(0, pCode);
	}
	bool CMinusICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	{
		if (pRightOperand)
		{
			pRightOperand->SetRegisterIndex(R_A);
			pRightOperand->MakeExeCode(vOut);
		}

		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_MINUS;
		code.cSign = R_A;
		code.cExtend = cRegisterIndex;
		vOut.vCodeData.push_back(code);
		return true;
	}
	void CMinusICode::AddICode(int nType, CBaseICode* pCode)
	{
		pRightOperand = pCode;
		CBaseICode::AddICode(0, pCode);
	}
	bool COperatorICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	{
		nOperatorCode = CScriptCodeLoader::GetInstance()->GetWordKey(strOperator);
		CodeStyle opercode(m_unBeginSoureIndex);
		opercode.qwCode = 0;
		opercode.wInstruct = nOperatorCode;
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
				CodeStyle pushcode(m_unBeginSoureIndex);
				pushcode.qwCode = 0;
				pushcode.wInstruct = ECODE_PUSH;
				pushcode.cSign = ESIGN_REGISTER;
				pushcode.dwPos = R_A;
				vOut.vCodeData.push_back(pushcode);

				pRightOperand->MakeExeCode(vOut);

				CodeStyle popcode(m_unBeginSoureIndex);
				popcode.qwCode = 0;
				popcode.wInstruct = ECODE_POP;
				popcode.cSign = ESIGN_REGISTER;
				popcode.dwPos = R_A;
				vOut.vCodeData.push_back(popcode);

				opercode.cSign = ESIGN_REGISTER;
				opercode.dwPos = pRightOperand->cRegisterIndex;
			}
			else
			{
				CLoadVarICode* pVarCode = (CLoadVarICode*)pRightOperand;
				opercode.cSign = pVarCode->AnalysisVar(vOut, opercode.dwPos);
			}
		}
		//for (auto it = m_OtherOperand.begin(); it != m_OtherOperand.end(); it++)
		//{
		//	auto pCode = *it;
		//	if (pCode)
		//	{
		//		pCode->MakeExeCode(vOut);
		//	}
		//}

		opercode.cExtend = cRegisterIndex;
		vOut.vCodeData.push_back(opercode);
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
	//bool CCallBackFunICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
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
	bool CCallFunICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	{
		for (unsigned int i = 0; i < vParams.size(); i++)
		{
			CBaseICode* pCode = vParams[i];
			if (pCode)
			{
				pCode->SetRegisterIndex(R_A);
				pCode->MakeExeCode(vOut);
				CodeStyle pushcode(m_unBeginSoureIndex);
				pushcode.qwCode = 0;
				pushcode.wInstruct = ECODE_PUSH;
				pushcode.cSign = ESIGN_REGISTER;
				pushcode.dwPos = R_A;
				vOut.vCodeData.push_back(pushcode);
			}
		}
		int nCallBack = CScriptCallBackFunion::GetFunIndex(strFunName);
		if (nCallBack >= 0)
		{
			CodeStyle code(m_unBeginSoureIndex);
			code.qwCode = 0;
			code.wInstruct = ECODE_CALL_CALLBACK;
			code.cSign = cRegisterIndex;
			code.cExtend = (char)vParams.size();
			code.dwPos = nCallBack;
			vOut.vCodeData.push_back(code);
			return true;
		}
		int nFunIndex = CScriptCodeLoader::GetInstance()->GetCodeIndex(strFunName.c_str());
		if (nFunIndex >= 0)
		{
			CodeStyle code(m_unBeginSoureIndex);
			code.qwCode = 0;
			code.wInstruct = ECODE_CALL_SCRIPT;
			code.cSign = cRegisterIndex;
			code.cExtend = (char)vParams.size();
			code.dwPos = nFunIndex;
			vOut.vCodeData.push_back(code);
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
	bool CCallClassFunICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
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
				if (pCode->MakeExeCode(vOut) == false)
				{
					return false;
				}
				CodeStyle pushcode(m_unBeginSoureIndex);
				pushcode.qwCode = 0;
				pushcode.wInstruct = ECODE_PUSH;
				pushcode.cSign = ESIGN_REGISTER;
				pushcode.dwPos = R_A;
				vOut.vCodeData.push_back(pushcode);
			}
		}
		CodeStyle classcode(m_unBeginSoureIndex);
		classcode.qwCode = 0;
		classcode.wInstruct = ECODE_LOAD;
		classcode.cSign = pVarInfo->cGlobal == 1 ? ESIGN_POS_GLOBAL_VAR : ESIGN_POS_LOACL_VAR;
		classcode.cExtend = cRegisterIndex;
		classcode.dwPos = pVarInfo->dwPos;
		vOut.vCodeData.push_back(classcode);

		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_CALL_CLASS_FUN;
		code.cSign = cRegisterIndex;
		code.cExtend = (char)vParams.size();
		code.dwPos = nFunIndex;
		vOut.vCodeData.push_back(code);
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

	//bool CSentenceICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	//{
	//	for (auto it = vData.begin(); it != vData.end(); it++)
	//	{
	//		auto pCode = *it;
	//		if (pCode)
	//		{
	//			pCode->MakeExeCode(vOut);
	//		}
	//	}

	//	if (bClearParm)
	//	{
	//		//每句结束后都清除堆栈
	//		CodeStyle clearcode(m_unBeginSoureIndex);
	//		clearcode.qwCode = 0;
	//		clearcode.wInstruct = ECODE_CLEAR_PARAM;
	//		clearcode.cSign = 0;
	//		vOut.vCodeData.push_back(clearcode);
	//	}

	//}

	//void CSentenceICode::AddExeCode(CBaseICode* code)
	//{
	//	vData.push_back(code);
	//}



	bool CIfICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	{
		//先压入条件
		if (pCondCode)
		{
			pCondCode->SetRegisterIndex(cRegisterIndex);
			pCondCode->MakeExeCode(vOut);
		}
		vOut.vCodeData.push_back(CodeStyle(m_unBeginSoureIndex));
		unsigned int ifpos = vOut.vCodeData.size() - 1;
		vOut.vCodeData[ifpos].qwCode = 0;
		vOut.vCodeData[ifpos].wInstruct = ECODE_JUMP_FALSE;
		vOut.vCodeData[ifpos].cExtend = cRegisterIndex;
		vOut.vCodeData[ifpos].cSign = 1;
		unsigned int nCurPos = vOut.vCodeData.size();
		if (pTureCode)
			pTureCode->MakeExeCode(vOut);

		vOut.vCodeData[ifpos].dwPos = vOut.vCodeData.size() - nCurPos + 1;

		if (pFalseCode)
		{
			vOut.vCodeData.push_back(CodeStyle(m_unElseSoureIndex));
			unsigned int elsepos = vOut.vCodeData.size() - 1;
			vOut.vCodeData[elsepos].qwCode = 0;
			vOut.vCodeData[elsepos].wInstruct = ECODE_JUMP;
			vOut.vCodeData[elsepos].cSign = 1;
			nCurPos = vOut.vCodeData.size();
			pFalseCode->MakeExeCode(vOut);
			vOut.vCodeData[ifpos].dwPos++;
			vOut.vCodeData[elsepos].dwPos = vOut.vCodeData.size() - nCurPos + 1;
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

	bool CWhileICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	{
		if (pCondCode == nullptr)
		{
			return false;
		}
		if (pBodyCode == nullptr)
		{
			return false;
		}
		unsigned int nBeginPos = vOut.vCodeData.size();

		pCondCode->MakeExeCode(vOut);
		vOut.vCodeData.push_back(CodeStyle(m_unBeginSoureIndex));
		unsigned int ifpos = vOut.vCodeData.size() - 1;
		vOut.vCodeData[ifpos].qwCode = 0;
		vOut.vCodeData[ifpos].wInstruct = ECODE_JUMP_FALSE;
		vOut.vCodeData[ifpos].cExtend = cRegisterIndex;
		vOut.vCodeData[ifpos].cSign = 0;
		unsigned int nCondPos = vOut.vCodeData.size();

		pBodyCode->MakeExeCode(vOut);
		//要在块尾加入返回块头的指令
		vOut.vCodeData.push_back(CodeStyle(pBodyCode->m_unBeginSoureIndex));
		CodeStyle& backcode = vOut.vCodeData[vOut.vCodeData.size() - 1];
		backcode.qwCode = 0;
		backcode.wInstruct = ECODE_JUMP;
		backcode.cSign = 0;
		backcode.dwPos = nBeginPos;
		vOut.vCodeData[ifpos].dwPos = vOut.vCodeData.size();

		for (unsigned int i = nCondPos; i < vOut.vCodeData.size(); i++)
		{
			CodeStyle& code = vOut.vCodeData[i];
			if (code.wInstruct == ECODE_BREAK)
			{
				code.wInstruct = ECODE_JUMP;
				code.cSign = 0;
				code.dwPos = vOut.vCodeData.size();
			}
			else if (code.wInstruct == ECODE_CONTINUE)
			{
				code.wInstruct = ECODE_JUMP;
				code.cSign = 0;
				code.dwPos = nBeginPos;
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
	bool CContinueICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	{
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_CONTINUE;
		vOut.vCodeData.push_back(code);
		return true;
	}
	bool CBreakICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	{
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_BREAK;
		vOut.vCodeData.push_back(code);
		return true;
	}
	bool CReturnICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	{
		if (pBodyCode)
		{
			if (pBodyCode->MakeExeCode(vOut) == false)
			{
				return false;
			}
		}

		CodeStyle backcode(m_unBeginSoureIndex);
		backcode.qwCode = 0;
		backcode.wInstruct = ECODE_RETURN;
		backcode.cExtend = cRegisterIndex;

		vOut.vCodeData.push_back(backcode);
		return true;
	}

	void CReturnICode::AddICode(int nType, CBaseICode* pCode)
	{
		pBodyCode = pCode;
		CBaseICode::AddICode(nType, pCode);
	}


	bool CNewICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
	{
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_NEW_CLASS;
		code.cSign = cRegisterIndex;
		unsigned short nClassType = 0;
		char cType = CScriptCodeLoader::GetInstance()->GetVarType(strClassType, nClassType);
		if (cType != EScriptVal_ClassPoint)
		{
			return false;
		}
		code.dwPos = nClassType;
		vOut.vCodeData.push_back(code);
		return true;
	}

	void CNewICode::AddICode(int nType, CBaseICode* pCode)
	{
		CBaseICode::AddICode(nType, pCode);
	}

	bool CDeleteICode::MakeExeCode(CScriptCodeLoader::tagCodeData& vOut)
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
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_RELEASE_CLASS;
		code.cSign = pVarInfo->cGlobal == 1 ? ESIGN_POS_GLOBAL_VAR : ESIGN_POS_LOACL_VAR;;
		code.dwPos = pVarInfo->dwPos;
		vOut.vCodeData.push_back(code);
		return false;
	}

	void CDeleteICode::AddICode(int nType, CBaseICode* pCode)
	{
		CBaseICode::AddICode(nType, pCode);
	}

	CICodeMgr CICodeMgr::s_Instance;
	CICodeMgr::CICodeMgr()
	{
	}
	CICodeMgr::~CICodeMgr()
	{
		Clear();
	}
	//void CICodeMgr::Release(CBaseICode* pPoint)
	//{
	//}
	void CICodeMgr::Clear()
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