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
#include <map>
#include <unordered_map>
#include <mutex>
#include <set>
#include <atomic>
#include <string>
#include "ScriptStack.h"
#include "ScriptSuperPointer.h"
#include "ScriptClassMgr.h"

namespace zlscript
{
	class CScriptHashMap : public CScriptPointInterface
	{
	public:
		CScriptHashMap();
		~CScriptHashMap();

		static void Init2Script();
	public:
		CLASS_SCRIPT_FUN(CScriptHashMap, GetVal);
		CLASS_SCRIPT_FUN(CScriptHashMap, SetVal);
		CLASS_SCRIPT_FUN(CScriptHashMap, Remove);
		CLASS_SCRIPT_FUN(CScriptHashMap, Clear);
		//int GetVal2Script(CScriptCallState* pState);
		//int SetVal2Script(CScriptCallState* pState);
		//int Remove2Script(CScriptCallState* pState);
		//int Clear2Script(CScriptCallState* pState);
	private:
		std::unordered_map<StackVarInfo, StackVarInfo, hash_SV> m_mapData;
	};
	class CScriptArray : public CScriptPointInterface
	{
	public:
		CScriptArray();
		~CScriptArray();
		static void Init2Script();
	public:
		CLASS_SCRIPT_FUN(CScriptArray, Add);
		CLASS_SCRIPT_FUN(CScriptArray, Get);
		CLASS_SCRIPT_FUN(CScriptArray, GetSize);
		CLASS_SCRIPT_FUN(CScriptArray, Set);
		CLASS_SCRIPT_FUN(CScriptArray, Remove);
		CLASS_SCRIPT_FUN(CScriptArray, Clear);

		//int Add2Script(CScriptCallState* pState);
		//int Get2Script(CScriptCallState* pState);
		//int GetSize2Script(CScriptCallState* pState);
		//int Set2Script(CScriptCallState* pState);
		//int Remove2Script(CScriptCallState* pState);

		//int Clear2Script(CScriptCallState* pState);
	public:
		std::vector<StackVarInfo>& GetVars()
		{
			return m_vecVars;
		}

		virtual bool AddAllData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint);
	private:
		//std::map<std::string, unsigned int> m_mapDicName2Index;
		std::vector<StackVarInfo> m_vecVars;
	};
	class CScriptSubData
	{
	public:
		CScriptSubData();
		~CScriptSubData();

	public:
		StackVarInfo m_var;
		std::map<std::string, CScriptSubData*> m_mapChild;
	};
	class CScriptData : public CScriptPointInterface
	{
	public:
		CScriptData();
		~CScriptData();
		static void Init2Script();

		CLASS_SCRIPT_FUN(CScriptData, GetVal);
		CLASS_SCRIPT_FUN(CScriptData, SetVal);
		CLASS_SCRIPT_FUN(CScriptData, SaveFile);
		CLASS_SCRIPT_FUN(CScriptData, LoadFile);

		std::map<std::string, CScriptSubData*> m_mapChild;

		std::mutex m_Lock;

		std::string strFlag;
		std::atomic_int m_nUseCount;
	};
	class CScriptDataMgr
	{
	public:
		CScriptDataMgr();
		~CScriptDataMgr();

		CScriptData* GetData(const char* pFlag);
		void ReleaseData(CScriptData* pData);

		void Clear();
	public:
		std::map<std::string, CScriptData*> m_mapScriptData;

		std::mutex m_Lock;

	public:
		CScriptArray* NewArray();
		void ReleaseArray(CScriptArray*);
	private:
		std::set<CScriptArray*> m_setArray;

	public:
		static CScriptDataMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptDataMgr s_Instance;
	};
}