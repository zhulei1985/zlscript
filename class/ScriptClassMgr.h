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
#include <type_traits>
#include "ScriptPointInterface.h"
#include "ScriptSuperPointer.h"

namespace zlscript
{
	class CScriptPointInterface;
	class CScriptBasePointer;
	class CBaseScriptClassMgr
	{
	public:
		CBaseScriptClassMgr();
		~CBaseScriptClassMgr();

		virtual CScriptPointInterface* New(int autorelease) = 0;
		//virtual CScriptPointInterface* NewImage(__int64 nID) = 0;
		virtual CScriptPointInterface* Get(__int64 nID) = 0;
		//virtual CScriptPointInterface* GetImage(__int64 nID) = 0;
		virtual void Release(CScriptPointInterface* pPoint) ;

		virtual void Release(CScriptBasePointer* pPoint);

		virtual void SetDBIdCount(__int64 val) = 0;
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

		virtual CScriptPointInterface* New(int autorelease);
		//virtual CScriptPointInterface* New(__int64 nID);
		virtual CScriptPointInterface* Get(__int64 nID);

		virtual void Release(CScriptPointInterface* pPoint);

		virtual void SetDBIdCount(__int64 val);
	public:
		static CBaseScriptClassMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		__int64 m_nIDCount;

		//本地类实例
		std::unordered_map<__int64, T*> m_mapLocalClassPoint;


		std::mutex m_MutexLock;

		static CScriptClassMgr s_Instance;
	};

	template<class T>
	CScriptClassMgr<T> CScriptClassMgr<T>::s_Instance;

	template<class T>
	inline CScriptPointInterface* CScriptClassMgr<T>::New(int autorelease)
	{
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		//TODO 以后做缓存优化

			T* pPoint = new T;
			if (pPoint)
			{
				//pPoint->SetID(++m_nIDCount);
				if (!CScriptSuperPointerMgr::GetInstance()->SetPointAutoRelease(pPoint->GetScriptPointIndex(), autorelease))
				{
					CScriptSuperPointerMgr::GetInstance()->SetClassPoint(pPoint->GetScriptPointIndex(), pPoint, autorelease);
				}
				m_mapLocalClassPoint[pPoint->GetScriptPointIndex()] = pPoint;
			}
			return pPoint;
	}


	template<class T>
	inline CScriptPointInterface* CScriptClassMgr<T>::Get(__int64 nID)
	{
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		auto it = m_mapLocalClassPoint.find(nID);
		if (it != m_mapLocalClassPoint.end())
		{
			return it->second;
		}
		return nullptr;
	}


	template<class T>
	inline void CScriptClassMgr<T>::Release(CScriptPointInterface* pPoint)
	{
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		//if (pPoint->GetType() == CScriptPointInterface::E_TYPE_LOCAL)
		//{
			m_mapLocalClassPoint.erase(pPoint->GetScriptPointIndex());
		//}
		//else if(pPoint->GetType() == CScriptPointInterface::E_TYPE_IMAGE)
		//{
		//	m_mapImageClassPoint.erase(pPoint->GetID());
		//}
		CBaseScriptClassMgr::Release(pPoint);
		//TODO 以后做缓存优化
		if (pPoint)
			delete pPoint;
	}

	template<class T>
	inline void CScriptClassMgr<T>::SetDBIdCount(__int64 val)
	{
		CScriptSuperPointer<T>::s_Info.nDB_Id_Count = val;
	}

	template<class T>
	class CScriptAbstractClassMgr : public CBaseScriptClassMgr
	{
	public:
		CScriptAbstractClassMgr()
		{
			m_nIDCount = 0;
		}
		~CScriptAbstractClassMgr()
		{

		}

		virtual CScriptPointInterface* New(int autorelease);
		//virtual CScriptPointInterface* New(__int64 nID);
		virtual CScriptPointInterface* Get(__int64 nID);

		virtual void Release(CScriptPointInterface* pPoint);

		void SetDBIdCount(__int64 val);
	public:
		static CBaseScriptClassMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		__int64 m_nIDCount;

		//本地类实例
		std::unordered_map<__int64, T*> m_mapLocalClassPoint;


		std::mutex m_MutexLock;

		static CScriptAbstractClassMgr s_Instance;
	};

	template<class T>
	CScriptAbstractClassMgr<T> CScriptAbstractClassMgr<T>::s_Instance;

	template<class T>
	inline CScriptPointInterface* CScriptAbstractClassMgr<T>::New(int autorelease)
	{

		return nullptr;
	}


	template<class T>
	inline CScriptPointInterface* CScriptAbstractClassMgr<T>::Get(__int64 nID)
	{
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		auto it = m_mapLocalClassPoint.find(nID);
		if (it != m_mapLocalClassPoint.end())
		{
			return it->second;
		}
		return nullptr;
	}


	template<class T>
	inline void CScriptAbstractClassMgr<T>::Release(CScriptPointInterface* pPoint)
	{
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		//if (pPoint->GetType() == CScriptPointInterface::E_TYPE_LOCAL)
		//{
		m_mapLocalClassPoint.erase(pPoint->GetScriptPointIndex());
		//}
		//else if(pPoint->GetType() == CScriptPointInterface::E_TYPE_IMAGE)
		//{
		//	m_mapImageClassPoint.erase(pPoint->GetID());
		//}
		CBaseScriptClassMgr::Release(pPoint);
		//TODO 以后做缓存优化
		if (pPoint)
			delete pPoint;
	}

	template<class T>
	inline void CScriptAbstractClassMgr<T>::SetDBIdCount(__int64 val)
	{
		CScriptSuperPointer<T>::s_Info.nDB_Id_Count = val;
	}

	template<class T>
	inline CBaseScriptClassMgr* GetScriptClassMgr()
	{
		typedef std::conditional<std::is_abstract<T>::value, CScriptAbstractClassMgr<T>, CScriptClassMgr<T>>::type ClassMgr;
		return ClassMgr::GetInstance();
	}

}