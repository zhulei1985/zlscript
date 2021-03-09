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
		auto it = m_mapScriptClassFun.begin();
		for (; it != m_mapScriptClassFun.end(); it++)
		{
			CScriptBaseClassFunInfo* pInfo = it->second;
			if (pInfo)
			{
				delete pInfo;
			}
		}
		m_mapScriptClassFun.clear();

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
		std::map<int, CScriptBaseClassFunInfo*>::iterator it = m_mapScriptClassFun.find(id);
		if (it != m_mapScriptClassFun.end())
		{
			CScriptBaseClassFunInfo* pOld = it->second;
			if (pOld)
			{
				delete pOld;
			}
		}
		m_mapScriptClassFun[id] = pInfo;
	}
	int CScriptPointInterface::RunFun(int id, CScriptRunState* pState)
	{
		int nResult = 0;
		//m_FunLock.lock();
		auto it = m_mapScriptClassFun.find(id);
		if (it != m_mapScriptClassFun.cend())
		{
			CScriptBaseClassFunInfo* pOld = it->second;
			if (pOld)
			{
				nResult = pOld->RunFun(pState);
			}
		}
		//m_FunLock.unlock();
		return nResult;
	}
	CScriptPointInterface::CScriptPointInterface(const CScriptPointInterface& val)
	{
		//AddClassObject(CScriptPointInterface::GetScriptPointIndex(), this);
		//this->m_nID = val.m_nID;
		auto itOld = m_mapScriptClassFun.begin();
		for (; itOld != m_mapScriptClassFun.end(); itOld++)
		{
			CScriptBaseClassFunInfo* pInfo = itOld->second;
			if (pInfo)
			{
				delete pInfo;
			}
		}
		m_mapScriptClassFun.clear();
		//this->m_mapScriptClassFun = val.m_mapScriptClassFun;
		auto it = val.m_mapScriptClassFun.begin();
		for (; it != val.m_mapScriptClassFun.end(); it++)
		{
			m_mapScriptClassFun[it->first] = it->second->Copy();
		}
	}
	CScriptPointInterface& CScriptPointInterface::operator=(const CScriptPointInterface& val)
	{
		//this->m_nID = val.m_nID;
		//this->m_mapScriptClassFun = val.m_mapScriptClassFun;
		// TODO: 在此处插入 return 语句

		auto itOld = m_mapScriptClassFun.begin();
		for (; itOld != m_mapScriptClassFun.end(); itOld++)
		{
			CScriptBaseClassFunInfo* pInfo = itOld->second;
			if (pInfo)
			{
				delete pInfo;
			}
		}
		m_mapScriptClassFun.clear();
		//this->m_mapScriptClassFun = val.m_mapScriptClassFun;
		auto it = val.m_mapScriptClassFun.begin();
		for (; it != val.m_mapScriptClassFun.end(); it++)
		{
			m_mapScriptClassFun[it->first] = it->second->Copy();
		}

		return *this;
	}

	void CScriptPointInterface::ChangeScriptAttribute(short flag, CBaseScriptClassAttribute* pAttr)
	{
		if (flag & CBaseScriptClassAttribute::E_FLAG_DB)
		{

		}
	}

	void CScriptPointInterface::RegisterScriptClassAttr(short flag, CBaseScriptClassAttribute* pAttr)
	{
		if (flag & CBaseScriptClassAttribute::E_FLAG_DB)
		{
			if (pAttr)
				m_mapDBAttributes[pAttr->m_strAttrName] = pAttr;
		}
	}

}