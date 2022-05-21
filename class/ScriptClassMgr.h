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
#include "ScriptVarInfo.h"
namespace zlscript
{
	class CScriptBasePointer;
	class CBaseScriptVarMgr
	{
	public:
		CBaseScriptVarMgr();
		~CBaseScriptVarMgr();

		virtual CBaseVar* New(int autorelease) = 0;

		virtual void Release(CBaseVar* pPoint) ;

	public:
		void SetType(int val)
		{
			m_nType = val;
		}
	protected:
		int m_nType;
	};

	template<class T>
	class CScriptVarMgr : public CBaseScriptVarMgr
	{
	public:
		CScriptVarMgr()
		{

		}
		~CScriptVarMgr()
		{

		}

		CBaseVar* New(int autorelease);
		void Release(CBaseVar* pPoint);
	
	protected:
		std::list<CBaseVar*> cache;
	};


	template<class T>
	inline CBaseVar* CScriptVarMgr<T>::New(int autorelease)
	{
		CBaseVar* pPoint = nullptr;
		if (!cache.empty())
		{
			pPoint = cache.front();
			cache.pop_front();
		}
		if (pPoint == nullptr)
		{
			pPoint = new T;
			if (pPoint)
			{
				pPoint->SetType(m_nType);
			}

		}
		return pPoint;
	}

	template<class T>
	inline void CScriptVarMgr<T>::Release(CBaseVar* pPoint)
	{
		if (pPoint)
			cache.push_back(pPoint);
		//if (pPoint)
		//	delete pPoint;
	}


	template<class T>
	class CScriptRealClassMgr
	{
	public:
		CScriptRealClassMgr()
		{

		}
		~CScriptRealClassMgr()
		{

		}

		virtual CScriptPointInterface* New(int autorelease);

		virtual void Release(CScriptPointInterface* pPoint);

		void SetDBIdCount(__int64 val);
	public:

	private:
		__int64 m_nDBIDCount;

	};


	template<class T>
	inline CScriptPointInterface* CScriptRealClassMgr<T>::New(int autorelease)
	{
		//TODO 以后做缓存优化
		T* pPoint = new T;
		if (pPoint)
		{
			pPoint->SetClassInfo(&CScriptClassInfo<T>::GetInstance());
		}
		return pPoint;
	}

	template<class T>
	inline void CScriptRealClassMgr<T>::Release(CScriptPointInterface* pPoint)
	{
		//TODO 以后做缓存优化
		if (pPoint)
			delete pPoint;
	}

	template<class T>
	inline void CScriptRealClassMgr<T>::SetDBIdCount(__int64 val)
	{
		m_nDBIDCount = val;
	}

	class CBaseScriptClassMgr : public CBaseScriptVarMgr
	{
	public:
		CBaseScriptClassMgr(){}

		virtual CScriptPointInterface* NewObject() = 0;

		virtual void Release(CScriptPointInterface* pPoint) = 0;
	};

	template<class T>
	class CScriptClassVarMgr : public CBaseScriptClassMgr
	{
	public:
		CScriptClassVarMgr()
		{
			//m_nIDCount = 0;
		}
		~CScriptClassVarMgr()
		{

		}

		virtual CBaseVar* New(int autorelease);

		virtual void Release(CBaseVar* pPoint);

		virtual CScriptPointInterface* NewObject();

		virtual void Release(CScriptPointInterface* pPoint);

	public:
		CScriptRealClassMgr<T> m_realClassMgr;
	};

	template<class T>
	inline CBaseVar* CScriptClassVarMgr<T>::New(int autorelease)
	{
		//std::lock_guard<std::mutex> Lock(m_MutexLock);
		//TODO 以后做缓存优化

			CPointVar* pPoint = new CPointVar;
			if (pPoint)
			{
				pPoint->SetType(m_nType);
			}
			return pPoint;
	}

	template<class T>
	inline void CScriptClassVarMgr<T>::Release(CBaseVar* pPoint)
	{
		CPointVar* pVar= dynamic_cast<CPointVar*>(pPoint);
		if (pVar)
		{
			delete pVar;
		}
	}

	template<class T>
	inline CScriptPointInterface* CScriptClassVarMgr<T>::NewObject()
	{
		return m_realClassMgr.New(1);
	}


	template<class T>
	inline void CScriptClassVarMgr<T>::Release(CScriptPointInterface* pPoint)
	{
		//TODO 以后做缓存优化
		m_realClassMgr.Release(pPoint);
	}



}