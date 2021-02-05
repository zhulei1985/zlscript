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

#pragma once

#include <mutex>
#include <atomic>
#include <map>
#include <functional> 
#include <string>
#include <vector>

#include "ScriptPointInterface.h"
#define SCRIPT_NO_USED_AUTO_RELEASE 1
namespace zlscript
{

	class CScriptBasePointer
	{
	public:
		CScriptBasePointer(){
			m_ID = 0;
			m_nUseCount = 0;
			m_nScriptUseCount = 0;
			m_nAutoReleaseMode = false;
		}
		virtual ~CScriptBasePointer(){}
	public:
		virtual const char* ClassName() = 0;
		virtual int GetType() = 0;
		virtual __int64 GetID();
		virtual void SetID(__int64 id);
		void SetAutoReleaseMode(int val);
		int GetAutoReleaseMode();

		virtual int RunFun(unsigned int nIndex, CScriptRunState*) = 0;
		virtual int GetFunIndex(std::string name) = 0;

		virtual CScriptPointInterface* GetPoint()
		{
			return nullptr;
		}
		virtual void SetPointer(CScriptPointInterface* Point) = 0;
		virtual void Lock() = 0;
		virtual void Unlock() = 0;
	protected:
		long m_ID;
		int m_nAutoReleaseMode;
	public:
		std::atomic_int m_nUseCount;
		std::atomic_int m_nScriptUseCount;
	};

	template<class T>
	class CScriptSuperPointer : public CScriptBasePointer
	{
	public:
		virtual const char* ClassName()
		{
			return s_strClassName.c_str();
		}
		int GetType();
		//typedef int (T::*ClassFun)(CScriptRunState *);
		virtual CScriptPointInterface* GetPoint()
		{
			return m_pPointer;
		}
	public:
		CScriptSuperPointer()
		{
			m_pPointer = nullptr;
			//m_nCount = 0;
			//m_nUseCount = 0;
		}

		int RunFun(unsigned int nIndex, CScriptRunState*);
		int GetFunIndex(std::string name);

		virtual void Lock();
		virtual void Unlock();
	public:
		void SetPointer(CScriptPointInterface* Point);
	private:
		T* m_pPointer;

		std::recursive_mutex m_MutexLock;
	public:
		static std::map<std::string, int> m_mapDicString2Index;
		static std::atomic_int s_nFunSize;
		static std::atomic_int s_nClassType;
		static std::string s_strClassName;
	};

	template<class T>
	std::atomic_int CScriptSuperPointer<T>::s_nFunSize = 0;

	template<class T>
	std::atomic_int CScriptSuperPointer<T>::s_nClassType = 0;

	template<class T>
	std::map<std::string, int> CScriptSuperPointer<T>::m_mapDicString2Index;

	template<class T>
	std::string CScriptSuperPointer<T>::s_strClassName;

	template<class T>
	inline int CScriptSuperPointer<T>::GetType()
	{
		return s_nClassType;
	}

	template<class T>
	inline int CScriptSuperPointer<T>::RunFun(unsigned int nIndex, CScriptRunState* pState)
	{
		Lock();
		int nResult = 0;
		if (m_pPointer)
		{
			nResult = m_pPointer->RunFun(nIndex, pState);
		}
		Unlock();
		return nResult;// (m_pPointer->*Fun)(pState);
	}
	template<class T>
	inline int CScriptSuperPointer<T>::GetFunIndex(std::string name)
	{
		if (CScriptSuperPointer<T>::m_mapDicString2Index.find(name) != CScriptSuperPointer<T>::m_mapDicString2Index.end())
		{
			return CScriptSuperPointer<T>::m_mapDicString2Index[name];
		}
		else
		{
			s_nFunSize++;
			CScriptSuperPointer<T>::m_mapDicString2Index[name] = s_nFunSize;
			return s_nFunSize;
		}
		return 0;
	}

	template<class T>
	inline void CScriptSuperPointer<T>::Lock()
	{
		m_MutexLock.lock();
	}

	template<class T>
	inline void CScriptSuperPointer<T>::Unlock()
	{
		m_MutexLock.unlock();
	}

	template<class T>
	inline void CScriptSuperPointer<T>::SetPointer(CScriptPointInterface* Point)
	{
		Lock();
		m_pPointer = dynamic_cast<T*>(Point);
		Unlock();
	}



	class CScriptSuperPointerMgr
	{
	public:
		CScriptSuperPointerMgr()
		{
			//Init();
			nClassTypeCount = 0;
		}
		~CScriptSuperPointerMgr()
		{
			Clear();
		}
		void Init();
		void Clear();
		void ClearPointer();

		void AddPoint2Release(__int64 id);
		void ReleaseAutoPoint();
	public:
		template<class T>
		bool RegisterClassType(std::string classname, T* p);

		template<class T>
		bool SetClassPoint(__int64 nID, T* pVal, bool autorelease);

		bool SetPointAutoRelease(__int64 nID, int autorelease);

		bool RemoveClassPoint(__int64 nID);

		int GetClassType(std::string funName);

		template<class T>
		int GetClassType(T* pVal);

		template<class T>
		bool SetClassFun(std::string funName, T* pPoint);

		//c++代码中，取一个指针进行操作 注意，pickup后必须return
		CScriptBasePointer* PickupPointer(__int64 id);
		void ReturnPointer(CScriptBasePointer* pPointer);
		//脚本代码中，用于标记是否被脚本变量引用
		bool ScriptUsePointer(__int64 id);
		bool ScriptReleasePointer(__int64 id);

