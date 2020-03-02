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
#include <string>
#include <functional>
#include <vector>
namespace zlscript
{
	class CScriptDebugPrintMgr
	{
	public:
		CScriptDebugPrintMgr();
		~CScriptDebugPrintMgr();

	public:
		void Print(std::string str);
		void RegisterCallBack_PrintFun(std::function<void(const char*)>);
	private:
		typedef std::function<void(const char*)> PrintFun;
		std::vector<PrintFun> m_vPrintFunFun;
	public:
		static CScriptDebugPrintMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptDebugPrintMgr s_Instance;
	};
}