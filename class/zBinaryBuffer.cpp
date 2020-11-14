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

#include "zBinaryBuffer.h"

namespace zlscript
{
	CBinaryPool::CBinaryPool()
	{
		m_IndexCount = 0;
	}
	CBinaryPool::~CBinaryPool()
	{
	}
	__int64 CBinaryPool::NewBinary(const char* pStr, unsigned int size)
	{
		if (pStr == nullptr)
		{
			return 0;
		}
		if (size == 0)
		{
			return 0;
		}
		m_Lock.lock();
		m_IndexCount++;
		auto &info = m_BinaryPool[m_IndexCount];
		info.data.resize(size);
		memcpy(&info.data[0],pStr,size);
		info.nCount = 1;
		m_Lock.unlock();
		return m_IndexCount;
	}
	void CBinaryPool::UseBinary(__int64 nIndex)
	{
		m_Lock.lock();
		auto it = m_BinaryPool.find(nIndex);
		if (it != m_BinaryPool.end())
		{
			it->second.nCount++;
		}
		m_Lock.unlock();
	}
	void CBinaryPool::ReleaseBinary(__int64 nIndex)
	{
		m_Lock.lock();
		auto it = m_BinaryPool.find(nIndex);
		if (it != m_BinaryPool.end())
		{
			it->second.nCount--;
			if (it->second.nCount <= 0)
			{
				m_BinaryPool.erase(it);
			}
		}
		m_Lock.unlock();
	}
	const char* CBinaryPool::GetBinary(__int64 nIndex, unsigned int & size)
	{
		const char* pStr = nullptr;
		m_Lock.lock();
		auto it = m_BinaryPool.find(nIndex);
		if (it != m_BinaryPool.end())
		{
			pStr = &it->second.data[0];
			size = it->second.data.size();
		}
		else
		{
			size = 0;
		}
		m_Lock.unlock();
		return pStr;
	}
}