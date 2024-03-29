﻿/****************************************************************************
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
#include <unordered_map>
#include <string>
#include <mutex>
namespace zlscript
{
	class CStringPool
	{
	public:
		CStringPool();
		~CStringPool();

		__int64 NewString(const char* pStr);

		void UseString(__int64 nIndex);
		void ReleaseString(__int64 nIndex);

		std::string& GetString(__int64 nIndex);

		struct tagInfo
		{
			std::string str;
			int nCount;
		};
	protected:
		std::unordered_map<std::string, __int64> m_String2Index;
		std::map<__int64, tagInfo> m_StringPool;
		__int64 m_IndexCount;

		std::string strEmpty{""};
		std::mutex m_Lock;
	};
}
 