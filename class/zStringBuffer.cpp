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

#include "zStringBuffer.h"

namespace zlscript
{
	CStringPool::CStringPool()
	{
		m_IndexCount = 0;
	}
	CStringPool::~CStringPool()
	{
	}
	__int64 CStringPool::NewString(const char* pStr)
	{
		if (pStr == nullptr)
		{
			return 0;
		}
		m_Lock.lock();
		m_IndexCount++;
		auto &info = m_StringPool[m_IndexCount];
		info.str = pStr;
		info.nCount = 1;
		m_Lock.unlock();
		return m_IndexCount;
	}
	void CStringPool::UseString(__int64 nIndex)
	{
		m_Lock.lock();
		auto it = m_StringPool.find(nIndex);
		if (it != m_StringPool.end())
		{
			it->second.nCount++;
		}
		m_Lock.unlock();
	}
	void CStringPool::ReleaseString(__int64 nIndex)
	{
		m_Lock.lock();
		auto it = m_StringPool.find(nIndex);
		if (it != m_StringPool.end())
		{
			it->second.nCount--;
			if (it->second.nCount <= 0)
			{
				m_StringPool.erase(it);
			}
		}
		m_Lock.unlock();
	}
	const char* CStringPool::GetString(__int64 nIndex)
	{
		const char* pStr = nullptr;
		m_Lock.lock();
		auto it = m_StringPool.find(nIndex);
		if (it != m_StringPool.end())
		{
			pStr = it->second.str.c_str();
		}
		m_Lock.unlock();
		return pStr;
	}
}