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
	__int64 CScriptBasePointer::GetID()
	{
		return m_ID;
	}

	void CScriptBasePointer::SetID(__int64 id)
	{
		m_ID = id;
	}

	void CScriptBasePointer::SetAutoReleaseMode(int val)
	{
		m_nAutoReleaseMode = val;
	}

	int CScriptBasePointer::GetAutoReleaseMode()
	{
		return m_nAutoReleaseMode;
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

	void CScriptSuperPointerMgr::AddPoint2Release(__int64 id)
	{
		m_autoReleaseIds.insert(id);
	}

	void CScriptSuperPointerMgr::ReleaseAutoPoint()
	{
		std::vector<CScriptPointInterface*> vPoints;
		//std::lock_guard<std::mutex> Lock(m_MutexLock);
		m_MutexLock.lock();
		auto itId = m_autoReleaseIds.begin();
		for (; itId != m_autoReleaseIds.end();)
		{
			std::map<__int64, CScriptBasePointer*>::iterator it = m_mapPointer.find(*itId);
			if (it != m_mapPointer.end())
			{
				CScriptBasePointer* pPoint = it->second;
				if (pPoint->m_nUseCount > 0)
				{
					itId++;
					//TODO 应该删除的指针，还有地方引用，不应该
					continue;
				}
				CBaseScriptClassMgr* pMgr = CScriptSuperPointerMgr::GetInstance()->GetClassMgr(pPoint->GetType());
				if (pPoint->GetPoint())
				{
					pPoint->GetPoint()->ClearScriptPointIndex();
					vPoints.push_back(pPoint->GetPoint());
					//delete pPoint->GetPoint();
				}

				pPoint->SetPointer(nullptr);

				m_mapPointer.erase(it);
				if (pPoint->m_nUseCount <= 0)
				{
					delete pPoint;
				}
			}
			itId = m_autoReleaseIds.erase(itId);
		}
		m_MutexLock.unlock();

		for (unsigned int i = 0; i < vPoints.size(); i++)
		{
			delete vPoints[i];
		}
	}

	bool CScriptSuperPointerMgr::SetPointAutoRelease(__int64 nID, int autorelease)
	{
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		std::map<__int64, CScriptBasePointer*>::iterator it = m_mapPointer.find(nID);
		if (it != m_mapPointer.end())
		{
			auto pPoint = it->second;
			if (pPoint)
			{
				pPoint->SetAutoReleaseMode(autorelease);
				return true;
			}
		}
		return false;
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

	CScriptBasePointer* CScriptSuperPointerMgr::PickupPointer(__int64 id, std::string className)
	{
		CScriptBasePointer* pResult = nullptr;
		m_MutexLock.lock();
		std::map<__int64, CScriptBasePointer*>::iterator it = m_mapPointer.find(id);
		if (it != m_mapPointer.end())
		{
			pResult = it->second;
			if (className != pResult->ClassName())
			{
				pResult = nullptr;
			}
		}
		if (pResult)
		{
			pResult->m_nUseCount++;
		}
		m_MutexLock.unlock();
		return pResult;
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

	bool CScriptSuperPointerMgr::ScriptUsePointer(__int64 id)
	{
		if (id == 0)
		{
			return false;
		}
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		std::map<__int64, CScriptBasePointer*>::iterator it = m_mapPointer.find(id);
		if (it != m_mapPointer.end())
		{
			auto pPoint = it->second;
			if (pPoint)
			{
				pPoint->m_nScriptUseCount++;
			}
			return true;
		}
		return false;
	}

	bool CScriptSuperPointerMgr::ScriptReleasePointer(__int64 id)
	{
		if (id == 0)
		{
			return false;
		}
		std::lock_guard<std::mutex> Lock(m_MutexLock);

		std::map<__int64, CScriptBasePointer*>::iterator it = m_mapPointer.find(id);
		if (it != m_mapPointer.end())
		{
			auto pPoint = it->second;
			if (pPoint)
			{
				if (pPoint->m_nScriptUseCount < 1)
				{
					pPoint->m_nScriptUseCount = 0;
					//标记是否可以释放
					if (pPoint->GetAutoReleaseMode() & SCRIPT_NO_USED_AUTO_RELEASE)
					{
						bool bRelease = true;
						pPoint->Lock();
						if (pPoint->GetPoint() && !pPoint->GetPoint()->CanRelease())
						{
							bRelease = false;
						}
						pPoint->Unlock();
						if (bRelease)
							m_autoReleaseIds.insert(pPoint->GetID());
					}
				}
				else
				{
					pPoint->m_nScriptUseCount--;
				}
			}
			return true;
		}
		return false;
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
	CBaseScriptClassMgr* CScriptSuperPointerMgr::GetClassMgr(int nType)
	{
		auto it = m_mapClassMgr.find(nType);
		if (it != m_mapClassMgr.end())
		{
			return it->second;
		}
		return nullptr;
	}
}