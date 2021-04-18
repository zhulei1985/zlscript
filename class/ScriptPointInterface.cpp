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
		auto it = m_vecScriptClassFun.begin();
		for (; it != m_vecScriptClassFun.end(); it++)
		{
			CScriptBaseClassFunInfo* pInfo = *it;
			if (pInfo)
			{
				delete pInfo;
			}
		}
		m_vecScriptClassFun.clear();

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

	void CScriptPointInterface::SetFun(int id, CScriptBaseClassFunInfo* pInfo)
	{
		if (pInfo == nullptr)
		{
			return;
		}
		if (id >= 0)
		{
			if ((int)m_vecScriptClassFun.size() == id)
			{
				m_vecScriptClassFun.push_back(pInfo);
			}
			else if ((int)m_vecScriptClassFun.size() < id)
			{
				m_vecScriptClassFun.resize(id + 1,nullptr);
				m_vecScriptClassFun[id] = pInfo;
			}
			else
			{
				if (m_vecScriptClassFun[id])
				{
					delete m_vecScriptClassFun[id];
				}
				m_vecScriptClassFun[id] = pInfo;
			}
		}
		else
		{
			delete pInfo;
		}
	}
	int CScriptPointInterface::RunFun(int id, CScriptRunState* pState)
	{
		if (id < 0)
		{
			return 0;
		}
		if (id >= m_vecScriptClassFun.size())
		{
			return 0;
		}
		int nResult = 0;
		CScriptBaseClassFunInfo* pFun = m_vecScriptClassFun[id];
		if (pFun)
		{
			nResult = pFun->RunFun(pState);
		}
		return nResult;
	}

	//int CScriptPointInterface::CallFun(const char* pFunName, CScriptStack& parms)
	//{
	//	if (pFunName == nullptr)
	//	{
	//		return 0;
	//	}
	//	if (m_pClassInfo == nullptr)
	//	{
	//		return 0;
	//	}
	//	int nResult = 0;
	//	auto it = m_pClassInfo->mapDicString2Index.find(pFunName);
	//	if (it != m_pClassInfo->mapDicString2Index.end())
	//	{
	//		CTempScriptRunState TempState;
	//		TempState.CopyFromStack(&parms);
	//		RunFun(it->second, &TempState);
	//	}
	//	return nResult;
	//}

	CScriptPointInterface::CScriptPointInterface(const CScriptPointInterface& val)
	{
		//AddClassObject(CScriptPointInterface::GetScriptPointIndex(), this);
		//this->m_nID = val.m_nID;
		auto itOld = m_vecScriptClassFun.begin();
		for (; itOld != m_vecScriptClassFun.end(); itOld++)
		{
			CScriptBaseClassFunInfo* pInfo = *itOld;
			if (pInfo)
			{
				delete pInfo;
			}
		}
		m_vecScriptClassFun.clear();
		//this->m_mapScriptClassFun = val.m_mapScriptClassFun;
		auto it = val.m_vecScriptClassFun.begin();
		for (; it != val.m_vecScriptClassFun.end(); it++)
		{
			m_vecScriptClassFun.push_back((*it)->Copy());
		}
	}
	CScriptPointInterface& CScriptPointInterface::operator=(const CScriptPointInterface& val)
	{
		//this->m_nID = val.m_nID;
		//this->m_mapScriptClassFun = val.m_mapScriptClassFun;
		// TODO: 在此处插入 return 语句

		auto itOld = m_vecScriptClassFun.begin();
		for (; itOld != m_vecScriptClassFun.end(); itOld++)
		{
			CScriptBaseClassFunInfo* pInfo = *itOld;
			if (pInfo)
			{
				delete pInfo;
			}
		}
		m_vecScriptClassFun.clear();
		//this->m_mapScriptClassFun = val.m_mapScriptClassFun;
		auto it = val.m_vecScriptClassFun.begin();
		for (; it != val.m_vecScriptClassFun.end(); it++)
		{
			m_vecScriptClassFun.push_back((*it)->Copy());
		}

		return *this;
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
	}

	void CScriptPointInterface::RemoveScriptAttribute(CBaseScriptClassAttribute* pAttr)
	{
		if (pAttr == nullptr)
		{
			return;
		}
	}

	CScriptBaseClassFunInfo* CScriptPointInterface::GetClassFunInfo(int id)
	{
		if (id < 0)
		{
			return 0;
		}
		if (id >= m_vecScriptClassFun.size())
		{
			return 0;
		}
		return m_vecScriptClassFun[id];
	}

}