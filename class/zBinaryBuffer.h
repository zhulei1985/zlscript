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
#include <map>
#include <vector>
#include <mutex>
namespace zlscript
{
	class CBinaryPool
	{
	public:
		CBinaryPool();
		~CBinaryPool();

		__int64 NewBinary(const char* pStr, unsigned int size);

		void UseBinary(__int64 nIndex);
		void ReleaseBinary(__int64 nIndex);

		const char* GetBinary(__int64 nIndex, unsigned int& size);

		struct tagInfo
		{
			std::vector<char> data;
			int nCount;
		};
	protected:
		std::map<__int64, tagInfo> m_BinaryPool;
		__int64 m_IndexCount;

		std::mutex m_Lock;
	};
}
 