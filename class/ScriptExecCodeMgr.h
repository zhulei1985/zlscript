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
#include <string>
#include <vector>
#include <map>
namespace zlscript
{
	class CScriptRunState;
	class CScriptExecCodeMgr
	{
	public:
		CScriptExecCodeMgr();
		~CScriptExecCodeMgr();
	public:
		static CScriptExecCodeMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptExecCodeMgr s_Instance;

	public:
		bool RemoteFunctionCall(std::string name, CScriptRunState* pState, int nParmNum);

		bool SetRemoteFunction(std::string name, __int64 nEventIndex);
		void RemoveRemoteFunction(std::string name, __int64 nEventIndex);
	private:
		struct stInfo
		{
			unsigned int unIndex;
			std::vector<__int64> vScriptEventIndexs;
		};

		std::map<std::string, stInfo> m_mapRemoteCallInfo;
	};
}