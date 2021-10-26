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
#include "ScriptClassMgr.h"
#include "ScriptPointInterface.h"
#include "ScriptDataMgr.h"
#include "TempScriptRunState.h"
#include "zByteArray.h"

namespace zlscript
{
	__int64 CScriptPointInterface::s_nScriptPointIndexCount = 0;
	CScriptPointInterface::CScriptPointInterface()
	{
		m_nScriptPointIndex = 0;
		m_pClassInfo = nullptr;
	}
	CScriptPointInterface::~CScriptPointInterface()
	{
		//auto it = m_vecScriptClassFun.begin();
		//for (; it != m_vecScriptClassFun.end(); it++)
		//{
		//	CScriptBaseClassFunInfo* pInfo = *it;
		//	if (pInfo)
		//	{
		//		delete pInfo;
		//	}
		//}
		//m_vecScriptClassFun.clear();

		if (IsInitScriptPointIndex())
		{
			RemoveClassObject(CScriptPointInterface::GetScriptPointIndex());
			ClearScriptPointIndex();
		}
	}
	void CScriptPointInterface::InitScriptPointIndex()
	{
		if (!IsInitScriptPointIndex())
		{
			s_nScriptPointIndexCount++;
			m_nScriptPointIndex = s_nScriptPointIndexCount;
		}

	}
	bool CScriptPointInterface::IsInitScriptPointIndex()
	{
		if (m_nScriptPointIndex > 0)
		{
			return true;
		}
		return false;
	}
	__int64 CScriptPointInterface::GetScriptPointIndex()
	{
		if (!IsInitScriptPointIndex())
		{
			InitScriptPointIndex();
		}
		return m_nScriptPointIndex;
	}
	void CScriptPointInterface::ClearScriptPointIndex()
	{
		m_nScriptPointIndex = 0;
	}

	unsigned int CScriptPointInterface::GetAttributeIndex(std::string name)
	{
		auto it = m_mapAttributeName2Index.find(name);
		if (it != m_mapAttributeName2Index.end())
		{
			return it->second;
		}
		return -1;
	}

	CBaseScriptClassAttribute* CScriptPointInterface::GetAttribute(unsigned int index)
	{
		auto it = m_mapAllAttributes.find(index);
		if (it != m_mapAllAttributes.end())
		{
			return it->second;
		}
		return nullptr;
	}

	int CScriptPointInterface::RunFun(unsigned int id, CScriptCallState* pState)
	{
		if (id >= m_vecScriptClassFun.size())
		{
			return 0;
		}
		int nResult = 0;
		CBaseScriptClassFun* pFun = m_vecScriptClassFun[id];
		if (pFun)
		{
			nResult = pFun->RunFun(pState);
		}
		return nResult;
	}



	void CScriptPointInterface::ChangeScriptAttribute(CBaseScriptClassAttribute* pAttr, StackVarInfo& old)
	{
		if (pAttr == nullptr)
		{
			return;
		}
		if (pAttr->m_flag & CBaseScriptClassAttribute::E_FLAG_DB)
		{

		}
	}

	void CScriptPointInterface::RegisterScriptAttribute(CBaseScriptClassAttribute* pAttr)
	{
		if (pAttr == nullptr)
		{
			return;
		}
		if (pAttr->m_flag & CBaseScriptClassAttribute::E_FLAG_DB)
		{
			m_mapDBAttributes[pAttr->m_strAttrName] = pAttr;
		}
		m_mapAllAttributes.insert(std::pair<unsigned int, CBaseScriptClassAttribute*>(pAttr->m_index, pAttr));
		m_mapAttributeName2Index.insert(std::pair<std::string, unsigned int>(pAttr->m_strAttrName, pAttr->m_index));
	}

	void CScriptPointInterface::RemoveScriptAttribute(CBaseScriptClassAttribute* pAttr)
	{
		if (pAttr == nullptr)
		{
			return;
		}
	}

	void CScriptPointInterface::RegisterScriptFun(CBaseScriptClassFun* pClassFun)
	{
		if (pClassFun)
		{
			pClassFun->m_index = m_vecScriptClassFun.size();
			m_mapFun2Index[pClassFun->m_name] = pClassFun->m_index;
			//m_mapFun2Index.insert(std::pair<std::string, unsigned int>(pClassFun->m_name, pClassFun->m_index));
			m_vecScriptClassFun.push_back(pClassFun);
		}

	}

	unsigned int CScriptPointInterface::GetClassFunIndex(std::string name)
	{
		auto it = m_mapFun2Index.find(name);
		if (it != m_mapFun2Index.end())
		{
			return it->second;
		}
		return -1;
	}

	CBaseScriptClassFun* CScriptPointInterface::GetClassFunInfo(unsigned int id)
	{
		if (id >= m_vecScriptClassFun.size())
		{
			return 0;
		}
		return m_vecScriptClassFun[id];
	}

