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

	CScriptStack::CScriptStack(unsigned int Size)
	{
		m_vData.resize(Size);
		nIndex = 0;
	}
	CScriptStack::CScriptStack(const CScriptStack& info)
	{
		//先清空，再赋值
		for (unsigned int i = 0; i < this->nIndex; i++)
		{
			this->m_vData[i].Clear();
		}
		this->nIndex = 0;
		if (info.m_vData.size() == 0)
		{
			//错误
			return;
		}
		this->m_vData.resize(info.m_vData.size());
		this->nIndex = info.nIndex;

		for (unsigned int i = 0; i < info.nIndex; i++)
		{
			this->m_vData[i] = info.m_vData[i];
		}
		//return *this;
	}
	CScriptStack::~CScriptStack()
	{
		nIndex = 0;
		m_vData.clear();
	}
	bool CScriptStack::pop()
	{
		if (nIndex > 0)
		{
			nIndex--;
			m_vData[nIndex].Clear();
			return true;
		}
		return false;
	}
	bool CScriptStack::push(StackVarInfo& val)
	{
		if (nIndex < m_vData.size())
		{
			m_vData[nIndex] = val;
			nIndex++;
			return true;
		}
		return false;
	}
	StackVarInfo& CScriptStack::top()
	{
		if (nIndex > 0)
		{
			return m_vData[nIndex - 1];
		}

		return emptyinfo;
	}
	unsigned int CScriptStack::size()
	{
		return nIndex;
	}
	bool CScriptStack::empty()
	{
		if (nIndex == 0)
		{
			return true;
		}
		return false;
	}

	CScriptStack& CScriptStack::operator=(const CScriptStack& info)
	{
		//先清空，再赋值
		for (unsigned int i = 0; i < this->nIndex; i++)
		{
			this->m_vData[i].Clear();
		}
		this->nIndex = 0;
		if (info.m_vData.size() == 0)
		{
			//错误
			return *this;
		}
		this->m_vData.resize(info.m_vData.size());
		this->nIndex = info.nIndex;

		for (unsigned int i = 0; i < info.nIndex; i++)
		{
			this->m_vData[i] = info.m_vData[i];
		}
		return *this;
	}

	StackVarInfo* CScriptStack::GetVal(int index)
	{
		if (index >= 0 && index < m_vData.size())
		{
			return &m_vData[index];
		}
		return nullptr;
	}
}
