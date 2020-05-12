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
#include "scriptcommon.h"
#include <list>
#include <stack>
#include <string>
#include "zStringBuffer.h"
#include "EScriptVariableType.h"
#include "ScriptSuperPointer.h"

#pragma warning(disable : 4996) 

namespace zlscript
{
	struct StackVarInfo
	{
		unsigned char cType;
		unsigned char cExtend;
		union
		{
			double Double;
			__int64 Int64;
			//const char *pStr;
		};

		StackVarInfo();
		StackVarInfo(const StackVarInfo& cls);
		~StackVarInfo();

		void Clear();

		//赋值重载
		StackVarInfo& operator=(const StackVarInfo& cls);

		static zlscript::CStringPool s_strPool;
	};

	class CScriptStack
	{
	public:
		CScriptStack();
		CScriptStack(const CScriptStack& info);
		~CScriptStack();
	public:
		void pop();
		void push(StackVarInfo& val);
		StackVarInfo& top();
		unsigned int size();
		bool empty();

		CScriptStack& operator =(const CScriptStack& info);
		StackVarInfo* GetVal(int index);
	private:
		std::vector<StackVarInfo*> m_vData;

		std::list<StackVarInfo*> m_Pool;
		unsigned int nIndex;

		StackVarInfo emptyinfo;
	};
	inline __int64 GetInt_StackVar(StackVarInfo* pVar)
	{
		if (pVar == nullptr)
		{
			return 0;
		}
		__int64 nReturn = 0;
		switch (pVar->cType)
		{
		case EScriptVal_Int:
			nReturn = pVar->Int64;
			break;
		case EScriptVal_Double:
			nReturn = (__int64)(pVar->Double + 0.5f);
			break;
		case EScriptVal_String:
		{
			const char* pStr = StackVarInfo::s_strPool.GetString(pVar->Int64);
			if (pStr)
			{
				nReturn = atoi(pStr);

			}
			else
			{
				nReturn = 0;
			}
		}
		break;
		}
		return nReturn;
	}
	inline double GetFloat_StackVar(StackVarInfo* pVar)
	{
		if (pVar == nullptr)
		{
			return 0;
		}
		double qReturn = 0;
		switch (pVar->cType)
		{
		case EScriptVal_Int:
			qReturn = (double)pVar->Int64;
			break;
		case EScriptVal_Double:
			qReturn = pVar->Double;
			break;
		case EScriptVal_String:
			const char* pStr = StackVarInfo::s_strPool.GetString(pVar->Int64);
			if (pStr)
			{
				qReturn = atof(pStr);

			}
			else
			{
				qReturn = 0;
			}
			break;
		}
		return qReturn;
	}
	inline std::string GetString_StackVar(StackVarInfo* pVar)
	{
		char strbuff[2048];
		memset(strbuff, 0, sizeof(strbuff));
		if (pVar == nullptr)
		{
			return strbuff;
		}

		switch (pVar->cType)
		{
		case EScriptVal_Int:
			//ltoa(var.Int64,strbuff,10);
			sprintf(strbuff, "%lld", pVar->Int64);
			break;
		case EScriptVal_Double:
			sprintf(strbuff, "%f", pVar->Double);
			//gcvt(var.Double,8,strbuff);
			break;
		case EScriptVal_String:
		{
			const char* pStr = StackVarInfo::s_strPool.GetString(pVar->Int64);
			if (pStr)
				strcpy(strbuff, pStr);
		}
		break;
		}
		return strbuff;
	}
	inline __int64 GetPointIndex_StackVar(StackVarInfo* pVar)
	{
		if (pVar == nullptr)
		{
			return 0;
		}
		__int64 nReturn = 0;
		switch (pVar->cType)
		{
		case EScriptVal_ClassPointIndex:
		{
			nReturn = pVar->Int64;
		}
		break;
		}
		return nReturn;
	}


	inline __int64 ScriptStack_GetInt(CScriptStack& Stack)
	{
		if (Stack.empty())
		{
			return 0;
		}
		StackVarInfo& var = Stack.top();
		__int64 nReturn = 0;
		switch (var.cType)
		{
		case EScriptVal_Int:
			nReturn = var.Int64;
			break;
		case EScriptVal_Double:
			nReturn = (__int64)(var.Double + 0.5f);
			break;
		case EScriptVal_String:
		{
			const char* pStr = StackVarInfo::s_strPool.GetString(var.Int64);
			if (pStr)
			{
				nReturn = atoi(pStr);

			}
			else
			{
				nReturn = 0;
			}
		}

		break;
		}

		Stack.pop();
		return nReturn;
	}

