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

#include "ScriptDebugPrint.h"
#include <stdarg.h>
namespace zlscript
{
	CScriptDebugPrintMgr CScriptDebugPrintMgr::s_Instance;
	CScriptDebugPrintMgr::CScriptDebugPrintMgr()
	{

	}
	CScriptDebugPrintMgr::~CScriptDebugPrintMgr()
	{

	}
	void CScriptDebugPrintMgr::Print(std::string flag, const char* str, ...)
	{
		flag += " : ";
		char strbuff[1024];
		va_list args;
		va_start(args, str);
		vsnprintf(strbuff,sizeof(strbuff),str, args);
		va_end(args);

		Print(flag+strbuff);
	}
	void CScriptDebugPrintMgr::Print(std::string str)
	{
		//str += "\n";
		for (auto it = m_vPrintFunFun.begin(); it != m_vPrintFunFun.end(); it++)
		{
			(*it)(str.c_str());
		}
	}
	void CScriptDebugPrintMgr::RegisterCallBack_PrintFun(std::function<void(const char*)> fun)
	{
		m_vPrintFunFun.push_back(fun);
	}
}