	bool CScriptPointInterface::AddVar2Bytes(std::vector<char>& vBuff, StackVarInfo* pVal, std::vector<PointVarInfo>& vOutClassPoint)
	{
		if (!pVal)
		{
			return false;
		}
		switch (pVal->cType)
		{
		case EScriptVal_Int:
		{
			AddChar2Bytes(vBuff, EScriptVal_Int);
			AddInt642Bytes(vBuff, pVal->Int64);
		}
		break;
		case EScriptVal_Double:
		{
			AddChar2Bytes(vBuff, EScriptVal_Double);
			AddDouble2Bytes(vBuff, pVal->Double);
		}
		break;
		case EScriptVal_String:
		{
			AddChar2Bytes(vBuff, EScriptVal_String);
			const char* pStr = StackVarInfo::s_strPool.GetString(pVal->Int64);
			AddString2Bytes(vBuff, (char*)pStr);
		}
		break;
		case EScriptVal_Binary:
		{
			AddChar2Bytes(vBuff, EScriptVal_Binary);
			unsigned int size = 0;
			const char* pStr = StackVarInfo::s_binPool.GetBinary(pVal->Int64, size);
			::AddData2Bytes(vBuff, pStr, size);
		}
		case EScriptVal_ClassPoint:
		{
			auto pPoint = pVal->pPoint;
			if (pPoint)
			{
				AddChar2Bytes(vBuff, EScriptVal_ClassPoint);
				AddUInt2Bytes(vBuff, vOutClassPoint.size());
				vOutClassPoint.push_back(pPoint);
			}
			else
			{
				//TODO 错误
				AddChar2Bytes(vBuff, EScriptVal_None);
			}
		}
		break;
		default:
			AddChar2Bytes(vBuff, EScriptVal_None);
			break;
		}
		return true;
	}

	bool CScriptPointInterface::AddVar2Bytes(std::vector<char>& vBuff, PointVarInfo* pVal, std::vector<PointVarInfo>& vOutClassPoint)
	{
		if (!pVal)
		{
			return false;
		}

		auto pPoint = pVal->pPoint;
		if (pPoint)
		{
			AddChar2Bytes(vBuff, EScriptVal_ClassPoint);
			AddUInt2Bytes(vBuff, vOutClassPoint.size());
			vOutClassPoint.push_back(*pVal);
		}
		else
		{
			//TODO 错误
			AddChar2Bytes(vBuff, EScriptVal_None);
		}
		return true;
	}

	StackVarInfo CScriptPointInterface::DecodeVar4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint)
	{
		char cType = DecodeBytes2Char(pBuff, pos, len);
		switch (cType)
		{
		case EScriptVal_None:
			break;
		case EScriptVal_Int:
			{
				__int64 nVal = DecodeBytes2Int64(pBuff, pos, len);
				return StackVarInfo(nVal);
			}
			break;
		case EScriptVal_Double:
			{
				double fVal = DecodeBytes2Double(pBuff, pos, len);
				return StackVarInfo(fVal);
			}
			break;
		case EScriptVal_String:
			{
				int nStrLen = DecodeBytes2Int(pBuff, pos, len);
				//if (nStrLen )
				std::vector<char> strTemp;
				for (int i = 0; i < nStrLen && pos < len; i++)
				{
					strTemp.push_back(*(pBuff + pos));
					pos++;
				}
				strTemp.push_back('\0');
				return StackVarInfo(&strTemp[0]);
			}
			break;
		case EScriptVal_Binary:
			{
				int nStrLen = DecodeBytes2Int(pBuff, pos, len);
				if (nStrLen <= len - pos)
				{
					StackVarInfo var;
					var.cType = EScriptVal_Binary;
					var.Int64 = StackVarInfo::s_binPool.NewBinary(pBuff + pos, nStrLen);
					return var;
				}
			}
			break;
		case EScriptVal_ClassPoint:
			{
				unsigned int index = DecodeBytes2Int(pBuff, pos, len);
				if (index < vOutClassPoint.size())
				{
					return StackVarInfo(vOutClassPoint[index].pPoint);
				}
			}
			break;
		}
		return StackVarInfo();
	}

	PointVarInfo CScriptPointInterface::DecodePointVar4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint)
	{
		char cType = DecodeBytes2Char(pBuff, pos, len);
		switch (cType)
		{
		case EScriptVal_None:
			break;
		case EScriptVal_ClassPoint:
		{
			unsigned int index = DecodeBytes2Int(pBuff, pos, len);
			if (index < vOutClassPoint.size())
			{
				return vOutClassPoint[index];
			}
		}
		break;
		}
		return PointVarInfo();
	}

	void CScriptPointInterface::GetBaseClassInfo(stScriptClassInfo& info)
	{
		for (auto it = m_vecScriptClassFun.begin(); it != m_vecScriptClassFun.end(); it++)
		{
			info.mapDicFunString2Index[(*it)->m_name] = (*it)->m_index;
		}
		for (auto it = m_mapAllAttributes.begin(); it != m_mapAllAttributes.end(); it++)
		{
			auto pAttr = it->second;
			if (pAttr)
			{
				stScriptClassParamInfo paramInfo;
				paramInfo.m_flag = pAttr->m_flag;
				paramInfo.m_index = pAttr->m_index;
				paramInfo.m_strAttrName = pAttr->m_strAttrName;
				paramInfo.strType = pAttr->ToType();
				info.mapDicString2ParamInfo[paramInfo.m_strAttrName] = paramInfo;
			}
		}
	}

}