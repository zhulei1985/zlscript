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
#include <unordered_map>

#include "ScriptPointInterface.h"
#define SCRIPT_NO_USED_AUTO_RELEASE 1
namespace zlscript
{
	struct stScriptClassInfo
	{
		stScriptClassInfo()
		{
			nFunSize = 0;
			nClassType = 0;
			nDB_Id_Count = 0;
		}
		std::map<std::string, int> mapDicString2Index;
		std::atomic_int nFunSize;
		std::atomic_int nClassType;
		std::string strClassName;
		std::atomic_int64_t nDB_Id_Count;
	};
	class CScriptCallState;
	class CScriptBasePointer
	{
	public:
		CScriptBasePointer(){
			m_ID = 0;
			m_nUseCount = 0;
			//m_nScriptUseCount = 0;
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

		virtual CBaseScriptClassAttribute* GetAttribute(unsigned int index) = 0;

		virtual int RunFun(unsigned int nIndex, CScriptCallState*) = 0;
		virtual int GetFunIndex(std::string name) = 0;
		virtual int GetAttributeIndex(std::string name) = 0;
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
		//std::atomic_int m_nScriptUseCount;
	};

	template<class T>
	class CScriptSuperPointer : public CScriptBasePointer
	{
	public:
		virtual const char* ClassName()
		{
			return s_Info.strClassName.c_str();
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
		CBaseScriptClassAttribute* GetAttribute(unsigned int index);
		int RunFun(unsigned int nIndex, CScriptCallState*);
		int GetFunIndex(std::string name);
		int GetAttributeIndex(std::string name);

		virtual void Lock();
		virtual void Unlock();
	public:
		void SetPointer(CScriptPointInterface* Point);
	private:
		T* m_pPointer;

		std::recursive_mutex m_MutexLock;
	public:
		static stScriptClassInfo s_Info;
	};

	template<class T>
	stScriptClassInfo CScriptSuperPointer<T>::s_Info;

	template<class T>
	inline int CScriptSuperPointer<T>::GetType()
	{
		return s_Info.nClassType;
	}

	template<class T>
	inline CBaseScriptClassAttribute* CScriptSuperPointer<T>::GetAttribute(unsigned int index)
	{
		if (m_pPointer)
		{
			return m_pPointer->GetAttribute(index);
		}
		return nullptr;
	}

	template<class T>
	inline int CScriptSuperPointer<T>::RunFun(unsigned int nIndex, CScriptCallState* pState)
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
		//auto it = s_Info.mapDicString2Index.find(name);
		//if (it != CScriptSuperPointer<T>::s_Info.mapDicString2Index.cend())
		//{
		//	return it->second;
		//}
		//else
		//{
		//	int index = s_Info.nFunSize++;
		//	CScriptSuperPointer<T>::s_Info.mapDicString2Index[name] = index;
		//	return index;
		//}
		if (GetPoint())
		{
			return GetPoint()->GetClassFunIndex(name);
		}
		return -1;
	}

	template<class T>
	inline int CScriptSuperPointer<T>::GetAttributeIndex(std::string name)
	{
		if (GetPoint())
		{
			return GetPoint()->GetAttributeIndex(name);
		}
		return -1;
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
		bool SetClassPoint(__int64 nID, T* pVal, int autorelease);

		bool SetPointAutoRelease(__int64 nID, int autorelease);

		bool RemoveClassPoint(__int64 nID);

		int GetClassType(std::string funName);
		void SetClassDBIdCount(std::string funName, __int64 val);

		template<class T>
		static int GetClassType(T* pVal);

		template<class T>
		static std::string GetClassName(T* pVal);

		//template<class T>
		//bool SetClassFun(std::string funName, T* pPoint);

		//c++代码中，取一个指针进行操作 注意，pickup后必须return
		CScriptBasePointer* PickupPointer(__int64 id, std::string className);
		CScriptBasePointer* PickupPointer(__int64 id);
		void PickupPointer(CScriptBasePointer *pPoint);
		void ReturnPointer(CScriptBasePointer* pPointer);
		//脚本代码中，用于标记是否被脚本变量引用
		//bool ScriptUsePointer(__int64 id);
		//bool ScriptReleasePointer(__int64 id);

		int GetClassFunIndex(int classindex, std::string funname);
		int GetClassParamIndex(int classindex, std::string paramname);

		CBaseScriptClassMgr* GetClassMgr(int nType);
		CBaseScriptClassMgr* GetClassMgr(std::string strType);
	private:
		int nClassTypeCount;
		std::map<std::string, int> m_mapString2ClassType;
		std::unordered_map<__int64, CScriptBasePointer*> m_mapPointer;

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
		std::lock_guard<std::mutex> Lock(m_MutexTypeLock);
		if (m_mapString2ClassType.find(classname) == m_mapString2ClassType.end())
		{
			nClassTypeCount++;
			m_mapString2ClassType[classname] = nClassTypeCount;
			CScriptSuperPointer<T>::s_Info.nClassType = nClassTypeCount;

			CScriptSuperPointer<T>::s_Info.strClassName = classname;

			m_mapClassMgr[nClassTypeCount] = GetScriptClassMgr<T>();//CScriptClassMgr<T>::GetInstance();
			
			if (p)
			{
				auto it = m_mapPointer.find(p->GetScriptPointIndex());
				if (it != m_mapPointer.end())
				{
					m_mapTypePointer[CScriptSuperPointer<T>::s_Info.nClassType] = it->second;
					m_mapPointer.erase(it);
				}
				else
				{
					CScriptSuperPointer<T>* pPoint = new CScriptSuperPointer<T>;
					if (pPoint == nullptr)
					{
						return false;
					}
					p->SetClassInfo(&CScriptSuperPointer<T>::s_Info);
					pPoint->SetAutoReleaseMode(false);
					pPoint->SetID(p->GetScriptPointIndex());
					pPoint->SetPointer(p);
					m_mapTypePointer[CScriptSuperPointer<T>::s_Info.nClassType] = pPoint;
				}
			}

			return true;
		}
		delete p;
		return false;
	}

