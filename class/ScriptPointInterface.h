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
#include <set>
#include "ScriptClassAttributes.h"

namespace zlscript
{
	class CBaseScriptClassMgr;
	struct stScriptClassInfo;

	template<class T>
	class CScriptAbstractClassMgr;
	template<class T>
	class CScriptClassMgr;
	template<class T>
	CBaseScriptClassMgr* GetScriptClassMgr();

	enum
	{
		ECALLBACK_ERROR,
		ECALLBACK_FINISH,
		ECALLBACK_WAITING,
		ECALLBACK_CALLSCRIPTFUN,
		ECALLBACK_NEXTCONTINUE,
	};
	template<typename TFuncAddr, typename TFunc>
	inline void GetMemberFuncAddr(TFuncAddr& addr, TFunc func)
	{
		union
		{
			TFunc _func;
			TFuncAddr _addr;
		}ut;
		ut._func = func;
		addr = ut._addr;
	}
	template<typename TFuncAddr, typename TFunc>
	inline void GetFuncForAddr(TFuncAddr addr, TFunc& func)
	{
		union
		{
			TFunc _func;
			TFuncAddr _addr;
		}ut;
		ut._addr = addr;
		func = ut._func;
	}
	class CScriptRunState;
	typedef int (*ScriptClassFunHandler)(CScriptRunState*);
	struct CScriptBaseClassFunInfo
	{
		struct tagParameterInfo
		{
			int type;//1 数值 2 浮点 3 字符串 4 类指针
			int classtype;//如果是类指针，类类型值
		};
		//virtual void init() = 0;
		std::vector<tagParameterInfo> vParmeterInfo;
		virtual int RunFun(CScriptRunState* pState) = 0;
		virtual CScriptBaseClassFunInfo* Copy() = 0;
		virtual const char* GetFunName() = 0;
	};

#define ATTR_BASE_INT(val,flag,index) CScriptIntAttribute val{#val,flag,index,this};
#define ATTR_BASE_INT64(val,flag,index) CScriptInt64Attribute val{#val,flag,index,this};
#define ATTR_BASE_FLOAT(val,flag,index) CScriptFloatAttribute val{#val,flag,index,this};
#define ATTR_BASE_DOUBLE(val,flag,index) CScriptDoubleAttribute val{#val,flag,index,this};
#define ATTR_BASE_STR(val,flag,index) CScriptStringAttribute val{#val,flag,index,this};
#define ATTR_BASE_INT64_ARRAY(val,flag,index) CScriptInt64ArrayAttribute val{#val,flag,index,this};
#define ATTR_BASE_INT64_MAP(val,flag,index) CScriptInt64toInt64MapAttribute val{#val,flag,index,this};
#define ATTR_BASE_CLASS_POINT(val,flag,index) CScriptClassPointAttribute val{#val,flag,index,this};
#define ATTR_BASE_CLASS_POINT_ARRAY(val,flag,index) CScriptClassPointArrayAttribute val{#val,flag,index,this};
#define ATTR_BASE_CLASS_POINT_MAP(val,flag,index) CScriptInt64toClassPointMapAttribute val{#val,flag,index,this};

#define ATTR_INT(val,index) ATTR_BASE_INT(val,CBaseScriptClassAttribute::E_FLAG_NONE,index);
#define ATTR_INT64(val,index) ATTR_BASE_INT64(val,CBaseScriptClassAttribute::E_FLAG_NONE,index);
#define ATTR_FLOAT(val,index) ATTR_BASE_FLOAT(val,CBaseScriptClassAttribute::E_FLAG_NONE,index);
#define ATTR_DOUBLE(val,index) ATTR_BASE_DOUBLE(val,CBaseScriptClassAttribute::E_FLAG_NONE,index);
#define ATTR_STR(val,index) ATTR_BASE_STR(val,CBaseScriptClassAttribute::E_FLAG_NONE,index);
#define ATTR_INT64_ARRAY(val,index) ATTR_BASE_INT64_ARRAY(val,CBaseScriptClassAttribute::E_FLAG_NONE,index);
#define ATTR_INT64_MAP(val,index) ATTR_BASE_INT64_MAP(val,CBaseScriptClassAttribute::E_FLAG_NONE,index);

#define ATTR_DB_INT(val,index) ATTR_BASE_INT(val,CBaseScriptClassAttribute::E_FLAG_DB,index);
#define ATTR_DB_INT64(val,index) ATTR_BASE_INT64(val,CBaseScriptClassAttribute::E_FLAG_DB,index);
#define ATTR_DB_FLOAT(val,index) ATTR_BASE_FLOAT(val,CBaseScriptClassAttribute::E_FLAG_DB,index);
#define ATTR_DB_DOUBLE(val,index) ATTR_BASE_DOUBLE(val,CBaseScriptClassAttribute::E_FLAG_DB,index);
#define ATTR_DB_STR(val,index) ATTR_BASE_STR(val,CBaseScriptClassAttribute::E_FLAG_DB,index);
#define ATTR_DB_INT64_ARRAY(val,index) ATTR_BASE_INT64_ARRAY(val,CBaseScriptClassAttribute::E_FLAG_DB,index);
#define ATTR_DB_INT64_MAP(val,index) ATTR_BASE_INT64_MAP(val,CBaseScriptClassAttribute::E_FLAG_DB,index);

	class CScriptPointInterface : public IClassAttributeObserver
	{
	public:
		CScriptPointInterface();
		//{
		//	m_nID = 0;
		//}
		virtual ~CScriptPointInterface();
		virtual void InitScriptPointIndex();
		bool IsInitScriptPointIndex();
		__int64 GetScriptPointIndex();
		void ClearScriptPointIndex();
		virtual bool CanRelease()
		{
			return true;
		}
		void SetClassInfo(stScriptClassInfo* pInfo)
		{
			m_pClassInfo = pInfo;
		}
		stScriptClassInfo* getClassInfo()
		{
			return m_pClassInfo;
		}

		void SetFun(int id, CScriptBaseClassFunInfo* pInfo);
		virtual int RunFun(int id, CScriptRunState* pState);
		//virtual int CallFun(const char*pFunName, CScriptStack &parms);

		CScriptPointInterface(const CScriptPointInterface& val);
		virtual CScriptPointInterface& operator=(const CScriptPointInterface& val);

		virtual const std::map<std::string, CBaseScriptClassAttribute*>& GetDBAttributes()
		{
			return m_mapDBAttributes;
		}
		virtual void ChangeScriptAttribute(CBaseScriptClassAttribute* pAttr, StackVarInfo& old);
		virtual void RegisterScriptAttribute(CBaseScriptClassAttribute* pAttr);
		virtual void RemoveScriptAttribute(CBaseScriptClassAttribute* pAttr);

		virtual unsigned int GetSyncInfo_ClassPoint2Index(CScriptBasePointer* point) { return 0; }
		virtual PointVarInfo GetSyncInfo_Index2ClassPoint(unsigned int index) { return PointVarInfo(); }

		CScriptBaseClassFunInfo* GetClassFunInfo(int id);
	protected:
		//用于所有脚本可用的类实例索引，作用范围是本地
		__int64 m_nScriptPointIndex;
		static __int64 s_nScriptPointIndexCount;

		stScriptClassInfo* m_pClassInfo;


		std::map<std::string, CBaseScriptClassAttribute*> m_mapDBAttributes;

		std::vector<CScriptBaseClassFunInfo*> m_vecScriptClassFun;

		std::mutex m_FunLock;
		//std::shared_ptr<std::mutex> m_FunLock;

	};

}