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

#include "ScriptSuperPointer.h"
#include "ScriptDataMgr.h"

namespace zlscript
{
	__int64 CScriptPointInterface::s_nIDCount = 0;
	CScriptPointInterface::CScriptPointInterface()
	{
		m_nID = 0;
	}
	CScriptPointInterface::~CScriptPointInterface()
	{
		std::map<int, CScriptBaseClassFunInfo*>::iterator it = m_mapScriptClassFun.begin();
		for (; it != m_mapScriptClassFun.end(); it++)
		{
			CScriptBaseClassFunInfo* pInfo = it->second;
			if (pInfo)
			{
				delete pInfo;
			}
		}
		m_mapScriptClassFun.clear();
	}
	void CScriptPointInterface::InitScriptPointIndex()
	{
		if (!IsInitScriptPointIndex())
		{
			s_nIDCount++;
			m_nID = s_nIDCount;
		}

	}
	bool CScriptPointInterface::IsInitScriptPointIndex()
	{
		if (m_nID > 0)
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
		return m_nID;
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
		//std::lock_guard<std::mutex> _lock{ *m_FunLock };
		m_FunLock.lock();
		std::map<int, CScriptBaseClassFunInfo*>::iterator it = m_mapScriptClassFun.find(id);
		if (it != m_mapScriptClassFun.end())
		{
			CScriptBaseClassFunInfo* pOld = it->second;
			if (pOld)
			{
				nResult = pOld->RunFun(pState);
			}
		}
		//std::lock_guard<std::mutex> _unlock{ *m_FunLock };
		m_FunLock.unlock();
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
	__int64 CScriptBasePointer::GetID()
	{
		return m_ID;
	}

	void CScriptBasePointer::SetID(__int64 id)
	{
		m_ID = id;
	}

	CScriptSuperPointerMgr CScriptSuperPointerMgr::s_Instance;

	void CScriptSuperPointerMgr::Init()
	{
		CScriptArray::Init2Script();

		CScriptData::Init2Script();
	}

	void CScriptSuperPointerMgr::Clear()
	{
		m_MutexLock.lock();
		{
			std::map<__int64, CScriptBasePointer*>::iterator it = m_mapPointer.begin();
			for (; it != m_mapPointer.end(); it++)
			{
				CScriptBasePointer* pPoint = it->second;
				if (pPoint)
				{
					delete pPoint;
				}
			}
		}
		m_MutexLock.unlock();
		m_MutexTypeLock.lock();
		{
			std::map<__int64, CScriptBasePointer*>::iterator it = m_mapTypePointer.begin();
			for (; it != m_mapTypePointer.end(); it++)
			{
				CScriptBasePointer* pPoint = it->second;
				if (pPoint)
				{
					delete pPoint;
				}
			}
		}

		m_MutexTypeLock.unlock();
	}

	void CScriptSuperPointerMgr::ClearPointer()
	{
		m_MutexLock.lock();
		std::map<__int64, CScriptBasePointer*>::iterator it = m_mapPointer.begin();
		for (; it != m_mapPointer.end(); it++)
		{
			CScriptBasePointer* pPoint = it->second;
			if (pPoint)
			{
				delete pPoint;
			}
		}
		m_MutexLock.unlock();
	}

	bool CScriptSuperPointerMgr::RemoveClassPoint(__int64 nID)
	{
		m_MutexLock.lock();
		bool bResult = false;
		std::map<__int64, CScriptBasePointer*>::iterator it = m_mapPointer.find(nID);
		if (it != m_mapPointer.end())
		{
			CScriptBasePointer* pPoint = it->second;

			pPoint->SetPointer(nullptr);


			m_mapPointer.erase(it);
			if (pPoint->m_nUseCount <= 0)
			{
				delete pPoint;
			}
			bResult = true;
		}
		m_MutexLock.unlock();
		return bResult;
	}

	int CScriptSuperPointerMgr::GetClassType(std::string classname)
	{
		if (m_mapString2ClassType.find(classname) != m_mapString2ClassType.end())
		{
			return m_mapString2ClassType[classname];
		}
		return 0;
	}

	CScriptBasePointer* CScriptSuperPointerMgr::PickupPointer(__int64 id)
	{
		CScriptBasePointer* pResult = nullptr;
		m_MutexLock.lock();
		std::map<__int64, CScriptBasePointer*>::iterator it = m_mapPointer.find(id);
		if (it != m_mapPointer.end())
		{
			pResult = it->second;
		}
		if (pResult)
		{
			pResult->m_nUseCount++;
		}
		m_MutexLock.unlock();
		return pResult;
	}

	void CScriptSuperPointerMgr::ReturnPointer(CScriptBasePointer* pPointer)
	{
		m_MutexLock.lock();
		if (pPointer)
		{
			pPointer->m_nUseCount--;
			if (pPointer->m_nUseCount <= 0 && pPointer->GetPoint() == nullptr)
			{
				std::map<__int64, CScriptBasePointer*>::iterator it = m_mapPointer.find(pPointer->GetID());
				if (it != m_mapPointer.end())
				{
					m_mapPointer.erase(it);
				}
				delete pPointer;
			}
		}
		m_MutexLock.unlock();
	}

	int CScriptSuperPointerMgr::GetClassFunIndex(int classindex, std::string funname)
	{
		int nResult = -1;
		m_MutexTypeLock.lock();
		std::map<__int64, CScriptBasePointer*>::iterator it = m_mapTypePointer.find(classindex);
		if (it != m_mapTypePointer.end())
		{
			CScriptBasePointer* pPointer = it->second;
			if (pPointer)
			{
				nResult = pPointer->GetFunIndex(funname);
			}
		}
		m_MutexTypeLock.unlock();
		return nResult;
	}
}