		int GetClassFunIndex(int classindex, std::string funname);

		CBaseScriptClassMgr* GetClassMgr(int nType);
	private:
		int nClassTypeCount;
		std::map<std::string, int> m_mapString2ClassType;
		std::map<__int64, CScriptBasePointer*> m_mapPointer;

		std::map<__int64, CScriptBasePointer*> m_mapTypePointer;

		std::map<int, CBaseScriptClassMgr*> m_mapClassMgr;

		std::mutex m_MutexLock;
		std::mutex m_MutexTypeLock;

		std::set<__int64> m_autoReleaseIds;
	public:
		static CScriptSuperPointerMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptSuperPointerMgr s_Instance;
	};

	template<class T>
	inline bool CScriptSuperPointerMgr::RegisterClassType(std::string classname, T* p)
	{
		m_MutexTypeLock.lock();
		if (m_mapString2ClassType.find(classname) == m_mapString2ClassType.end())
		{
			nClassTypeCount++;
			m_mapString2ClassType[classname] = nClassTypeCount;
			CScriptSuperPointer<T>::s_nClassType = nClassTypeCount;

			CScriptSuperPointer<T>::s_strClassName = classname;

			m_mapClassMgr[nClassTypeCount] = GetScriptClassMgr<T>();//CScriptClassMgr<T>::GetInstance();
			
			m_MutexTypeLock.unlock();
			return true;
		}
		m_MutexTypeLock.unlock();
		return false;
	}

	template<class T>
	inline bool CScriptSuperPointerMgr::SetClassFun(std::string funName, T* pPoint)
	{
		m_MutexTypeLock.lock();
		CScriptSuperPointer<T>::s_nFunSize++;
		int nIndex = CScriptSuperPointer<T>::s_nFunSize;
		std::map<std::string, int>::iterator it = CScriptSuperPointer<T>::m_mapDicString2Index.find(funName);
		if (it != CScriptSuperPointer<T>::m_mapDicString2Index.end())
		{
			nIndex = it->second;
		}
		else
		{
			CScriptSuperPointer<T>::m_mapDicString2Index[funName] = nIndex;

		}

		if (m_mapTypePointer[CScriptSuperPointer<T>::s_nClassType] == nullptr)
		{
			m_mapTypePointer[CScriptSuperPointer<T>::s_nClassType] = new CScriptSuperPointer<T>;
		}
		m_MutexTypeLock.unlock();
		return true;
	}

	template<class T>
	inline int CScriptSuperPointerMgr::GetClassType(T* pVal)
	{
		return CScriptSuperPointer<T>::s_nClassType;
	}

	template<class T>
	inline bool CScriptSuperPointerMgr::SetClassPoint(__int64 nID, T* pVal, bool autorelease)
	{
		if (pVal == nullptr)
		{
			return false;
		}
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		std::map<__int64, CScriptBasePointer*>::iterator it = m_mapPointer.find(nID);
		if (it != m_mapPointer.end())
		{
			if (it->second)
				return false;
		}
		
		{
			CScriptSuperPointer<T>* pPoint = new CScriptSuperPointer<T>;
			if (pPoint == nullptr)
			{
				return false;
			}
			if (pPoint->GetType() <= 0)
			{
				delete pPoint;
				return false;
			}
			pPoint->SetAutoReleaseMode(autorelease);
			pPoint->SetID(nID);
			pPoint->SetPointer(pVal);
			m_mapPointer[nID] = pPoint;
		}
		return true;
	}

#define RegisterClassType(name,T) \
{ \
	T *pPoint = nullptr; \
	CScriptSuperPointerMgr::GetInstance()->RegisterClassType(name, pPoint); \
}

#define AddClassObject(id,point) CScriptSuperPointerMgr::GetInstance()->SetClassPoint(id,point,false);
#define RemoveClassObject(id) CScriptSuperPointerMgr::GetInstance()->RemoveClassPoint(id);
	typedef std::function<int(CScriptRunState*)> Script_ClassFun;


#define RegisterClassFun1(name,T) \
{ \
	T *pPoint = nullptr; \
	CScriptSuperPointerMgr::GetInstance()->SetClassFun(name, pPoint); \
}

#define RegisterClassFun(name, p, fun) \
	{ \
		struct CScript_##name##_ClassFunInfo :public CScriptBaseClassFunInfo \
		{ \
			std::function< int (CScriptRunState *)> m_fun; \
			int RunFun(CScriptRunState *pState) \
			{ \
				return m_fun(pState); \
			} \
			CScriptBaseClassFunInfo* Copy() \
			{ \
				CScript_##name##_ClassFunInfo* pInfo = new CScript_##name##_ClassFunInfo; \
				if (pInfo) \
				{ \
					pInfo->vParmeterInfo = this->vParmeterInfo; \
				} \
				return pInfo; \
			} \
			const char* GetFunName() \
			{ \
				return #name; \
			} \
		}; \
		CScript_##name##_ClassFunInfo *pInfo = new CScript_##name##_ClassFunInfo; \
		pInfo->m_fun = std::bind(fun,p,std::placeholders::_1); \
		int nClassType = CScriptSuperPointerMgr::GetInstance()->GetClassType(p); \
		int index = CScriptSuperPointerMgr::GetInstance()->GetClassFunIndex(nClassType,#name); \
		p->SetFun(index,pInfo); \
	}
}