	inline double ScriptStack_GetFloat(CScriptStack& Stack)
	{
		if (Stack.empty())
		{
			return 0;
		}
		StackVarInfo& var = Stack.top();
		double qReturn = 0;
		switch (var.cType)
		{
		case EScriptVal_Int:
			qReturn = (double)var.Int64;
			break;
		case EScriptVal_Double:
			qReturn = var.Double;
			break;
		case EScriptVal_String:
			const char* pStr = StackVarInfo::s_strPool.GetString(var.Int64);
			if (pStr)
			{
				qReturn = atof(pStr);

			}
			else
			{
				qReturn = 0;
			}
			break;
		}

		Stack.pop();
		return qReturn;
	}


	inline std::string ScriptStack_GetString(CScriptStack& Stack)
	{
		char strbuff[2048];
		memset(strbuff, 0, sizeof(strbuff));
		if (Stack.empty())
		{
			return strbuff;
		}
		StackVarInfo var = Stack.top();
		switch (var.cType)
		{
		case EScriptVal_Int:
			//ltoa(var.Int64,strbuff,10);
			sprintf(strbuff, "%lld", var.Int64);
			break;
		case EScriptVal_Double:
			sprintf(strbuff, "%f", var.Double);
			//gcvt(var.Double,8,strbuff);
			break;
		case EScriptVal_String:
		{
			const char* pStr = StackVarInfo::s_strPool.GetString(var.Int64);
			if (pStr)
				strcpy(strbuff, pStr);
		}


		break;
		}

		Stack.pop();
		return strbuff;
	}

	inline __int64 ScriptStack_GetClassPointIndex(CScriptStack& Stack)
	{
		if (Stack.empty())
		{
			return 0;
		}
		StackVarInfo& var = Stack.top();
		__int64 nReturn = 0;
		switch (var.cType)
		{
		case EScriptVal_ClassPointIndex:
			nReturn = var.Int64;
			break;
		}

		Stack.pop();
		return nReturn;
	}

	inline StackVarInfo ScriptStack_GetVar(CScriptStack& Stack)
	{
		if (Stack.empty())
		{
			StackVarInfo var;
			var.cType = EScriptVal_None;
			var.Int64 = 0;
			return var;
		}
		StackVarInfo var = Stack.top();
		Stack.pop();
		return var;
	}

	inline void ScriptVector_PushVar(CScriptStack& Stack, __int64 val)
	{
		StackVarInfo var;
		var.cType = EScriptVal_Int;
		var.Int64 = val;
		Stack.push(var);;
	}

	//inline void ScriptVector_PushVar(std::vector<StackVarInfo>& vec, int val)
	//{
	//	ScriptVector_PushVar(vec, (__int64)val);
	//}
	inline void ScriptVector_PushVar(CScriptStack& Stack, double val)
	{
		StackVarInfo var;
		var.cType = EScriptVal_Double;
		var.Double = val;
		Stack.push(var);
	}

	inline void ScriptVector_PushVar(CScriptStack& Stack, const char* pVal)
	{
		StackVarInfo var;
		var.cType = EScriptVal_String;
		var.Int64 = StackVarInfo::s_strPool.NewString(pVal);
		Stack.push(var);
	}

	inline void ScriptVector_PushVar(CScriptStack& Stack, CScriptPointInterface* pVal)
	{
		StackVarInfo var;
		var.cType = EScriptVal_ClassPointIndex;
		if (pVal)
			var.Int64 = pVal->GetScriptPointIndex();
		else
			var.Int64 = 0;
		Stack.push(var);
	}

	inline void ScriptVector_PushClassPointIndex(CScriptStack& Stack, __int64 val)
	{
		StackVarInfo var;
		var.cType = EScriptVal_ClassPointIndex;
		var.Int64 = val;
		Stack.push(var);
	}

	inline void ScriptVector_PushVar(CScriptStack& Stack, StackVarInfo* val)
	{
		if (val)
			Stack.push(*val);
	}

	inline void ScriptVector_PushEmptyVar(CScriptStack& Stack)
	{
		StackVarInfo var;
		var.cType = EScriptVal_None;
		var.Int64 = 0;
		Stack.push(var);
	}
}