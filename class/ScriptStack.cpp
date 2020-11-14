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

#include "EScriptVariableType.h"
#include "ScriptStack.h"

namespace zlscript
{
	zlscript::CStringPool StackVarInfo::s_strPool;
	zlscript::CBinaryPool StackVarInfo::s_binPool;
	StackVarInfo::StackVarInfo()
	{
		cType = 0;
		cExtend = 0;
		Int64 = 0;
	}
	StackVarInfo::StackVarInfo(const StackVarInfo& cls)
	{
		cType = cls.cType;
		cExtend = cls.cExtend;

		switch (cls.cType)
		{
		case EScriptVal_Int:
			Int64 = cls.Int64;
			break;
		case EScriptVal_Double:
			Double = cls.Double;
			break;
		case EScriptVal_String:
			{
				Int64 = cls.Int64;
				s_strPool.UseString(Int64);
			}
			break;
		case EScriptVal_ClassPointIndex:
			Int64 = cls.Int64;
			break;
		case EScriptVal_Binary:
			{
				Int64 = cls.Int64;
				s_binPool.UseBinary(Int64);
			}
			break;
		}

	}
	StackVarInfo::~StackVarInfo()
	{
		Clear();
	}

	void StackVarInfo::Clear()
	{
		if (cType == EScriptVal_String)
		{
			s_strPool.ReleaseString(Int64);
		}
		else if (cType == EScriptVal_Binary)
		{
			s_binPool.ReleaseBinary(Int64);
		}
		cType = EScriptVal_None;
		Int64 = 0;
	}

	StackVarInfo& StackVarInfo::operator=(const StackVarInfo& cls)
	{
		Clear();
		cType = cls.cType;
		cExtend = cls.cExtend;

		switch (cls.cType)
		{
		case EScriptVal_Int:
			Int64 = cls.Int64;
			break;
		case EScriptVal_Double:
			Double = cls.Double;
			break;
		case EScriptVal_String:
			{
				Int64 = cls.Int64;
				s_strPool.UseString(Int64);
			}
			break;
		case EScriptVal_ClassPointIndex:
			Int64 = cls.Int64;
			break;
		case EScriptVal_Binary:
			{
				Int64 = cls.Int64;
				s_binPool.UseBinary(Int64);
			}
			break;
		}

		// TODO: 在此处插入 return 语句
		return *this;
	}

	CScriptStack::CScriptStack()
	{
		nIndex = 0;
	}
	CScriptStack::CScriptStack(const CScriptStack& info)
	{
		//先清空，再赋值
		while (this->size() > 0)
		{
			this->pop();
		}
		auto it = info.m_vData.begin();
		for (; it != info.m_vData.end(); it++)
		{
			StackVarInfo* info = *it;
			this->push(*info);
		}
		//return *this;
	}
	CScriptStack::~CScriptStack()
	{
		while (size() > 0)
		{
			pop();
		}

		auto it = m_Pool.begin();
		while (it != m_Pool.end())
		{
			StackVarInfo* pInfo = *it;
			if (pInfo)
			{
				delete pInfo;
			}
			it = m_Pool.erase(it);
		}
	}
	void CScriptStack::pop()
	{
		if (m_vData.size() > 0)
		{
			auto pInfo = m_vData.back();
			if (pInfo)
			{
				pInfo->Clear();
				m_Pool.push_back(pInfo);
			}
			m_vData.pop_back();
		}
	}
	void CScriptStack::push(StackVarInfo& val)
	{
		StackVarInfo* pInfo = nullptr;
		auto it = m_Pool.begin();
		if (it != m_Pool.end())
		{
			pInfo = *it;
			m_Pool.erase(it);
		}
		if (pInfo == nullptr)
		{
			pInfo = new StackVarInfo;
		}
		if (pInfo)
		{
			*pInfo = val;
			m_vData.push_back(pInfo);
		}
		//if (nIndex >= m_vData.size())
		//{
		//	StackVarInfo val;
		//	val.cType = EScriptVal_Int;
		//	val.Int64 = 0;
		//	m_vData.resize((nIndex / 8 + 1) * 8, val);
		//}
		//m_vData[nIndex++] = val;
	}
	StackVarInfo& CScriptStack::top()
	{
		auto pInfo = m_vData.back();
		if (pInfo)
		{
			return *pInfo;
		}

		return emptyinfo;
	}
	unsigned int CScriptStack::size()
	{
		return m_vData.size();
	}
	bool CScriptStack::empty()
	{
		if (m_vData.empty())
		{
			return true;
		}
		return false;
	}

	CScriptStack& CScriptStack::operator=(const CScriptStack& info)
	{
		//先清空，再赋值
		while (this->size() > 0)
		{
			this->pop();
		}
		auto it = info.m_vData.begin();
		for (; it != info.m_vData.end(); it++)
		{
			StackVarInfo* info = *it;
			this->push(*info);
		}
		return *this;
	}

	StackVarInfo* CScriptStack::GetVal(int index)
	{
		if (index >= 0 && index < m_vData.size())
		{
			return m_vData[index];
		}
		return nullptr;
	}
}
