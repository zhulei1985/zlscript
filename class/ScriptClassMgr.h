#pragma once

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
#include <mutex>
#include <unordered_map>
#include "ScriptPointInterface.h"
namespace zlscript
{
	class CScriptPointInterface;
	class CScriptBasePointer;
	class CBaseScriptClassMgr
	{
	public:
		CBaseScriptClassMgr();
		~CBaseScriptClassMgr();

		virtual CScriptPointInterface* New() = 0;
		virtual CScriptPointInterface* New(__int64 nID) = 0;
		virtual CScriptPointInterface* Get(__int64 nID) = 0;
		virtual void Release(CScriptPointInterface* pPoint) ;

		virtual void Release(CScriptBasePointer* pPoint);

	};

	template<class T>
	class CScriptClassMgr : public CBaseScriptClassMgr
	{
	public:
		CScriptClassMgr()
		{
			m_nIDCount = 0;
		}
		~CScriptClassMgr()
		{

		}

		virtual CScriptPointInterface* New();
		virtual CScriptPointInterface* New(__int64 nID);
		virtual CScriptPointInterface* Get(__int64 nID);

		virtual void Release(CScriptPointInterface* pPoint);
	public:
		static CBaseScriptClassMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		__int64 m_nIDCount;
		std::unordered_map<__int64, T*> m_mapClassPoint;

		std::mutex m_MutexLock;

		static CScriptClassMgr s_Instance;
	};

	template<class T>
	CScriptClassMgr<T> CScriptClassMgr<T>::s_Instance;

	template<class T>
	inline CScriptPointInterface* CScriptClassMgr<T>::New()
	{
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		T* pPoint = new T;
		if (pPoint)
		{
			pPoint->SetID(++m_nIDCount);
			m_mapClassPoint[pPoint->GetID()] = pPoint;
		}
		return pPoint;
	}

	template<class T>
	inline CScriptPointInterface* CScriptClassMgr<T>::New(__int64 nID)
	{
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		T* pPoint = dynamic_cast<T*>(Get(nID));
		if (pPoint == nullptr)
		{
			pPoint = new T;
			if (pPoint)
			{
				pPoint->SetID(nID);
				m_mapClassPoint[pPoint->GetID()] = pPoint;
			}
		}
		return pPoint;
	}

	template<class T>
	inline CScriptPointInterface* CScriptClassMgr<T>::Get(__int64 nID)
	{
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		auto it = m_mapClassPoint.find(nID);
		if (it != m_mapClassPoint.end())
		{
			return it->second;
		}
		return nullptr;
	}

	template<class T>
	inline void CScriptClassMgr<T>::Release(CScriptPointInterface* pPoint)
	{
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		CBaseScriptClassMgr::Release(pPoint);
		if (pPoint)
			delete pPoint;
	}

}