#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include "ScriptVarInfo.h"
#include "ScriptClassMgr.h"
#include "ScriptClassInfo.h"
namespace zlscript
{
	class CScriptPointInterface;
	class CScriptVarTypeMgr
	{
	public:
		CScriptVarTypeMgr();
		~CScriptVarTypeMgr();
	public:
		static CScriptVarTypeMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptVarTypeMgr s_Instance;
	public:
		template<class T>
		bool RegisterVarType(std::string name, T* p);

		template<class T>
		bool RegisterClassType(std::string classname, T* p);

		unsigned int GetVarType(std::string name);

		CBaseVar* GetVar(unsigned int type);
		void ReleaseVar(CBaseVar* pVar);

		CScriptPointInterface* NewObject(unsigned int type);
		void ReleaseObject(CScriptPointInterface* pVar);

		CBaseScriptClassInfo *GetTypeInfo(unsigned int type);
	private:

		std::mutex m_MutexTypeLock;
		std::unordered_map<std::string, unsigned int> mapDicString2Type;
		std::vector<CBaseScriptVarMgr*> m_vVarTypeMgr;

		std::vector<CBaseScriptClassInfo*> m_vVarTypeInfo;
		////只有初始化的时候才会写，理论上不需要线程锁
		//std::vector<CScriptClassInfo> m_vClassInfo;
	};


	template<class T>
	inline bool CScriptVarTypeMgr::RegisterVarType(std::string name, T* p)
	{
		std::lock_guard<std::mutex> Lock(m_MutexTypeLock);
		if (mapDicString2Type.find(name) == mapDicString2Type.end())
		{
			auto pMgr = new CScriptVarMgr<T>();
			int nType = m_vVarTypeMgr.size();
			mapDicString2Type[name] = nType;
			pMgr->SetType(nType);
			m_vVarTypeMgr.push_back(pMgr);

			CScriptClassInfo<T> &info = CScriptClassInfo<T>::GetInstance();
			info.strClassName = name;
			info.nClassType = nType;
			m_vVarTypeInfo.push_back(&info);
			//m_vClassInfo.push_back(info);
			return true;
		}
		return false;
	}

	template<class T>
	inline bool CScriptVarTypeMgr::RegisterClassType(std::string classname, T* p)
	{
		std::lock_guard<std::mutex> Lock(m_MutexTypeLock);
		if (mapDicString2Type.find(classname) == mapDicString2Type.end())
		{
			auto pMgr = new CScriptClassVarMgr<T>();
			int nType = m_vVarTypeMgr.size();
			pMgr->SetType(nType);
			mapDicString2Type[classname] = nType;
			m_vVarTypeMgr.push_back(pMgr);

			CBaseScriptClassInfo *pinfo = &CScriptClassInfo<T>::GetInstance();
			pinfo->strClassName = classname;
			pinfo->nClassType = nType;
			p->GetBaseClassInfo(*pinfo);
			m_vVarTypeInfo.push_back(pinfo);
			//m_vClassInfo.push_back(info);
			return true;
		}
		return false;
	}

//注册内部变量类型
#define RegisterVarType(name,T) \
{ \
	T *pPoint = new T(); \
	if (pPoint) {\
	CScriptVarTypeMgr::GetInstance()->RegisterVarType(name, pPoint); \
	delete pPoint;}\
}

#define RegisterClassType(name,T) \
{ \
	T *pPoint = new T(); \
	if (pPoint) {\
	CScriptVarTypeMgr::GetInstance()->RegisterClassType(name, pPoint); \
	delete pPoint;}\
}

#define SCRIPTVAR_RELEASE(pVar) {\
		if (pVar)\
		{\
			CScriptVarTypeMgr::GetInstance()->ReleaseVar(pVar);\
			pVar = nullptr;\
		}\
	}\
	
#define SCRIPTVAR_COPY_VAR(pVar1,pVar2) {\
		if (pVar2)\
		{\
			if (pVar1 && pVar1->GetType() == pVar2->GetType())\
			{\
				pVar1->Set(pVar2);\
			}\
			else \
			{\
				SCRIPTVAR_RELEASE(pVar1);\
				pVar1 = CScriptVarTypeMgr::GetInstance()->GetVar(pVar2->GetType());\
				pVar1->Set(pVar2);\
			}\
		}\
	}
#define SCRIPTVAR_MOVE_VAR(pVar1,pVar2) {\
		SCRIPTVAR_RELEASE(pVar1); \
		if (pVar2)\
		{\
			pVar1 = pVar2;\
			pVar2 = nullptr;\
		}\
	}

}