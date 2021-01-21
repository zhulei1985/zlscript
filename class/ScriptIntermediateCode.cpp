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

			CodeStyle code;
			code.qwCode = 0;
			code.cSign = 1;
			code.wInstruct = ECODE_EVALUATE;
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

	void CSentenceICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		for (auto it = vData.begin(); it != vData.end(); it++)
		{
			vOut.push_back(*it);
		}

		if (bClearParm)
		{
			//每句结束后都清除堆栈
			CodeStyle clearcode;
			clearcode.qwCode = 0;
			clearcode.wInstruct = ECODE_CLEAR_PARAM;
			clearcode.cSign = 0;
			vOut.push_back(clearcode);
		}

	}

	void CSentenceICode::AddExeCode(CodeStyle code)
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

		CodeStyle ifcode;
		ifcode.qwCode = 0;
		ifcode.wInstruct = ECODE_BRANCH_IF;
		ifcode.cSign = 0;
		ifcode.dwPos = vTrueData.size() + 1;
		vOut.push_back(ifcode);

		for (auto it = vTrueData.begin(); it != vTrueData.end(); it++)
		{
			vOut.push_back(*it);
		}

		if (pFalseCode)
		{
			std::vector<CodeStyle> vFalseData;
			pFalseCode->MakeExeCode(vFalseData);

			CodeStyle elsecode;
			elsecode.qwCode = 0;
			elsecode.wInstruct = ECODE_BRANCH_ELSE;
			elsecode.cSign = 0;
			elsecode.dwPos = vFalseData.size() + 1;
			vOut.push_back(elsecode);

			for (auto it = vFalseData.begin(); it != vFalseData.end(); it++)
			{
				vOut.push_back(*it);
			}
		}
	}

	void CIfICode::AddICode(int nType, CBaseICode* pCode)
	{
		if (nType == 0)
		{
			pCondCode = dynamic_cast<CSentenceICode*>(pCode);
		}
		else if (nType == 1)
		{
			pTureCode = dynamic_cast<CBlockICode*>(pCode);
		}
		else if (nType == 2)
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
		CodeStyle backcode;
		backcode.qwCode = 0;
		backcode.wInstruct = ECODE_BLOCK_CYC;
		backcode.dwPos = vCondData.size() + vBlockData.size() + 1;
		vBlockData.push_back(backcode);

		CodeStyle code;
		code.qwCode = 0;
		code.wInstruct = ECODE_CYC_IF;
		code.dwPos = vBlockData.size()+1;
		vCondData.push_back(code);

		for (auto it = vCondData.begin(); it != vCondData.end(); it++)
		{
			vOut.push_back(*it);
		}

		for (auto it = vBlockData.begin(); it != vBlockData.end(); it++)
		{
			vOut.push_back(*it);
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

	void CReturnICode::MakeExeCode(std::vector<CodeStyle>& vOut)
	{
		if (pBodyCode)
		{
			pBodyCode->SetClear(false);
			pBodyCode->MakeExeCode(vOut);
		}

		CodeStyle backcode;
		backcode.qwCode = 0;
		backcode.wInstruct = ECODE_RETURN;
		if (pBodyCode)
		{
			backcode.cSign = 1;
		}
		vOut.push_back(backcode);
	}

	void CReturnICode::AddICode(int nType, CBaseICode* pCode)
	{
		pBodyCode = dynamic_cast<CSentenceICode*>(pCode);
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