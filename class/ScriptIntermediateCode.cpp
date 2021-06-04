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

	VarInfo CBaseICode::GetTempVarInfo(const char* pVarName)
	{
		if (GetFather())
		{
			return GetFather()->GetTempVarInfo(pVarName);
		}
		return VarInfo();
	}

	void CBaseICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (pCode)
		{
			pCode->SetFather(this);
		}
	}


	void CFunICode::SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex)
	{
		if (pVarName == nullptr)
		{
			return;
		}
		if (m_mapTempVarIndex.find(pVarName) == m_mapTempVarIndex.end())
		{
			auto& var = m_mapTempVarIndex[pVarName];
			var.cGlobal = 0;//局部变量
			var.cType = nType;
			var.dwPos = nIndex;
			var.wExtend = ClassIndex;
			m_vecTempVarOrder.push_back(pVarName);
		}

	}
	unsigned int CFunICode::GetTempVarIndex(const char* pVarName)
	{
		auto it = m_mapTempVarIndex.find(pVarName);
		if (it != m_mapTempVarIndex.end())
		{
			return it->second.dwPos;
		}
		return g_nTempVarIndexError;
	}

	VarInfo CFunICode::GetTempVarInfo(const char* pVarName)
	{
		auto it = m_mapTempVarIndex.find(pVarName);
		if (it != m_mapTempVarIndex.end())
		{
			return it->second;
		}
		return VarInfo();
	}

	void CFunICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		//首先是参数的赋值
		for (int i = 0; i < m_vecTempVarOrder.size(); i++)
		{

			CodeStyle code(m_unBeginSoureIndex);
			code.qwCode = 0;
			code.cSign = ESIGN_POS_LOACL_VAR;
			code.wInstruct = ECODE_POP;
			code.cExtend = 0;
			code.dwPos = i;
			vOut.push_back(code);
		}
		if (pBodyCode)
		{
			pBodyCode->MakeExeCode(vOut);
		}
	}

	void CFunICode::AddICode(int nType, CBaseICode* pCode)
	{
		pBodyCode = dynamic_cast<CBlockICode*>(pCode);
		CBaseICode::AddICode(nType,pBodyCode);
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

	void CBlockICode::SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex)
	{
		if (pVarName == nullptr)
		{
			return;
		}
		if (m_mapTempVarIndex.find(pVarName) == m_mapTempVarIndex.end())
		{
			auto &var = m_mapTempVarIndex[pVarName];
			var.cGlobal = 0;//局部变量
			var.cType = nType;
			var.dwPos = nIndex;
			var.wExtend = ClassIndex;
		}
	}

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

	VarInfo CBlockICode::GetTempVarInfo(const char* pVarName)
	{
		auto it = m_mapTempVarIndex.find(pVarName);
		if (it != m_mapTempVarIndex.end())
		{
			return it->second;
		}
		if (GetFather())
		{
			return GetFather()->GetTempVarInfo(pVarName);
		}
		return VarInfo();
	}

	void CBlockICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		for (auto it = m_vICode.begin(); it != m_vICode.end(); it++)
		{
			if (*it)
			{
				(*it)->MakeExeCode(vOut);
			}
		}
	}

	void CBlockICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (pCode)
			m_vICode.push_back(pCode);
		CBaseICode::AddICode(nType, pCode);
	}

	void CLoadVarICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_LOAD;
		code.cSign = cSource;
		code.cExtend = cRegisterIndex;
		code.dwPos = nPos;
		vOut.push_back(code);
	}
	void CSaveVarICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_MOVE;
		code.cSign = cDestination;
		code.cExtend = cRegisterIndex;
		code.dwPos = nPos;
		vOut.push_back(code);
	}
	void CPush2StackICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_PUSH;
		code.cSign = cSource;
		code.cExtend = 0;
		code.dwPos = nPos;
		vOut.push_back(code);
	}
	void CPop4StackICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_POP;
		code.cSign = cDestination;
		code.cExtend = cRegisterIndex;
		code.dwPos = nPos;
		vOut.push_back(code);
	}
	void CGetClassParamICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_GET_CLASS_PARAM;
		code.cSign = cClassRIndex;
		code.cExtend = cDestRIndex;
		code.dwPos = nParamPos;
		vOut.push_back(code);
	}
	void CSetClassParamICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_SET_CLASS_PARAM;
		code.cSign = cClassRIndex;
		code.cExtend = cVarRIndex;
		code.dwPos = nParamPos;
		vOut.push_back(code);
	}
	void COperatorICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		for (auto it = m_vICode.begin(); it != m_vICode.end(); it++)
		{
			auto pCode = *it;
			if (pCode)
			{
				pCode->MakeExeCode(vOut);
			}
		}
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = nOperatorCode;
		code.cExtend = cRegisterIndex;
		vOut.push_back(code);
	}
	void COperatorICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (pCode)
		{
			m_vICode.push_back(pCode);
		}
		CBaseICode::AddICode(nType, pCode);
	}
	void CCallBackFunICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		for (unsigned int i = 0; i < vParams.size(); i++)
		{
			CBaseICode* pCode = vParams[i];
			if (pCode)
			{
				pCode->MakeExeCode(vOut);
			}
		}
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_CALL_CALLBACK;
		code.cSign = cRegisterIndex;
		code.cExtend = (char)vParams.size();
		code.dwPos = nFunIndex;
		vOut.push_back(code);
	}
	void CCallBackFunICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (nType == E_PARAM)
		{
			vParams.push_back(pCode);
		}
		CBaseICode::AddICode(nType, pCode);
	}
	void CCallScriptFunICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		for (unsigned int i = 0; i < vParams.size(); i++)
		{
			CBaseICode* pCode = vParams[i];
			if (pCode)
			{
				pCode->MakeExeCode(vOut);
			}
		}
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_CALL_SCRIPT;
		code.cSign = cRegisterIndex;
		code.cExtend = (char)vParams.size();
		code.dwPos = nFunIndex;
		vOut.push_back(code);
	}
	void CCallScriptFunICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (nType == E_PARAM)
		{
			vParams.push_back(pCode);
		}
		CBaseICode::AddICode(nType, pCode);
	}
	void CCallClassFunICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		for (unsigned int i = 0; i < vParams.size(); i++)
		{
			CBaseICode* pCode = vParams[i];
			if (pCode)
			{
				pCode->MakeExeCode(vOut);
			}
		}
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_CALL_CLASS_FUN;
		code.cSign = cRegisterIndex;
		code.cExtend = (char)vParams.size();
		code.dwPos = nFunIndex;
		vOut.push_back(code);
	}

	void CCallClassFunICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (nType == E_POINT)
		{
			m_pClassPointCode = pCode;
		}
		else if (nType == E_PARAM)
		{
			vParams.push_back(pCode);
		}
		CBaseICode::AddICode(nType, pCode);
	}

	void CSentenceICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		for (auto it = vData.begin(); it != vData.end(); it++)
		{
			auto pCode = *it;
			if (pCode)
			{
				pCode->MakeExeCode(vOut);
			}
		}

		if (bClearParm)
		{
			//每句结束后都清除堆栈
			CodeStyle clearcode(m_unBeginSoureIndex);
			clearcode.qwCode = 0;
			clearcode.wInstruct = ECODE_CLEAR_PARAM;
			clearcode.cSign = 0;
			vOut.push_back(clearcode);
		}

	}

	void CSentenceICode::AddExeCode(CBaseICode* code)
	{
		vData.push_back(code);
	}



	void CIfICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		//先压入条件
		if (pCondCode)
			pCondCode->MakeExeCode(vOut);

		std::vector<CodeStyle> vTrueData;
		if (pTureCode)
			pTureCode->MakeExeCode(vTrueData);
		vOut.push_back(CodeStyle(m_unBeginSoureIndex));
		CodeStyle &ifcode = vOut[vOut.size()-1];
		ifcode.qwCode = 0;
		ifcode.wInstruct = ECODE_JUMP_FALSE;
		ifcode.cSign = 1;
		ifcode.dwPos = vTrueData.size() + 1;

		for (auto it = vTrueData.begin(); it != vTrueData.end(); it++)
		{
			vOut.push_back(*it);
		}

		if (pFalseCode)
		{
			std::vector<CodeStyle> vFalseData;
			pFalseCode->MakeExeCode(vFalseData);

			ifcode.dwPos++;
			vOut.push_back(CodeStyle(m_unElseSoureIndex));
			CodeStyle &elsecode = vOut[vOut.size()-1];
			elsecode.qwCode = 0;
			elsecode.wInstruct = ECODE_JUMP;
			elsecode.cSign = 1;
			elsecode.dwPos = vFalseData.size() + 1;

			for (auto it = vFalseData.begin(); it != vFalseData.end(); it++)
			{
				vOut.push_back(*it);
			}
		}
	}

	void CIfICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (nType == E_COND)
		{
			pCondCode = dynamic_cast<CSentenceICode*>(pCode);
		}
		else if (nType == E_TRUE)
		{
			pTureCode = dynamic_cast<CBlockICode*>(pCode);
		}
		else if (nType == E_FALSE)
		{
			pFalseCode = pCode;
		}
		CBaseICode::AddICode(nType, pCode);
	}

	void CWhileICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		std::vector<CodeStyle> vCondData;
		if (pCondCode)
			pCondCode->MakeExeCode(vCondData);

		std::vector<CodeStyle> vBlockData;
		if (pBodyCode)
			pBodyCode->MakeExeCode(vBlockData);

		//要在块尾加入返回块头的指令
		CodeStyle backcode(m_unBeginSoureIndex);
		backcode.qwCode = 0;
		backcode.wInstruct = ECODE_JUMP;
		backcode.cSign = 2;
		backcode.dwPos = vCondData.size() + vBlockData.size() + 1;
		vBlockData.push_back(backcode);

		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_JUMP_FALSE;
		code.cSign = 1;
		code.cExtend = R_A;
		code.dwPos = vBlockData.size()+1;
		vCondData.push_back(code);

		for (auto it = vCondData.begin(); it != vCondData.end(); it++)
		{
			vOut.push_back(*it);
		}

		for (unsigned int i = 0; i < vBlockData.size(); i++)
		{
			CodeStyle& code = vBlockData[i];
			if (code.wInstruct == ECODE_BREAK)
			{
				code.wInstruct = ECODE_JUMP;
				code.cSign = 1;
				code.dwPos = vBlockData.size() - i + 1;
			}
			else if (code.wInstruct == ECODE_CONTINUE)
			{
				code.wInstruct = ECODE_JUMP;
				code.cSign = 2;
				code.dwPos = i + vCondData.size();
			}
			vOut.push_back(code);
		}
	}

	void CWhileICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (nType == 0)
		{
			pCondCode = dynamic_cast<CSentenceICode*>(pCode);
		}
		else if (nType == 1)
		{
			pBodyCode = dynamic_cast<CBlockICode*>(pCode);
		}
		CBaseICode::AddICode(nType, pCode);
	}
	void CContinueICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_CONTINUE;
		vOut.push_back(code);
	}
	void CBreakICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		CodeStyle code(m_unBeginSoureIndex);
		code.qwCode = 0;
		code.wInstruct = ECODE_BREAK;
		vOut.push_back(code);
	}
	void CReturnICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		if (pBodyCode)
		{
			pBodyCode->MakeExeCode(vOut);
		}

		CodeStyle backcode(m_unBeginSoureIndex);
		backcode.qwCode = 0;
		backcode.wInstruct = ECODE_RETURN;
		backcode.cExtend = cRegisterIndex;
		
		vOut.push_back(backcode);
	}

	void CReturnICode::AddICode(int nType, CBaseICode* pCode)
	{
		pBodyCode = pCode;
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
	void CICodeMgr::Release(CBaseICode* pPoint)
	{
	}
	void CICodeMgr::Clear()
	{
	}

}