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

#define MAXSIZE_STRING 2048
#include <string>
#include <map>
#include <list>
#include <vector>
#include <stack>
#include <set>
#include <functional> 
#include <unordered_map>
 //#include "scriptcommon.h"

#include "ScriptCompileInfo.h"
namespace zlscript
{
	class CScriptCompiler;
	class CExeCodeData;
	class CScriptCodeLoader
	{
	public:
		CScriptCodeLoader(void);
		~CScriptCodeLoader(void);
	public:
		static CScriptCodeLoader* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptCodeLoader s_Instance;

	public:
		void RegisterCallBack_LoadFun(std::function<bool(const char*, std::vector<char>&)>);
		bool LoadFile(const char* filename, CScriptCompiler* pCompiler);

	private:
		static bool DefaultLoadFile(const char*, std::vector<char>&);
		typedef std::function< bool(const char*, std::vector<char>&)> LoadFun;
		std::vector<LoadFun> m_vLoadFun;
	private:
		//***************字典*****************//

		std::map<std::string, unsigned short> m_mapDicSignToPRI;//计算符的优先级，越高越优先

		std::map<std::string, unsigned short> m_mapDic2KeyWord;

		//*********************中间码的字典**************************//
		std::map<std::string, unsigned short> m_mapDicVarTypeToICode;
		std::map<std::string, unsigned short> m_mapDicSentenceToICode;

		std::map<std::string, unsigned short> m_mapDicFunToICode;

		void initDic();

	public:

		unsigned short GetWordKey(std::string str)
		{
			auto it = m_mapDic2KeyWord.find(str);
			if (it != m_mapDic2KeyWord.end())
			{
				return it->second;
			}
			return 0;
		}
		unsigned short GetSignPRI(std::string str)
		{
			auto it = m_mapDicSignToPRI.find(str);
			if (it != m_mapDicSignToPRI.end())
			{
				return it->second;
			}
			return 0xffff;
		}
	public:

		//*****************代码*******************//

		unsigned int GetCodeIndex(const char* pStr);
	private:

		//代码库
		std::vector<CExeCodeData*> m_vecCodeData;

		std::unordered_map<std::string, int> m_mapString2CodeIndex;
	public:

		int GetCodeSize()
		{
			return (int)m_vecCodeData.size();
		}
		CExeCodeData* GetCode(int index)
		{
			if (index >= 0 && index < (int)m_vecCodeData.size())
			{
				return m_vecCodeData[index];
			}
			return nullptr;
		}
		CExeCodeData* GetCode(const char* pName);
		int RegisterCodeName(std::string name);
		void LoadXml(std::string filename);

		void PrintAllCode(const char* pFilename);

		void clear();

		//检查变量名是否合法
		bool CheckVarName(std::string varName);

		bool AddCode(std::string name, CExeCodeData* pCode);


	public:
		void SaveToBin();
		void LoadFormBin();

		friend class CScriptVirtualMachine;
	};

}