	//template<class T>
	//inline bool CScriptSuperPointerMgr::SetClassFun(std::string funName, T* pPoint)
	//{
	//	m_MutexTypeLock.lock();
	//	int nIndex = -1;
	//	auto it = CScriptSuperPointer<T>::s_Info.mapDicString2Index.find(funName);
	//	if (it != CScriptSuperPointer<T>::s_Info.mapDicString2Index.end())
	//	{
	//		nIndex = it->second;
	//	}
	//	else
	//	{
	//		CScriptSuperPointer<T>::s_Info.mapDicString2Index[funName] = CScriptSuperPointer<T>::s_Info.nFunSize;
	//		CScriptSuperPointer<T>::s_Info.nFunSize++;
	//	}


	//	m_MutexTypeLock.unlock();
	//	return true;
	//}

	template<class T>
	inline int CScriptSuperPointerMgr::GetClassType(T* pVal)
	{
		return CScriptSuperPointer<T>::s_Info.nClassType;
	}

	template<class T>
	inline std::string CScriptSuperPointerMgr::GetClassName(T* pVal)
	{
		return CScriptSuperPointer<T>::s_Info.strClassName;
	}

	template<class T>
	inline bool CScriptSuperPointerMgr::SetClassPoint(__int64 nID, T* pVal, int autorelease)
	{
		if (pVal == nullptr)
		{
			return false;
		}
		std::lock_guard<std::mutex> Lock(m_MutexLock);
		auto it = m_mapPointer.find(nID);
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
			pVal->SetClassInfo(&CScriptSuperPointer<T>::s_Info);
			pPoint->SetAutoReleaseMode(autorelease);
			pPoint->SetID(nID);
			pPoint->SetPointer(pVal);
			m_mapPointer[nID] = pPoint;
		}
		return true;
	}

#define RegisterClassType(name,T) \
{ \
	T *pPoint = new T(); \
	CScriptSuperPointerMgr::GetInstance()->RegisterClassType(name, pPoint); \
}

#define AddClassObject(id,point) CScriptSuperPointerMgr::GetInstance()->SetClassPoint(id,point,0);
#define RemoveClassObject(id) CScriptSuperPointerMgr::GetInstance()->RemoveClassPoint(id);
	typedef std::function<int(CScriptCallState*)> Script_ClassFun;

//
//#define RegisterClassFun1(name,T) \
//{ \
//	T *pPoint = nullptr; \
//	CScriptSuperPointerMgr::GetInstance()->SetClassFun(name, pPoint); \
//}
////代替RegisterClassFun1
//#define RegisterClassFunName(name,T) \
//{ \
//	T *pPoint = nullptr; \
//	CScriptSuperPointerMgr::GetInstance()->SetClassFun(#name, pPoint); \
//}
//
//#define RegisterClassFun(name, p, fun) \
//	{ \
//		struct CScript_##name##_ClassFunInfo :public CScriptBaseClassFunInfo \
//		{ \
//			std::function< int (CScriptCallState *)> m_fun; \
//			int RunFun(CScriptCallState *pState) \
//			{ \
//				return m_fun(pState); \
//			} \
//			CScriptBaseClassFunInfo* Copy() \
//			{ \
//				CScript_##name##_ClassFunInfo* pInfo = new CScript_##name##_ClassFunInfo; \
//				if (pInfo) \
//				{ \
//					pInfo->vParmeterInfo = this->vParmeterInfo; \
//				} \
//				return pInfo; \
//			} \
//			const char* GetFunName() \
//			{ \
//				return #name; \
//			} \
//		}; \
//		CScript_##name##_ClassFunInfo *pInfo = new CScript_##name##_ClassFunInfo; \
//		pInfo->m_fun = std::bind(fun,p,std::placeholders::_1); \
//		int nClassType = CScriptSuperPointerMgr::GetClassType(p); \
//		int index = CScriptSuperPointerMgr::GetInstance()->GetClassFunIndex(nClassType,#name); \
//		p->SetFun(index,pInfo); \
//	}
}