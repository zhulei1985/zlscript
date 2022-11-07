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
#include <unordered_map>
#include "ScriptClassAttributes.h"
#include "ScriptClassFunion.h"
#include "ScriptClassInfo.h"
namespace zlscript
{
	class CBaseScriptVarMgr;

	template<class T>
	class CScriptAbstractClassMgr;
	template<class T>
	class CScriptClassVarMgr;
	template<class T>
	CBaseScriptVarMgr* GetScriptClassMgr();

	enum
	{
		ECALLBACK_ERROR,
		ECALLBACK_FINISH,
		ECALLBACK_WAITING,
		//ECALLBACK_CALLSCRIPTFUN,
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
	class CScriptCallState;



#define ATTR_BASE_INT(val,flag,index) CScriptIntAttribute val{#val,flag,index,this};
#define ATTR_BASE_INT64(val,flag,index) CScriptInt64Attribute val{#val,flag,index,this};
#define ATTR_BASE_FLOAT(val,flag,index) CScriptFloatAttribute val{#val,flag,index,this};
#define ATTR_BASE_DOUBLE(val,flag,index) CScriptDoubleAttribute val{#val,flag,index,this};
#define ATTR_BASE_STR(val,flag,index) CScriptStringAttribute val{#val,flag,index,this};
#define ATTR_BASE_INT64_ARRAY(val,flag,index) CScriptInt64ArrayAttribute val{#val,flag,index,this};
#define ATTR_BASE_INT64_MAP(val,flag,index) CScriptInt64toInt64MapAttribute val{#val,flag,index,this};
#define ATTR_BASE_CLASS_POINT(val,flag,index) CScriptClassPointAttribute val{#val,flag,index,this};
#define ATTR_BASE_VAR(val,flag,index) CScriptVarAttribute val{#val,flag,index,this};
#define ATTR_BASE_VAR_ARRAY(val,flag,index) CScriptVarArrayAttribute val{#val,flag,index,this};
#define ATTR_BASE_VAR_MAP(val,flag,index) CScriptVar2VarMapAttribute val{#val,flag,index,this};

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

#define CLASS_SCRIPT_FUN_TEST(classname,funname) \
	int funname##2Script(CScriptCallState* pState); 

#define CLASS_SCRIPT_FUN(classname,funname) \
	int funname##2Script(CScriptCallState* pState); \
	CBaseScriptClassFun funname##Fun{#funname,std::bind(&classname::funname##2Script,this,std::placeholders::_1),this, CBaseScriptClassFun::E_FLAG_NONE};

	class CBaseScriptClassInfo;
	class CScriptPointInterface : /*public CBaseVar,*/public IClassAttributeObserver , public IClassFunObserver
	{
	public:
		CScriptPointInterface();
		//{
		//	m_nID = 0;
		//}
		virtual ~CScriptPointInterface();
	public:
		void PickUp();
		void PickDown();
	private:
		std::mutex m_UseLock;
		std::mutex m_CountLock;
		std::atomic_int nUsedCount{ 0 };
	public:
		void Lock()
		{
			m_UseLock.lock();
		}
		void UnLock()
		{
			m_UseLock.unlock();
		}
		void SetClassInfo(CBaseScriptClassInfo* pInfo)
		{
			m_pClassInfo = pInfo;
		}
		CBaseScriptClassInfo* getClassInfo()
		{
			return m_pClassInfo;
		}

		unsigned int GetAttributeIndex(std::string name);
		CBaseScriptClassAttribute* GetAttribute(unsigned int index);

		virtual int RunFun(unsigned int id, CScriptCallState* pState);


		virtual const std::map<std::string, CBaseScriptClassAttribute*>& GetDBAttributes()
		{
			return m_mapDBAttributes;
		}
		virtual void ChangeScriptAttribute(CBaseScriptClassAttribute* pAttr, CBaseVar* old);
		virtual void RegisterScriptAttribute(CBaseScriptClassAttribute* pAttr);
		virtual void RemoveScriptAttribute(CBaseScriptClassAttribute* pAttr);

		virtual void RegisterScriptFun(CBaseScriptClassFun* pClassFun);
		unsigned int GetClassFunIndex(std::string name);
		CBaseScriptClassFun* GetClassFunInfo(unsigned int id);

		//virtual unsigned int GetSyncInfo_ClassPoint2Index(CScriptBasePointer* point) { return 0; }
		//virtual PointVarInfo GetSyncInfo_Index2ClassPoint(unsigned int index) { return PointVarInfo(); }

		bool AddVar2Bytes(std::vector<char>& vBuff, const CBaseVar* pVal, std::vector<CPointVar*>& vOutClassPoint);
		bool AddVar2Bytes(std::vector<char>& vBuff, const CPointVar* pVal, std::vector<CPointVar*>& vOutClassPoint);

		//返回值需要接收方控制释放与否
		CBaseVar* DecodeVar4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint);
		CPointVar* DecodePointVar4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint);

		//vOutClassPoint 当成员变量中包含了可用于脚本的类实例指针时，将指针放入vOutClassPoint单独进行传递
		virtual bool AddAllData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
		{
			return true;
		}
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint)
		{
			return true;
		}
	protected:

		CBaseScriptClassInfo* m_pClassInfo;


		std::map<std::string, CBaseScriptClassAttribute*> m_mapDBAttributes;

		std::unordered_map<unsigned int, CBaseScriptClassAttribute*> m_mapAllAttributes;

		std::vector<CBaseScriptClassFun*> m_vecScriptClassFun;

		std::mutex m_FunLock;
	public:
		void GetBaseClassInfo(CBaseScriptClassInfo& info);
	};

}