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
#include <functional> 
#include <unordered_map>
#include "ScriptVarInfo.h"
#include "EScriptSentenceType.h"
#include "EScriptVariableType.h"
namespace zlscript
{

	//一句源码
	enum E_SOURCE_WORD_FLAG
	{
		E_WORD_FLAG_NORMAL,
		E_WORD_FLAG_STRING,
	};
	struct tagSourceWord
	{
		tagSourceWord()
		{
			nFlag = E_WORD_FLAG_NORMAL;
			nSourceWordsIndex = -1;
		}
		int nFlag;
		std::string word;
		unsigned int nSourceWordsIndex;
	};
	typedef std::list<tagSourceWord> SentenceSourceCode;

#define SignToPos() \
	unsigned int nBeginSourceWordIndex = 0; \
	SentenceSourceCode rollBackList; \
	if (vIn.size() != 0) \
	{ \
		tagSourceWord word = vIn.front(); \
		nBeginSourceWordIndex = word.nSourceWordsIndex; \
	}
#define RevertOne() \
	vIn.push_front(rollBackList.back()); \
	rollBackList.pop_back();

#define RevertAll() \
	while (rollBackList.size() > 0) \
	{ \
		vIn.push_front(rollBackList.back()); \
		rollBackList.pop_back(); \
	}

#define GetNewWord(word) \
	if (vIn.size() == 0) \
	{ \
		RevertAll() \
		return false; \
	} \
	tagSourceWord word = vIn.front(); \
	vIn.pop_front(); \
	rollBackList.push_back(word);

#define GetWord(word) \
	if (vIn.size() == 0) \
	{ \
		RevertAll() \
		return false; \
	} \
	word = vIn.front(); \
	vIn.pop_front(); \
	rollBackList.push_back(word);

#define GetNewWord2(word) \
	if (vIn.size() == 0) \
	{ \
		RevertAll() \
		AddErrorInfo(nBeginSourceWordIndex,"(Unexpected end of statement)"); \
		return nullptr; \
	} \
	tagSourceWord word = vIn.front(); \
	vIn.pop_front(); \
	rollBackList.push_back(word);

#define GetWord2(word) \
	if (vIn.size() == 0) \
	{ \
		RevertAll() \
		AddErrorInfo(nBeginSourceWordIndex,"(Unexpected end of statement)"); \
		return nullptr; \
	} \
	word = vIn.front(); \
	vIn.pop_front(); \
	rollBackList.push_back(word);


	struct tagCodeData
	{
		tagCodeData()
		{
			nType = EICODE_FUN_DEFAULT;
			nDefaultReturnType = EScriptVal_None;
			pBeginCode = nullptr;
			pEndCode = nullptr;
		}
		void AddCode(CBaseExeCode* pCode)
		{
			if (pCode == nullptr)
			{
				return;
			}
			if (pBeginCode == nullptr)
			{
				pBeginCode = pCode;
				pEndCode = pCode;
			}
			else
			{
				pCode->nCodeIndex = pEndCode->nCodeIndex + 1;
				pEndCode->m_pNext = pCode;
				pEndCode = pCode;
			}
		}
		CBaseExeCode* pBeginCode;
		CBaseExeCode* pEndCode;
		int nType;

		std::vector<StackVarInfo> vNumVar;//临时变量
		//std::vector<CodeStyle> vCodeData;

		std::vector<__int64> vInt64Const;//64位整形常量
		std::vector<double> vFloatConst;//浮点常量
		std::vector<std::string> vStrConst;//字符常量

		std::vector<std::string> vCallFunName;//会调用的函数的名称

		std::string filename;
		std::string funname;

		//函数属性
		std::map<std::string, StackVarInfo> m_mapFunAttribute;

		int nDefaultReturnType;//默认返回值类型
	};


}