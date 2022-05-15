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

namespace zlscript
{

	struct stScriptClassParamInfo
	{
		unsigned short m_flag;
		unsigned short m_index;
		std::string m_strAttrName;
		std::string strType;//MYSQL 字段类型
	};

	class CBaseScriptClassInfo
	{
	public:
	public:
		std::atomic_int nFunSize;
		std::atomic_int nClassType;
		std::string strClassName;
		std::atomic_int64_t nDB_Id_Count;

		std::map<std::string, int> mapDicFunString2Index;
		std::map<std::string, stScriptClassParamInfo> mapDicString2ParamInfo;
	};
	template<class T>
	class CScriptClassInfo : public CBaseScriptClassInfo
	{
	public:
		CScriptClassInfo()
		{
			nFunSize = 0;
			nClassType = 0;
			nDB_Id_Count = 0;
		}
	public:
		static CScriptClassInfo& GetInstance()
		{
			return s_Instance;
		}
	private:
		static CScriptClassInfo s_Instance;

	};
	template<class T>
	CScriptClassInfo<T> CScriptClassInfo<T>::s_Instance;
}