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

#include "scriptcommon.h"

#include <sstream>


#include "EScriptSentenceType.h"
#include "EScriptVariableType.h"
#include "ScriptCallBackFunion.h"
#include "EMicroCodeType.h"

#include "ScriptCodeLoader.h"
#include "ScriptDebugPrint.h"

#include "zByteArray.h"
#include "ScriptSuperPointer.h"
#include "ScriptIntermediateCode.h"

#pragma warning(disable : 4996) 

using namespace std;
namespace zlscript
{
	enum E_OPERATOR_FLAG
	{
		E_OPERATOR_NONE,
		E_OPERATOR_OPERAND = 1,
		E_OPERATOR_LEFT = 2,
		E_OPERATOR_RIGHT = 4,
		E_OPERATOR_LEFT_AND_RIGHT = 8,
	};
	CScriptCodeLoader CScriptCodeLoader::s_Instance;
	CScriptCodeLoader::CScriptCodeLoader(void)
	{
		//CScriptCallBackFunion::init();
		initDic();
		RegisterCallBack_LoadFun(DefaultLoadFile);
	}

	CScriptCodeLoader::~CScriptCodeLoader(void)
	{
	}
	void CScriptCodeLoader::RegisterCallBack_LoadFun(std::function<bool(const char*, std::vector<char>&)> fun)
	{
		m_vLoadFun.push_back(fun);
	}
	bool CScriptCodeLoader::LoadFile(const char* filename)
	{
		clearAllCompileData();
#if _SCRIPT_DEBUG
		strCurFileName = filename;
		//nErrorWordPos = 0;
		m_vError.clear();
#endif

		std::vector<char> vBuff;
		auto rit = m_vLoadFun.rbegin();
		for (; rit != m_vLoadFun.rend(); rit++)
		{
			vBuff.clear();
			if ((*rit)(filename, vBuff))
			{
				break;
			}
		}
#if _SCRIPT_DEBUG
		PartitionSourceWords(vBuff);
#endif
		string strbuff;
		for (unsigned int i = 0; i < vBuff.size(); )
		{
			if (RunLexical(strbuff, vBuff[i], i))
			{
				i++;
			}
		}


		if (m_vCurSourceSentence.empty())
		{
			//空文件,或者是没有有意义字符的文件
			return false;
		}

		//词法分析完成，开始语法分析
		unsigned int nStrIndex = 0;

		while (m_vCurSourceSentence.size())
		{
			if (RunCompileState(m_vCurSourceSentence) == ECompile_ERROR)
			{
				zlscript::CScriptDebugPrintMgr::GetInstance()->Print("ScriptLoad Error:");

				for (unsigned int i = 0; i < m_vError.size(); i++)
				{
					zlscript::CScriptDebugPrintMgr::GetInstance()->Print(strCurFileName + ":" + m_vError[i].strError);
#ifdef  _SCRIPT_DEBUG
					auto souceInfo = GetSourceWords(m_vError[i].nErrorWordPos);
					zlscript::CScriptDebugPrintMgr::GetInstance()->Print("Debug","file:%s,line:%d,word:%s",
						souceInfo.strCurFileName.c_str(), souceInfo.nLineNum, souceInfo.strLineWords.c_str());
#endif //  _SCRIPT_DEBUG
				}
				
				break;
			}
		}

		//#if _SCRIPT_DEBUG
		//			sprintf("编译失败 %s,%s",filename,strErrorWord.size());
		//#endif
		return true;
	}

	bool CScriptCodeLoader::DefaultLoadFile(const char* filename, std::vector<char>& vOut)
	{
		FILE* fp = fopen(filename, "rb");
		if (fp == nullptr)
		{
			return false;
		}

		string strbuff;
		char ch = fgetc(fp);
		while (ch != EOF)
		{
			vOut.push_back(ch);
			ch = fgetc(fp);
		}

		fclose(fp);
		fp = nullptr;
		return true;
	}


	void CScriptCodeLoader::initDic()
	{
		m_mapDic2KeyWord["include"] = SCRIPTSENTENCE_INCLUDE;
		m_mapDic2KeyWord["if"] = SCRIPTSENTENCE_IF;//if
		m_mapDic2KeyWord["while"] = SCRIPTSENTENCE_WHILE;//while
		m_mapDic2KeyWord["else"] = SCRIPTSENTENCE_ELSE;//else
		m_mapDic2KeyWord["switch"] = SCRIPTSENTENCE_SWITCH;//switch

		m_mapDic2KeyWord["=="] = ECODE_CMP_EQUAL;//==
		m_mapDic2KeyWord["!="] = ECODE_CMP_NOTEQUAL;//!=
		m_mapDic2KeyWord[">"] = ECODE_CMP_BIG;//>
		m_mapDic2KeyWord[">="] = ECODE_CMP_BIGANDEQUAL;//>=
		m_mapDic2KeyWord["<"] = ECODE_CMP_LESS;//<
		m_mapDic2KeyWord["<="] = ECODE_CMP_LESSANDEQUAL;//<=

		m_mapDic2KeyWord["&"] = ECODE_BIT_AND;
		m_mapDic2KeyWord["|"] = ECODE_BIT_OR;
		m_mapDic2KeyWord["^"] = ECODE_BIT_XOR;

		m_mapDic2KeyWord["+"] = ECODE_ADD; //加+
		m_mapDic2KeyWord["-"] = ECODE_SUM; //减-
		m_mapDic2KeyWord["*"] = ECODE_MUL; //乘*
		m_mapDic2KeyWord["/"] = ECODE_DIV; //除/
		m_mapDic2KeyWord["%"] = ECODE_MOD; //求余%

		m_mapDic2KeyWord["="] = ECODE_MOVE;// 赋值=

		m_mapDic2KeyWord["->"] = ECODE_CALL_CLASS_FUN; //调用类函数
		m_mapDic2KeyWord["new"] = ECODE_NEW_CLASS; //调用类函数
		//符号
		m_mapDicSignToEnum["=="] = E_OPERATOR_LEFT_AND_RIGHT;
		m_mapDicSignToEnum["!="] = E_OPERATOR_LEFT_AND_RIGHT;
		m_mapDicSignToEnum[">"] = E_OPERATOR_LEFT_AND_RIGHT;
		m_mapDicSignToEnum[">="] = E_OPERATOR_LEFT_AND_RIGHT;
		m_mapDicSignToEnum["<"] = E_OPERATOR_LEFT_AND_RIGHT;
		m_mapDicSignToEnum["<="] = E_OPERATOR_LEFT_AND_RIGHT;

		m_mapDicSignToEnum["&"] = E_OPERATOR_LEFT_AND_RIGHT;
		m_mapDicSignToEnum["|"] = E_OPERATOR_LEFT_AND_RIGHT;
		m_mapDicSignToEnum["^"] = E_OPERATOR_LEFT_AND_RIGHT;

		m_mapDicSignToEnum["+"] = E_OPERATOR_LEFT_AND_RIGHT;
		m_mapDicSignToEnum["-"] = E_OPERATOR_RIGHT | E_OPERATOR_LEFT_AND_RIGHT;
		m_mapDicSignToEnum["*"] = E_OPERATOR_LEFT_AND_RIGHT;
		m_mapDicSignToEnum["/"] = E_OPERATOR_LEFT_AND_RIGHT;
		m_mapDicSignToEnum["%"] = E_OPERATOR_LEFT_AND_RIGHT;

		m_mapDicSignToEnum["->"] = E_OPERATOR_LEFT_AND_RIGHT; //调用类函数

		m_mapDicSignToEnum["="] = E_OPERATOR_LEFT_AND_RIGHT;
		m_mapDicSignToEnum["new"] = E_OPERATOR_RIGHT;

		m_mapDicSignToPRI["="] = 0;

		m_mapDicSignToPRI["=="] = 2;//==
		m_mapDicSignToPRI["!="] = 2;//!=
		m_mapDicSignToPRI[">"] = 2;//>
		m_mapDicSignToPRI[">="] = 2;//>=
		m_mapDicSignToPRI["<"] = 2;//<
		m_mapDicSignToPRI["<="] = 2;//<=

		m_mapDicSignToPRI["&"] = 1;//>=
		m_mapDicSignToPRI["|"] = 1;//<
		m_mapDicSignToPRI["^"] = 1;//<=

		m_mapDicSignToPRI["+"] = 3; //加+
		m_mapDicSignToPRI["-"] = 3; //减-
		m_mapDicSignToPRI["*"] = 4; //乘*
		m_mapDicSignToPRI["/"] = 4; //除/
		m_mapDicSignToPRI["%"] = 4; //求余%

		m_mapDicSignToPRI["->"] = 5; //调用类函数
		m_mapDicSignToPRI["new"] = 5; //调用类函数

		//m_mapDic2KeyWord["int"] = EScriptVal_Int;
		//m_mapDic2KeyWord["float"] = EScriptVal_Double;
		//m_mapDic2KeyWord["string"] = EScriptVal_String;
		//*********************中间码的字典**************************//
		m_mapDicSentenceToICode["global"] = EICode_Global_Define;
		m_mapDicVarTypeToICode["void"] = EScriptVal_None;
		m_mapDicVarTypeToICode["int"] = EScriptVal_Int;
		m_mapDicVarTypeToICode["float"] = EScriptVal_Double;
		m_mapDicVarTypeToICode["string"] = EScriptVal_String;

		m_mapDicFunToICode["intact"] = EICODE_FUN_CAN_BREAK;
	}
	char CScriptCodeLoader::GetVarType(std::string type, unsigned short& classtype)
	{
		auto itType = m_mapDicVarTypeToICode.find(type);
		if (itType == m_mapDicVarTypeToICode.end())
		{
			int nClassType = CScriptSuperPointerMgr::GetInstance()->GetClassType(type);
			if (nClassType)
			{
				classtype = nClassType;
				return EScriptVal_ClassPoint;
			}
		}
		else
		{
			return (char)itType->second;
		}
		return EScriptVal_None;
	}
	void CScriptCodeLoader::clear()
	{
		//m_mapNewString2CodeIndex.clear();
		m_mapString2CodeIndex.clear();

		m_vecCodeData.clear();

		vGlobalNumVar.clear();
	}
	void CScriptCodeLoader::clearAllCompileData()
	{
		//清空词法分析机的一些临时
		while (!m_stackLexical.empty())
		{
			m_stackLexical.pop();
		}
		//m_vLexicalData.clear();
		m_vCurSourceSentence.clear();
	}
	bool CScriptCodeLoader::RunLexical(string& strOut, char ch, unsigned int nSourceIndex)
	{
		if (m_stackLexical.empty())
		{
			m_stackLexical.push(ESkipState);
		}

		if (m_stackLexical.size())
		{
			int nState = m_stackLexical.top();

			int nResult = 0;
			switch (nState)
			{
			case ESkipState:
				nResult = LoadSkipState(strOut, ch);
				break;
			case ESignState:
				nResult = LoadSignState(strOut, ch);
				break;
			case EKeyState:
				nResult = LoadKeyState(strOut, ch);
				break;
			case EStringState:
				nResult = LoadStringState(strOut, ch);
				break;
			case ESkipAnnotateState:
				nResult = LoadSkipAnnotateState(strOut, ch);
				break;
			}

			switch (nResult & 0x7fff)
			{
			case EReturn:
				if (strOut.size() > 0)
				{
					tagSourceWord word;
					word.word = strOut;
					if (nState == EStringState)
					{
						word.nFlag = E_WORD_FLAG_STRING;
					}
#if _SCRIPT_DEBUG
					word.nSourceWordsIndex = GetSourceWordsIndex(nSourceIndex);
#endif
					m_vCurSourceSentence.push_back(word);
					strOut.clear();
				}
				else if (nState == EStringState)
				{
					//允许空字符串
					tagSourceWord word;
					word.word = strOut;
					word.nFlag = E_WORD_FLAG_STRING;
#if _SCRIPT_DEBUG
					word.nSourceWordsIndex = GetSourceWordsIndex(nSourceIndex);
#endif
					m_vCurSourceSentence.push_back(word);
				}
				m_stackLexical.pop();
				if (nResult & ENeedLoadNewChar)
				{
					return true;
				}
				return false;
			case EContinue:
				return true;
			default:
				if (strOut.size() > 0)
				{
					tagSourceWord word;
					word.word = strOut;
#if _SCRIPT_DEBUG
					word.nSourceWordsIndex = GetSourceWordsIndex(nSourceIndex);
#endif
					m_vCurSourceSentence.push_back(word);
					strOut.clear();
				}
				m_stackLexical.push(nResult & 0x7fff);
				if (nResult & ENeedLoadNewChar)
				{
					return true;
				}
				return false;
			}
		}
		return true;
	}
	int CScriptCodeLoader::LoadSkipState(string& strOut, char ch)
	{
		//如果是字符
		switch (ch)
		{
		case ' '://跳过空格
			return EContinue;
		case 13://跳过回车
		case 10://跳过换行
			return EContinue;

		case '_'://这个字符转到LoadKeyState
			return EKeyState;
		case '"'://双引号转到LoadStringState
			//strOut.push_back(ch);
			return EStringState | ENeedLoadNewChar;

		}

		//所有大些字母转到LoadKeyState
		if (ch >= 'A' && ch <= 'Z')
		{
			return EKeyState;
		}
		//所有小写字母转到LoadKeyState
		if (ch >= 'a' && ch <= 'z')
		{
			return EKeyState;
		}
		//所有数字转到LoadKeyState
		if (ch >= '0' && ch <= '9')
		{
			return EKeyState;
		}

		//剩下的字符
		//以下符号转到LoadSignState
		// ! ~ # $ % & ' ( ) * + , - . /
		// : ; < = > ? @
		// [ \ ] ^ _ '
		// { | } ~
		if (ch >= 33 && ch <= 126)
		{
			return ESignState;
		}


		return EReturn | ENeedLoadNewChar;
	}
	//读取符号
	int CScriptCodeLoader::LoadSignState(string& strOut, char ch)
	{
		//排除一些特别字符
		switch (ch)
		{
		case ' '://跳过空格
		case 13://跳过回车
		case 10://跳过换行
			return EReturn | ENeedLoadNewChar;
		case '_'://这个字符转到LoadKeyState
			return EKeyState;
		case '"'://双引号转到LoadStringState
			//strOut.push_back(ch);
			return EStringState | ENeedLoadNewChar;
			//case ';'://分号作为一句的结束
			//	//m_vLexicalData.push_back(m_vCurSourceSentence);
			//	//m_vCurSourceSentence.clear();
			//	return EReturn | ENeedLoadNewChar;
		}

		//排除所有大些字母
		if (ch >= 'A' && ch <= 'Z')
		{
			return EReturn;
		}
		//排除所有小写字母
		if (ch >= 'a' && ch <= 'z')
		{
			return EReturn;
		}
		//排除所有数字
		if (ch >= '0' && ch <= '9')
		{
			return EReturn;
		}

		//剩下的字符
		//以下符号转到LoadSignState
		// ! ~ # $ % & ' ( ) * + , - . /
		// : ; < = > ? @
		// [ \ ] ^ _ '
		// { | } ~
		if (ch >= 33 && ch <= 126)
		{
			strOut.push_back(ch);
			//可能存在多个符号字符组成的符号，比如 >= <= ==
			if (strOut.size() == 1)
			{
				switch (ch)
				{
				case '>':
				case '<':
				case '=':
				case '!':
				case '/':
				case '-':
					return EContinue;
				}
			}
			else if (strOut.size() == 2)
			{
				if (strOut == "//")
				{
					//这个是注释
					strOut.clear();
					return ESkipAnnotateState;
				}
			}
		}
		return EReturn | ENeedLoadNewChar;
	}
	//读取数据
	int CScriptCodeLoader::LoadKeyState(string& strOut, char ch)
	{
		if (ch == '_')
		{
			strOut.push_back(ch);
			return EContinue;
		}
		//所有大些字母转到LoadKeyState
		if (ch >= 'A' && ch <= 'Z')
		{
			strOut.push_back(ch);
			return EContinue;
		}
		//所有小写字母转到LoadKeyState
		if (ch >= 'a' && ch <= 'z')
		{
			strOut.push_back(ch);
			return EContinue;
		}
		//所有数字转到LoadKeyState
		if (ch >= '0' && ch <= '9')
		{
			strOut.push_back(ch);
			return EContinue;
		}
		if (ch == '.')
		{
			strOut.push_back(ch);
			return EContinue;
		}
		return EReturn;
	}
	//读取字符串
	int CScriptCodeLoader::LoadStringState(string& strOut, char ch)
	{
		if (ch == '"')
		{
			return EReturn | ENeedLoadNewChar;
		}
		strOut.push_back(ch);
		return EContinue;
	}
	int CScriptCodeLoader::LoadSkipAnnotateState(string& strOut, char ch)
	{
		switch (ch)
		{
		case 13://回车
		case 10://换行
			return EReturn | ENeedLoadNewChar;
		}
		return EContinue;
	}


	bool CScriptCodeLoader::CheckOperator(std::string word)
	{
		if (word == "")
		{
			return false;
		}
		auto it = m_mapDicSignToEnum.find(word);
		if (it != m_mapDicSignToEnum.end())
		{
			return true;
		}
		return false;
	}

	bool CScriptCodeLoader::CheckVarName(string varName)
	{
		if (varName == "")
		{
			return false;
		}
		bool bResult = true;
		//TODO 不能是关键字和函数名
		return bResult;
	}

	int CScriptCodeLoader::QueryTempVar(std::string varName, CBaseICode* pICode)
	{
		if (pICode)
		{
			return pICode->GetTempVarIndex(varName.c_str());
		}
		return g_nTempVarIndexError;
	}
#define SignToPos \
	unsigned int nBeginSourceWordIndex = 0; \
	if (vIn.size() != 0) \
	{ \
		tagSourceWord word = vIn.front(); \
		nBeginSourceWordIndex = word.nSourceWordsIndex; \
	}

#define GetNewWord(word) \
	if (vIn.size() == 0) \
	{ \
		AddErrorInfo(nBeginSourceWordIndex,"(Unexpected end of statement)"); \
		return ECompile_ERROR; \
	} \
	tagSourceWord word = vIn.front(); \
	vIn.pop_front();

#define GetWord(word) \
	if (vIn.size() == 0) \
	{ \
		AddErrorInfo(nBeginSourceWordIndex,"(Unexpected end of statement)"); \
		return ECompile_ERROR; \
	} \
	word = vIn.front(); \
	vIn.pop_front();

#define GetNewWord2(word) \
	if (vIn.size() == 0) \
	{ \
		AddErrorInfo(nBeginSourceWordIndex,"(Unexpected end of statement)"); \
		return nullptr; \
	} \
	tagSourceWord word = vIn.front(); \
	vIn.pop_front();

#define GetWord2(word) \
	if (vIn.size() == 0) \
	{ \
		AddErrorInfo(nBeginSourceWordIndex,"(Unexpected end of statement)"); \
		return nullptr; \
	} \
	word = vIn.front(); \
	vIn.pop_front();

#define RevertWord(word) \
	vIn.push_front(word);

	bool CScriptCodeLoader::RunCompileState(SentenceSourceCode& vIn)
	{
		if (LoadDefineFunState(vIn) == ECompile_ERROR)
		{
			return false;
		}
		return true;
	}

	int CScriptCodeLoader::LoadDefineFunctionParameter(SentenceSourceCode& vIn, CBaseICode* pCode)
	{
		SignToPos;
		GetNewWord(strType);
		GetNewWord(strName);
		if (!pCode->DefineTempVar(strType.word, strName.word))
		{
			AddErrorInfo(
				strType.nSourceWordsIndex,
				"DefineTempVar(defining temporary variable error)");
			return ECompile_ERROR;
		}
		return ECompile_Return;
	}

	int CScriptCodeLoader::LoadDefineTempVar(SentenceSourceCode& vIn, CBaseICode* pCode)
	{
		SignToPos;
		GetNewWord(strType);
		GetNewWord(strName);

		if (!pCode->DefineTempVar(strType.word, strName.word))
		{
			AddErrorInfo(
				strType.nSourceWordsIndex,
				"DefineTempVar(defining temporary variable error)");
			return ECompile_ERROR;
		}

		GetNewWord(nextWord);
		if (nextWord.word == "=")
		{
			RevertWord(nextWord);
			RevertWord(strName);

			return ECompile_Next;
		}
		return ECompile_Return;
	}

	int CScriptCodeLoader::LoadDefineFunState(SentenceSourceCode& vIn)
	{
		SignToPos;

		std::vector<std::string> vHeadFlag;
		std::string strVarType;//返回类型
		std::string strName;
		while (true)
		{
			GetNewWord(strHeadFlag);
			auto it = m_mapDicFunToICode.find(strHeadFlag.word);
			if (it != m_mapDicFunToICode.end())
			{
				vHeadFlag.push_back(strHeadFlag.word);
			}
			else
			{
				RevertWord(strHeadFlag);
				break;
			}
		}
		GetNewWord(VarTypeWord);
		strVarType = VarTypeWord.word;

		int nVarType = m_mapDicVarTypeToICode[strVarType];
		if (m_mapDicVarTypeToICode.find(strVarType) == m_mapDicVarTypeToICode.end())
		{
			if (CScriptSuperPointerMgr::GetInstance()->GetClassType(strVarType))
			{
				AddErrorInfo(
					VarTypeWord.nSourceWordsIndex,
					"LoadDefineFunState(var type error)");
				return ECompile_ERROR;
			}
		}

		GetNewWord(NameWord);
		strName = NameWord.word;

		//检测名字是否与回调函数名冲突
		if (CScriptCallBackFunion::GetFunIndex(strName) >= 0)
		{
			AddErrorInfo(
				NameWord.nSourceWordsIndex,
				"LoadDefineFunState(Callback Function name already exists)");
			return ECompile_ERROR;
		}

		GetNewWord(NextWord);

		//如果语句结束，说明只是声明了一个全局变量
		if (NextWord.word == ";")
		{
			if (m_mapDicGlobalVar.find(strName) != m_mapDicGlobalVar.end())
			{
				if ((int)m_mapDicGlobalVar[strName].cType != nVarType
					|| m_mapDicGlobalVar[strName].wExtend != CScriptSuperPointerMgr::GetInstance()->GetClassType(strVarType))
				{
					AddErrorInfo(
						NextWord.nSourceWordsIndex,
						"LoadDefineFunState(Global var type error)");
					return ECompile_ERROR;
				}
			}
			else
			{
				VarInfo info;
				info.cType = nVarType;
				info.cGlobal = 1;
				if (nVarType == EScriptVal_ClassPoint)
					info.wExtend = CScriptSuperPointerMgr::GetInstance()->GetClassType(strVarType);
				else
					info.wExtend = 0;
				info.dwPos = vGlobalNumVar.size();
				m_mapDicGlobalVar[strName] = info;
				StackVarInfo defVar;//默认值
				defVar.cType = nVarType;
				vGlobalNumVar.push_back(defVar);
			}
		}
		//说明初始化了一个全局变量
		else if (NextWord.word == "=")
		{
			//GetWord(nextWord);
			StackVarInfo defVar;//默认值
			defVar.cType = nVarType;

			//有初始化值，只支持常量赋值
			GetWord(NextWord);
			switch (nVarType)
			{
				case EScriptVal_Int:
				{
					defVar.Int64 = _atoi64(NextWord.word.c_str());
				}
				break;
				case EScriptVal_Double:
				{
					defVar.Double = atof(NextWord.word.c_str());
				}
				break;
				case EScriptVal_String:
				{
					defVar.Int64 = StackVarInfo::s_strPool.NewString(NextWord.word.c_str());
				}
				break;
				case EScriptVal_ClassPoint:
				{
					if (NextWord.word == "nullptr")
					{
						defVar.Int64 = 0;
					}
				}
			}
			GetWord(NextWord);
			if (NextWord.word != ";")
			{
				AddErrorInfo(
					NextWord.nSourceWordsIndex,
					"Format error defining global variable");
				return ECompile_ERROR;
			}
			if (m_mapDicGlobalVar.find(strName) == m_mapDicGlobalVar.end())
			{
				VarInfo info;
				info.cType = nVarType;
				info.cGlobal = 1;
				if (nVarType == EScriptVal_ClassPoint)
					info.wExtend = CScriptSuperPointerMgr::GetInstance()->GetClassType(strVarType);
				else
					info.wExtend = 0;
				info.dwPos = vGlobalNumVar.size();
				m_mapDicGlobalVar[strName] = info;
				vGlobalNumVar.push_back(defVar);
			}
			else
			{
				VarInfo &info = m_mapDicGlobalVar[strName];
				if (info.cType != defVar.cType
					|| info.wExtend != CScriptSuperPointerMgr::GetInstance()->GetClassType(strVarType))
				{
					AddErrorInfo(
						NextWord.nSourceWordsIndex,
						"LoadDefineFunState(Global var type error)");
					return ECompile_ERROR;
				}
				if (info.dwPos < vGlobalNumVar.size() && vGlobalNumVar[info.dwPos].cType == EScriptVal_None)
					vGlobalNumVar[info.dwPos] = defVar;
			}
		}
		//如果接下来的是括号，说明是函数定义
		else if (NextWord.word == "(")
		{
			//m_vTempCodeData.vCodeData.clear();
			//m_vTempCodeData.vNumVar.clear();
			//m_vTempCodeData.vFloatConst.clear();
			//m_vTempCodeData.vStrConst.clear();
			//一个新函数
			CFunICode *pFunICode = m_mapString2Code[strName];
			if (pFunICode == nullptr)
			{
				pFunICode = CICodeMgr::GetInstance()->New<CFunICode>(NextWord.nSourceWordsIndex);
				pFunICode->strReturnType = strVarType;

				m_mapString2Code[strName] = pFunICode;
			}

			int VarIndex = 0;
			while (true)
			{
				GetWord(NextWord);
				if (NextWord.word == ")")
				{
					break;
				}
				RevertWord(NextWord);

				if (LoadDefineFunctionParameter(vIn, pFunICode) <= 0)
				{
					return ECompile_ERROR;
				}

				GetWord(NextWord);

				if (NextWord.word == ")")
				{
					break;
				}
				else if (NextWord.word != ",")
				{
					AddErrorInfo(
						NextWord.nSourceWordsIndex,
						"LoadDefineFunState(Function parameter definition format error)");
					return ECompile_ERROR;
				}
			}

			GetWord(NextWord);
			if (NextWord.word == "{")
			{
				if (pFunICode->pBodyCode != nullptr)
				{
					//这个函数已经定义过了
					AddErrorInfo(
						NextWord.nSourceWordsIndex,
						"LoadDefineFunState(Function Block already exists)");
					return ECompile_ERROR;
				}
				RevertWord(NextWord);
				if (LoadBlockState(vIn, pFunICode, 0) != ECompile_ERROR)
				{
					pFunICode->funname = NameWord.word;
				}
				else
				{
					AddErrorInfo(NextWord.nSourceWordsIndex, "LoadDefineFunState(Function Load Block error)");
					return ECompile_ERROR;
				}
			}
			else if (NextWord.word == ";")
			{
				//说明只是声明了函数，还没有定义
			}

			//将数据放入代码库
		}
		else
		{
			//错误
			AddErrorInfo(
				NextWord.nSourceWordsIndex,
				"LoadDefineFunState(format error)");
			return ECompile_ERROR;
		}


		return ECompile_Return;
	}
	int CScriptCodeLoader::LoadBlockState(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	{
		SignToPos;
		GetNewWord(nextWord);
		CBlockICode* pBlockICode = CICodeMgr::GetInstance()->New<CBlockICode>(nextWord.nSourceWordsIndex);
		if (pBlockICode == nullptr)
		{
			return ECompile_ERROR;
		}
		pCode->AddICode(nType, pBlockICode);

		if (nextWord.word != "{")
		{
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"LoadBlockState(Block format error)");
			return ECompile_ERROR;
		}

		int nReturn = ECompile_Return;
		while (vIn.size())
		{
			GetWord(nextWord);
			if (nextWord.word == "}")
			{
				break;
			}
			else if (nextWord.word == "{")
			{
				RevertWord(nextWord);
				nReturn = LoadBlockState(vIn, pBlockICode, 0);
			}
			else if (nextWord.word == "if")
			{
				RevertWord(nextWord);
				nReturn = LoadIfSentence(vIn, pBlockICode, 0);
			}
			else if (nextWord.word == "for")
			{
				RevertWord(nextWord);
				nReturn = LoadForSentence(vIn, pBlockICode, 0);
			}
			else if (nextWord.word == "while")
			{
				RevertWord(nextWord);
				nReturn = LoadWhileSentence(vIn, pBlockICode, 0);
			}
			else if (nextWord.word == "switch")
			{
				RevertWord(nextWord);
				nReturn = LoadSwitchSentence(vIn, pBlockICode, 0);
			}
			else if (nextWord.word == "return")
			{
				RevertWord(nextWord);
				nReturn = LoadReturnSentence(vIn, pBlockICode, 0);
			}
			else if (nextWord.word == "delete")
			{
				GetWord(nextWord);
				CDeleteICode* pDelICode = CICodeMgr::GetInstance()->New<CDeleteICode>(nextWord.nSourceWordsIndex);
				pDelICode->m_VarName = nextWord.word;
				pBlockICode->AddICode(0, pDelICode);

				GetWord(nextWord);
				if (nextWord.word != ";")
				{
					AddErrorInfo(
						nextWord.nSourceWordsIndex,
						"delete: format error");
					return ECompile_ERROR;
				}
			}
			else if (nextWord.word == "break")
			{
				GetWord(nextWord);

				if (nextWord.word != ";")
				{
					AddErrorInfo(
						nextWord.nSourceWordsIndex,
						"break: format error");
					return ECompile_ERROR;
				}
				CBreakICode* pBreakICode = CICodeMgr::GetInstance()->New<CBreakICode>(nextWord.nSourceWordsIndex);
				if (pBreakICode == nullptr)
				{
					return ECompile_ERROR;
				}
				pBlockICode->AddICode(0,pBreakICode);
			}
			else if (nextWord.word == "continue")
			{
				GetWord(nextWord);

				if (nextWord.word != ";")
				{
					AddErrorInfo(
						nextWord.nSourceWordsIndex,
						"continue: format error");
					return ECompile_ERROR;
				}
				CContinueICode* pContinueICode = CICodeMgr::GetInstance()->New<CContinueICode>(nextWord.nSourceWordsIndex);
				if (pContinueICode == nullptr)
				{
					return ECompile_ERROR;
				}
				pBlockICode->AddICode(0, pContinueICode);
			}
			else if (nextWord.word == "int" || nextWord.word == "float" || nextWord.word == "string"
				|| CScriptSuperPointerMgr::GetInstance()->GetClassType(nextWord.word) > 0)
			{
				RevertWord(nextWord);
				nReturn = LoadDefineTempVar(vIn, pBlockICode);
				if (nReturn == ECompile_Next)
					nReturn = LoadOperatiorState(vIn, pBlockICode, 0);
			}
			else
			{
				RevertWord(nextWord);
				nReturn = LoadOperatiorState(vIn, pBlockICode, 0);
			}

			if (nReturn == ECompile_ERROR)
				return ECompile_ERROR;
		}

		return nReturn;
	}
	int CScriptCodeLoader::LoadIfSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	{
		SignToPos;
		GetNewWord(nextWord);
		CIfICode* pIfICode = CICodeMgr::GetInstance()->New<CIfICode>(nextWord.nSourceWordsIndex);
		if (pIfICode == nullptr)
		{
			return ECompile_ERROR;
		}
		pCode->AddICode(nType, pIfICode);


		if (nextWord.word != "if")
		{
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"LoadIfSentence(Block format error)");
			return ECompile_ERROR;
		}

		GetWord(nextWord);
		if (nextWord.word != "(")
		{
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"LoadIfSentence(Block format error)");
			return ECompile_ERROR;
		}

		int nBracketSize = 0;
		SentenceSourceCode vCondCode;
		while (true)
		{
			GetWord(nextWord);
			if (nextWord.word == "(")
			{
				nBracketSize++;
			}
			else if (nextWord.word == ")")
			{
				nBracketSize--;
				if (nBracketSize < 0)
				{
					break;
				}
			}
			vCondCode.push_back(nextWord);
		}
		if (LoadOperatiorState(vCondCode, pIfICode, CIfICode::E_COND) == ECompile_ERROR)
		{
			return ECompile_ERROR;
		}

		GetWord(nextWord);
		if (nextWord.word != "{")
		{
			if (LoadOperatiorState(vIn, pIfICode, CIfICode::E_TRUE) == ECompile_ERROR)
			{
				return ECompile_ERROR;
			}
		}
		else
		{
			RevertWord(nextWord);
			if (LoadBlockState(vIn, pIfICode, CIfICode::E_TRUE) == ECompile_ERROR)
			{
				return ECompile_ERROR;
			}
		}
		GetWord(nextWord);
		if (nextWord.word == "else")
		{
			GetWord(nextWord);
			pIfICode->m_unElseSoureIndex = nextWord.nSourceWordsIndex;
			if (nextWord.word == "if")
			{
				RevertWord(nextWord);
				if (LoadIfSentence(vIn, pIfICode, CIfICode::E_FALSE) == ECompile_ERROR)
				{
					return ECompile_ERROR;
				}
			}
			else if (nextWord.word == "{")
			{
				RevertWord(nextWord);
				if (LoadBlockState(vIn, pIfICode, CIfICode::E_FALSE) == ECompile_ERROR)
				{
					return ECompile_ERROR;
				}
			}
			else
			{
				if (LoadOperatiorState(vIn, pIfICode, CIfICode::E_FALSE) == ECompile_ERROR)
				{
					return ECompile_ERROR;
				}
			}

		}
		else
		{
			RevertWord(nextWord);
		}
		return ECompile_Return;
	}
	int CScriptCodeLoader::LoadForSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	{
		SignToPos;
		return 0;
	}
	int CScriptCodeLoader::LoadWhileSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	{
		SignToPos;
		GetNewWord(nextWord);
		CWhileICode* pWhileICode = CICodeMgr::GetInstance()->New<CWhileICode>(nextWord.nSourceWordsIndex);
		if (pWhileICode == nullptr)
		{
			return ECompile_ERROR;
		}
		pCode->AddICode(nType, pWhileICode);


		if (nextWord.word != "while")
		{
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"LoadWhileSentence(Block format error)");
			return ECompile_ERROR;
		}

		GetWord(nextWord);
		if (nextWord.word != "(")
		{
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"LoadWhileSentence(Block format error)");
			return ECompile_ERROR;
		}

		int nBracketSize = 0;
		SentenceSourceCode vCondCode;
		while (true)
		{
			GetWord(nextWord);

			if (nextWord.word == "(")
			{
				nBracketSize++;
			}
			else if (nextWord.word == ")")
			{
				nBracketSize--;
				if (nBracketSize < 0)
				{
					break;
				}
			}
			vCondCode.push_back(nextWord);
		}
		if (LoadOperatiorState(vCondCode, pWhileICode, CWhileICode::E_COND) == ECompile_ERROR)
		{
			return ECompile_ERROR;
		}
		GetWord(nextWord);
		if (nextWord.word != "{")
		{
			if (LoadOperatiorState(vIn, pWhileICode, CWhileICode::E_BLOCK) == ECompile_ERROR)
			{
				return ECompile_ERROR;
			}
		}
		else
		{
			RevertWord(nextWord);
			if (LoadBlockState(vIn, pWhileICode, CWhileICode::E_BLOCK) == ECompile_ERROR)
			{
				return ECompile_ERROR;
			}
		}
		return ECompile_Return;
	}
	int CScriptCodeLoader::LoadSwitchSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	{
		SignToPos;
		return 0;
	}
	int CScriptCodeLoader::LoadReturnSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	{
		SignToPos;
		GetNewWord(nextWord);
		CReturnICode* pReturnICode = CICodeMgr::GetInstance()->New<CReturnICode>(nextWord.nSourceWordsIndex);
		if (pReturnICode == nullptr)
		{
			return ECompile_ERROR;
		}
		pCode->AddICode(nType, pReturnICode);

		if (nextWord.word != "return")
		{
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"LoadReturnSentence:format error");
			return ECompile_ERROR;
		}

		GetWord(nextWord);
		if (nextWord.word == ";")
		{
			return ECompile_Return;
		}
		RevertWord(nextWord);
		//什么都不是，那就可能是算式了
		if (LoadOperatiorState(vIn, pReturnICode, 0) == ECompile_ERROR)
		{
			return ECompile_ERROR;
		}
		return ECompile_Return;
	}

	//int CScriptCodeLoader::LoadOneSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType, std::string endFlag)
	//{
	//	SignToPos;
	//	GetNewWord(nextWord);
	//	CSentenceICode* pSentenceICode = CICodeMgr::GetInstance()->New<CSentenceICode>(nextWord.nSourceWordsIndex);
	//	if (pSentenceICode == nullptr)
	//	{
	//		return ECompile_ERROR;
	//	}
	//	pCode->AddICode(nType, pSentenceICode);


	//	if (nextWord.word == ";")
	//	{
	//		return ECompile_Return;
	//	}

	//	{
	//		std::vector<CodeStyle> vTempCode;
	//		int nReturn = ECompile_ERROR;

	//		{

	//			//什么都不是，那就可能是算式了
	//			nReturn = LoadOperatiorState(vIn, pSentenceICode, 0);
	//		}
	//		if (!endFlag.empty())
	//		{
	//			GetWord(nextWord);
	//			if (nextWord.word != endFlag)
	//			{
	//				AddErrorInfo(
	//					nextWord.nSourceWordsIndex,
	//					"Sentence format error");
	//				return ECompile_ERROR;
	//			}
	//		}
	//		else
	//		{
	//			pSentenceICode->SetClear(false);
	//		}

	//		return nReturn;
	//	}
	//	return ECompile_Return;
	//}
	int CScriptCodeLoader::LoadOperatiorState(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	{
		SignToPos;
		CBaseICode* pOperCode = LoadOperator(vIn, ";");
		if (pOperCode == nullptr)
		{
			AddErrorInfo(
				nBeginSourceWordIndex,
				"Sentence format error");
			return ECompile_ERROR;
		}
		pCode->AddICode(nType, pOperCode);

		return ECompile_Return;
	}
	//int CScriptCodeLoader::LoadCallFunState(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	//{
	//	SignToPos;
	//	GetNewWord(FunName);

	//	CBaseICode* pICode = nullptr;

	//	int nCallFunIndex = CScriptCallBackFunion::GetFunIndex(FunName.word);
	//	if (nCallFunIndex >= 0)
	//	{
	//		auto pCallBackCode = CICodeMgr::GetInstance()->New<CCallBackFunICode>(FunName.nSourceWordsIndex);

	//		pICode = pCallBackCode;
	//	}
	//	else if (m_mapNewString2CodeIndex.find(FunName.word) != m_mapNewString2CodeIndex.end())
	//	{
	//		auto pCallScriptCode = CICodeMgr::GetInstance()->New<CCallScriptFunICode>(FunName.nSourceWordsIndex);


	//		pICode = pCallScriptCode;
	//	}
	//	else
	//	{
	//		strError = "LoadCallFunState(Function call, function not found)";
	//		nErrorWordPos = FunName.nSourceWordsIndex;
	//		return ECompile_ERROR;
	//	}
	//	GetNewWord(nextWord);
	//	if (nextWord.word != "(")
	//	{
	//		nErrorWordPos = nextWord.nSourceWordsIndex;
	//		strError = "LoadCallFunState(Block format error)";
	//		return ECompile_ERROR;
	//	}
	//	//函数参数要倒着取才行
	//	stack<tagCodeSection> sTempCode;
	//	int nParamNum = 0;
	//	while (true)
	//	{
	//		int nBracketSize = 0;
	//		SentenceSourceCode vParmCode;
	//		while (true)
	//		{
	//			GetWord(nextWord);
	//			if (nextWord.word == "(")
	//			{
	//				nBracketSize++;
	//			}
	//			else if (nextWord.word == ")")
	//			{
	//				nBracketSize--;
	//				if (nBracketSize < 0)
	//				{
	//					break;
	//				}
	//			}
	//			else if (nextWord.word == ",")
	//			{
	//				if (nBracketSize <= 0)
	//				{
	//					break;
	//				}
	//			}
	//			vParmCode.push_back(nextWord);
	//		}

	//		if (vParmCode.size() > 0)
	//		{
	//			tagCodeSection vTemp;
	//			if (LoadFormulaSentence(vParmCode, pCode, CCallBackFunICode::E_PARAM) == ECompile_ERROR)
	//			{
	//				return ECompile_ERROR;
	//			}
	//			sTempCode.push(vTemp);
	//			nParamNum++;
	//		}

	//		if (nextWord.word == ")")
	//		{
	//			break;
	//		}

	//	}

	//	return ECompile_Return;
	//}

	//int CScriptCodeLoader::LoadCallClassFun(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	//{
	//	SignToPos;
	//	GetNewWord(FunName);

	//	auto pCallCode = CICodeMgr::GetInstance()->New<CCallClassFunICode>(FunName.nSourceWordsIndex);
	//	pCode->AddICode(nType, pCallCode);


	//	int nClassIndex = QueryTempVar(FunName.word, pCode);
	//	int nClassType = 0;
	//	VarInfo info;
	//	StackVarInfo* pVar = nullptr;
	//	if (nClassIndex >= 0)
	//	{
	//		info = pCode->GetTempVarInfo(FunName.word.c_str());
	//		pVar = &m_vTempCodeData.vNumVar[nClassIndex];
	//		classcode.cSign = 2;
	//		classcode.cExtend = 0;
	//		classcode.dwPos = nClassIndex;
	//	}
	//	else if (m_mapDicGlobalVar.find(FunName.word) != m_mapDicGlobalVar.end())
	//	{
	//		info = m_mapDicGlobalVar[FunName.word];
	//		nClassIndex = info.dwPos;
	//		pVar = &vGlobalNumVar[nClassIndex];
	//		classcode.cSign = 1;
	//		classcode.cExtend = 0;
	//		classcode.dwPos = nClassIndex;
	//	}
	//	if (pVar && pVar->cType == EScriptVal_ClassPoint)
	//	{
	//		callCode.dwPos = info.wExtend;
	//	}
	//	else
	//	{
	//		strError = "LoadCallClassFunn(Non class pointer when calling class function)";
	//		nErrorWordPos = FunName.nSourceWordsIndex;
	//		return ECompile_ERROR;
	//	}
	//	GetNewWord(nextWord);
	//	if (nextWord.word != "->")
	//	{
	//		strError = "LoadCallClassFunn(When calling a class function, the class pointer cannot have an operator other than)";
	//		nErrorWordPos = nextWord.nSourceWordsIndex;
	//		return ECompile_ERROR;
	//	}
	//	GetWord(nextWord);
	//	int nFunIndex = CScriptSuperPointerMgr::GetInstance()->GetClassFunIndex(info.wExtend, nextWord.word);
	//	if (nFunIndex < 0)
	//	{
	//		strError = "LoadCallClassFunn(Class function not registered when calling class function)";
	//		nErrorWordPos = nextWord.nSourceWordsIndex;
	//		return ECompile_ERROR;
	//	}
	//	callCode.dwPos = nFunIndex;

	//	GetWord(nextWord);
	//	if (nextWord.word != "(")
	//	{
	//		strError = "LoadCallClassFunn()";
	//		nErrorWordPos = nextWord.nSourceWordsIndex;
	//		return ECompile_ERROR;
	//	}

	//	stack<tagCodeSection> sTempCode;
	//	int nParamNum = 0;
	//	while (true)
	//	{
	//		int nBracketSize = 0;
	//		SentenceSourceCode vParmCode;
	//		while (true)
	//		{
	//			GetWord(nextWord);
	//			if (nextWord.word == "(")
	//			{
	//				nBracketSize++;
	//			}
	//			else if (nextWord.word == ")")
	//			{
	//				nBracketSize--;
	//				if (nBracketSize < 0)
	//				{
	//					break;
	//				}
	//			}
	//			else if (nextWord.word == ",")
	//			{
	//				if (nBracketSize <= 0)
	//				{
	//					break;
	//				}
	//			}
	//			vParmCode.push_back(nextWord);
	//		}
	//		if (vParmCode.size() > 0)
	//		{
	//			if (LoadFormulaSentence(vParmCode, pCode, vTemp) == ECompile_ERROR)
	//			{
	//				return ECompile_ERROR;
	//			}
	//			sTempCode.push(vTemp);
	//			nParamNum++;
	//		}
	//		if (nextWord.word == ")")
	//		{
	//			break;
	//		}

	//	}

	//	vOut.push_back(begincode);

	//	while (!sTempCode.empty())
	//	{
	//		tagCodeSection& vTemp = sTempCode.top();
	//		for (unsigned int i = 0; i < vTemp.size(); i++)
	//		{
	//			vOut.push_back(vTemp[i]);
	//		}
	//		sTempCode.pop();
	//	}
	//	callCode.cExtend = nParamNum;

	//	vOut.push_back(classcode);
	//	vOut.push_back(callCode);

	//	return ECompile_Return;
	//}

	//int CScriptCodeLoader::LoadFormulaSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	//{
	//	SignToPos;
	//	GetNewWord(nextWord);
	//	if (nextWord.word == "new")
	//	{
	//		GetNewWord(ClassName);
	//		CodeStyle newCode(ClassName.nSourceWordsIndex);
	//		newCode.qwCode = 0;
	//		newCode.wInstruct = ECODE_NEW_CLASS;
	//		newCode.dwPos = CScriptSuperPointerMgr::GetInstance()->GetClassType(ClassName.word);

	//		vOut.push_back(newCode);
	//		return ECompile_Return;
	//	}
	//	RevertWord(nextWord);
	//	//先检查是不是运算符
	//	enum
	//	{
	//		E_NODE_NONE,
	//		E_NODE_SIGN,
	//		E_NODE_VAR,
	//	};
	//	int nLastNodeType = E_NODE_NONE;
	//	struct CBaseNode
	//	{
	//		CBaseNode()
	//		{
	//			pFather = nullptr;
	//			pLeft = nullptr;
	//			pRight = nullptr;
	//		}
	//		virtual int GetType() = 0;
	//		virtual void ReleaseSelf()
	//		{
	//			if (pLeft)
	//			{
	//				pLeft->ReleaseSelf();
	//			}
	//			if (pRight)
	//			{
	//				pRight->ReleaseSelf();
	//			}
	//			delete this;
	//		}
	//		CBaseNode* pFather;
	//		CBaseNode* pLeft;
	//		CBaseNode* pRight;

	//		virtual void addCode(vector<CodeStyle>& vOut)
	//		{
	//			if (pLeft)
	//			{
	//				pLeft->addCode(vOut);
	//			}
	//			if (pRight)
	//			{
	//				pRight->addCode(vOut);
	//			}
	//		}
	//	};
	//	struct CSignNode : public CBaseNode
	//	{
	//		CSignNode(unsigned int unIndex)
	//		{
	//			unSoureIndex = unIndex;
	//		}
	//		int GetType()
	//		{
	//			return E_NODE_SIGN;
	//		}
	//		unsigned int unSoureIndex;
	//		unsigned short unSign;
	//		int nLevel;

	//		void addCode(vector<CodeStyle>& vOut)
	//		{
	//			CBaseNode::addCode(vOut);

	//			CodeStyle code(unSoureIndex);
	//			code.qwCode = 0;
	//			code.wInstruct = unSign;

	//			vOut.push_back(code);
	//		}
	//	};
	//	struct CVarNode : public CBaseNode
	//	{
	//		int GetType()
	//		{
	//			return E_NODE_VAR;
	//		}
	//		vector<CodeStyle> vTempCode;
	//		void addCode(vector<CodeStyle>& vOut)
	//		{
	//			CBaseNode::addCode(vOut);

	//			for (size_t i = 0; i < vTempCode.size(); i++)
	//			{
	//				vOut.push_back(vTempCode[i]);
	//			}
	//		}
	//	};
	//	CBaseNode* pRootNode = nullptr;
	//	int nResult = ECompile_Return;
	//	int nState = 0;
	//	while (true)
	//	{
	//		if (vIn.empty())
	//		{
	//			break;
	//		}
	//		GetNewWord(nextWord);

	//		CBaseNode* pNode = nullptr;
	//		if (nextWord.word == ";")
	//		{
	//			RevertWord(nextWord);
	//			break;
	//		}
	//		else if (nextWord.word == "(")
	//		{
	//			nState = 1;
	//			RevertWord(nextWord);
	//			CVarNode* pVarNode = new CVarNode;
	//			//unsigned int nTempPos = curPos;
	//			if (LoadBracket(vIn, pCode, vOut) == ECompile_ERROR)
	//			{
	//				nResult = ECompile_ERROR;
	//				break;
	//			}
	//			nLastNodeType = E_NODE_VAR;
	//			pNode = pVarNode;
	//		}
	//		else if (m_mapDicSignToPRI.find(nextWord.word) == m_mapDicSignToPRI.end())
	//		{
	//			if (nState == 1)
	//			{
	//				nResult = ECompile_Return;
	//				break;
	//			}
	//			nState = 1;
	//			CVarNode* pVarNode = new CVarNode;
	//			nLastNodeType = E_NODE_VAR;

	//			RevertWord(nextWord);
	//			if (LoadAndPushNumVar(vIn, pCode, pVarNode->vTempCode) == ECompile_ERROR)
	//			{
	//				//生成压值进堆栈的代码失败
	//				nResult = ECompile_ERROR;
	//				break;
	//			}
	//			//}
	//			pNode = pVarNode;
	//		}
	//		else
	//		{
	//			if (nState == 2)
	//			{
	//				nResult = ECompile_Return;
	//				break;
	//			}
	//			nState = 2;
	//			CSignNode* pSignNode = new CSignNode(nextWord.nSourceWordsIndex);
	//			if (nLastNodeType != E_NODE_VAR)
	//			{
	//				pSignNode->nLevel = 0;
	//				pSignNode->unSign = m_mapDicSingleSignToEnum[nextWord.word];
	//			}
	//			else
	//			{
	//				pSignNode->nLevel = m_mapDicSignToPRI[nextWord.word];
	//				pSignNode->unSign = m_mapDicSignToEnum[nextWord.word];
	//			}
	//			nLastNodeType = E_NODE_SIGN;
	//			pNode = pSignNode;
	//		}
	//		if (pRootNode == nullptr)
	//		{
	//			pRootNode = pNode;
	//		}
	//		else
	//		{
	//			if (pNode->GetType() == E_NODE_VAR)
	//			{
	//				//放在最右侧
	//				CBaseNode* pNowNode = pRootNode;

	//				while (pNowNode)
	//				{
	//					if (pNowNode->pRight == nullptr)
	//					{
	//						pNowNode->pRight = pNode;
	//						pNode->pFather = pNowNode;
	//						break;
	//					}
	//					pNowNode = pNowNode->pRight;
	//				}
	//			}
	//			else if (pNode->GetType() == E_NODE_SIGN)
	//			{

	//				if (pRootNode->GetType() == E_NODE_VAR)
	//				{
	//					pRootNode->pFather = pNode;
	//					pNode->pLeft = pRootNode;
	//					pRootNode = pNode;
	//				}
	//				else
	//				{
	//					CBaseNode* pNowNode = pRootNode;
	//					while (pNowNode)
	//					{
	//						if (pNowNode->GetType() == E_NODE_SIGN)
	//						{
	//							if (((CSignNode*)pNowNode)->nLevel >= ((CSignNode*)pNode)->nLevel)
	//							{
	//								pNode->pLeft = pNowNode;
	//								pNode->pFather = pNowNode->pFather;
	//								pNowNode->pFather = pNode;
	//								if (pNowNode == pRootNode)
	//								{
	//									pRootNode = pNode;
	//								}
	//								break;
	//							}
	//						}
	//						else if (pNowNode->GetType() == E_NODE_VAR)
	//						{
	//							if (pNowNode->pFather)
	//							{
	//								pNowNode->pFather->pRight = pNode;
	//							}
	//							pNode->pFather = pNowNode->pFather;
	//							pNowNode->pFather = pNode;
	//							pNode->pLeft = pNowNode;
	//						}
	//						else if (pNowNode->pRight == nullptr)
	//						{
	//							pNowNode->pRight = pNode;
	//							pNode->pFather = pNowNode;
	//						}
	//						pNowNode = pNowNode->pRight;
	//					}
	//				}

	//			}
	//		}
	//	}

	//	if (nResult != ECompile_ERROR)
	//	{
	//		if (pRootNode)
	//		{
	//			pRootNode->addCode(vOut);
	//		}
	//	}
	//	//释放二叉树
	//	if (pRootNode)
	//	{
	//		pRootNode->ReleaseSelf();
	//	}
	//	return nResult;
	//}

	//int CScriptCodeLoader::LoadAndPushNumVar(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	//{
	//	SignToPos;
	//	GetNewWord(varName);
	//	if (vIn.size() > 0)
	//	{
	//		GetNewWord(nextWord);
	//		if (nextWord.word == "->")
	//		{
	//			RevertWord(nextWord);
	//			RevertWord(varName);
	//			if (LoadCallClassFun(vIn, pCode, vOut) == ECompile_ERROR)
	//			{
	//				return ECompile_ERROR;
	//			}
	//			return ECompile_Return;
	//		}
	//		else if (nextWord.word == "(")
	//		{
	//			RevertWord(nextWord);
	//			//脚本函数
	//			if (m_mapNewString2CodeIndex.find(varName.word) != m_mapNewString2CodeIndex.end())
	//			{
	//				RevertWord(varName);
	//				if (LoadCallFunState(vIn, pCode, vOut) == ECompile_ERROR)
	//				{
	//					return ECompile_ERROR;
	//				}
	//			}
	//			//回调函数
	//			else if (CScriptCallBackFunion::GetFunIndex(varName.word) >= 0)
	//			{
	//				RevertWord(varName);
	//				if (LoadCallFunState(vIn, pCode, vOut) == ECompile_ERROR)
	//				{
	//					return ECompile_ERROR;
	//				}
	//			}
	//			else
	//			{
	//				//TODO:检查函数调用语句是否完整，定义新函数
	//				m_mapString2CodeIndex[varName.word] = m_vecCodeData.size();
	//				tagCodeData data;
	//				data.funname = varName.word;
	//				data.nType = EICODE_FUN_NO_CODE;
	//				m_vecCodeData.push_back(data);
	//				//然后再读取
	//				RevertWord(varName);
	//				if (LoadCallFunState(vIn, pCode, vOut) == ECompile_ERROR)
	//				{
	//					return ECompile_ERROR;
	//				}
	//			}
	//			return ECompile_Return;
	//		}
	//		RevertWord(nextWord);
	//	}
	//	//if (vIn.empty())
	//	{
	//		if (varName.nFlag == E_WORD_FLAG_STRING)
	//		{
	//			//字符串
	//			CodeStyle code(varName.nSourceWordsIndex);
	//			m_vTempCodeData.vStrConst.push_back(varName.word);
	//			code.wInstruct = ECODE_PUSH;
	//			code.cSign = 3;
	//			code.cExtend = 0;
	//			code.dwPos = m_vTempCodeData.vStrConst.size() - 1;
	//			vOut.push_back(code);
	//		}
	//		else
	//		{
	//			CodeStyle code(varName.nSourceWordsIndex);
	//			code.qwCode = 0;
	//			int nIndexVar = QueryTempVar(varName.word, pCode);
	//			if (nIndexVar >= 0)//临时变量
	//			{
	//				code.wInstruct = ECODE_PUSH;
	//				code.cSign = 2;
	//				code.cExtend = 0;
	//				code.dwPos = nIndexVar;
	//				vOut.push_back(code);
	//			}
	//			else if (m_mapDicGlobalVar.find(varName.word) != m_mapDicGlobalVar.end())//全局变量
	//			{
	//				code.wInstruct = ECODE_PUSH;
	//				code.cSign = 1;
	//				code.cExtend = 0;
	//				VarInfo info = m_mapDicGlobalVar[varName.word];
	//				code.dwPos = info.dwPos;
	//				vOut.push_back(code);
	//			}
	//			else
	//			{
	//				bool isStr = false;
	//				bool isFloat = false;
	//				for (unsigned int i = 0; i < varName.word.size(); i++)
	//				{
	//					if ((varName.word[i] >= '0' && varName.word[i] <= '9'))
	//					{
	//						//是数字
	//					}
	//					else if (varName.word[i] == '.')
	//					{
	//						//是点
	//						isFloat = true;
	//					}
	//					else
	//					{
	//						isStr = true;
	//						break;
	//					}
	//				}
	//				if (isFloat)
	//				{
	//					m_vTempCodeData.vFloatConst.push_back(stod(varName.word.c_str()));
	//					code.wInstruct = ECODE_PUSH;
	//					code.cSign = 4;
	//					code.cExtend = 1;
	//					code.dwPos = m_vTempCodeData.vFloatConst.size() - 1;
	//					vOut.push_back(code);
	//				}
	//				else
	//				{
	//					code.wInstruct = ECODE_PUSH;
	//					code.cSign = 0;
	//					code.cExtend = 0;
	//					code.dwPos = _atoi64(varName.word.c_str());
	//					vOut.push_back(code);
	//				}
	//			}
	//		}
	//	}

	//	return ECompile_Return;
	//}

	//int CScriptCodeLoader::LoadBracket(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	//{
	//	SignToPos;
	//	GetNewWord(nextWord);
	//	if (nextWord.word != "(")
	//	{
	//		strError = "LoadBracket(Wrong definition format in parentheses)";
	//		nErrorWordPos = nextWord.nSourceWordsIndex;
	//		return ECompile_ERROR;
	//	}
	//	GetWord(nextWord);
	//	if (nextWord.word == ")")
	//	{
	//		return ECompile_Return;
	//	}

	//	if (LoadFormulaSentence(vIn, pCode, 0) == ECompile_ERROR)
	//	{
	//		return ECompile_ERROR;
	//	}
	//	GetWord(nextWord);
	//	//结尾没有括号，格式错误
	//	if (nextWord.word != ")")
	//	{
	//		strError = "LoadBracket(Wrong definition format in parentheses)";
	//		nErrorWordPos = nextWord.nSourceWordsIndex;
	//		return ECompile_ERROR;
	//	}

	//	return ECompile_Return;
	//}

	CBaseICode* CScriptCodeLoader::LoadOperator(SentenceSourceCode& vIn, std::string endFlag)
	{
		SignToPos;
		COperatorICode* pRootCode = nullptr;
		COperatorICode* pCurNode = nullptr;
		CBaseICode* pOperandCode = nullptr;
		std::vector<CBaseICode*> vOperand;
		//int nCanOperatorFlag = E_OPERATOR_LEFT | E_OPERATOR_LEFT_AND_RIGHT | E_OPERATOR_OPERAND;
		while (vIn.size() > 0)
		{
			CBaseICode* pCode = nullptr;
			GetNewWord2(NextWord);
			if (CheckOperator(NextWord.word))
			{
				auto pOperCode = CICodeMgr::GetInstance()->New<COperatorICode>(NextWord.nSourceWordsIndex);
				pOperCode->strOperator = NextWord.word;
				pOperCode->nPriorityLv = m_mapDicSignToPRI[NextWord.word];
				//pOperCode->nOperatorFlag = m_mapDicSignToEnum[NextWord.word];
				pCode = pOperCode;
			}
			else if (NextWord.word == "(")
			{
				pCode = LoadOperator(vIn, ")");
				GetWord2(NextWord);
			}
			else if (NextWord.word == endFlag)
			{
				break;
			}
			else
			{
				//检查关键字
				if (m_mapDic2KeyWord.find(NextWord.word) != m_mapDic2KeyWord.end())
				{
					RevertWord(NextWord);
					break;
				}
				else if (NextWord.word == ";")
				{
					RevertWord(NextWord);
					break;
				}
				else if (NextWord.word == ",")
				{
					RevertWord(NextWord);
					break;
				}
				else if (NextWord.word == ")")
				{
					RevertWord(NextWord);
					break;
				}
				tagSourceWord operandWord = NextWord;
				if (vIn.size() > 0)
				{
					GetWord2(NextWord);
					if (NextWord.word == "(")
					{
						RevertWord(NextWord);
						RevertWord(operandWord);
						pCode = LoadCallFun(vIn);
					}
					else
					{
						RevertWord(NextWord);
						pCode = LoadOperand(operandWord);
					}
				}
				else
				{
					pCode = LoadOperand(operandWord);
				}
			}
			if (pCode == nullptr)
			{
				return nullptr;
			}
			if (pCode->GetType() == E_I_CODE_OPERATOR)
			{
				//将新的操作符插入二叉树中
				COperatorICode* pCurOperCode = (COperatorICode*)pCode;
				if (pCurNode == nullptr)
				{
					pRootCode = pCurOperCode;
					pCurNode = pCurOperCode;
					if (pOperandCode)
					{
						pCurOperCode->AddICode(COperatorICode::E_LEFT_OPERAND, pOperandCode);
						pOperandCode = nullptr;
					}
				}
				else
				{
					while (pCurNode)
					{
						if (pCurNode->nPriorityLv == pCurOperCode->nPriorityLv)
						{
							if (pCurNode == pRootCode)
							{
								pRootCode = pCurOperCode;
							}
							else
							{
								COperatorICode* pFather = dynamic_cast<COperatorICode*>(pCurNode->GetFather());
								if (pFather)
								{
									pFather->AddICode(COperatorICode::E_RIGHT_OPERAND, pCurOperCode);
								}
							}
							pCurOperCode->AddICode(COperatorICode::E_LEFT_OPERAND, pCurNode);
							pCurNode = pCurOperCode;
							break;
						}
						else if (pCurNode->nPriorityLv < pCurOperCode->nPriorityLv)
						{
							auto pOperand = pCurNode->GetICode(COperatorICode::E_RIGHT_OPERAND, 0);
							pCurOperCode->AddICode(COperatorICode::E_LEFT_OPERAND, pOperand);
							pCurNode->AddICode(COperatorICode::E_RIGHT_OPERAND, pCurOperCode);
							pCurNode = pCurOperCode;
							break;
						}
						else if (pCurNode->nPriorityLv > pCurOperCode->nPriorityLv)
						{
							if (pCurNode == pRootCode)
							{
								pRootCode = pCurOperCode;
								pCurOperCode->AddICode(COperatorICode::E_LEFT_OPERAND, pCurNode);
								pCurNode = pCurOperCode;
								break;
							}
							else
							{
								pCurNode = dynamic_cast<COperatorICode*>(pCurNode->GetFather());
								if (pCurNode == nullptr)
								{
									//TODO 报错
									AddErrorInfo(
										NextWord.nSourceWordsIndex,
										"Operator format error: no father");
									return nullptr;
								}
							}
						}
					}
				}
			}
			else
			{
				//获得的是个参数
				if (pCurNode == nullptr)
				{
					//还没又读到操作符，暂时保存下来
					if (pOperandCode == nullptr)
					{
						pOperandCode = pCode;
					}
					else
					{
						AddErrorInfo(
							NextWord.nSourceWordsIndex,
							"Operator format error: move operand");
						return nullptr;
					}
				}
				else
				{
					pCurNode->AddICode(COperatorICode::E_RIGHT_OPERAND, pCode);
				}
			}
		}
		if (pRootCode)
		{
			//上面只是初步列成二叉树，接下要对树进行整理
			if (CheckOperatorTree((CBaseICode**)&pRootCode) == false)
			{
				AddErrorInfo(
					pRootCode->m_unBeginSoureIndex,
					"Operator format Check error");
				return nullptr;
			}
			return pRootCode;
		}

		return pOperandCode;
	}

	CBaseICode* CScriptCodeLoader::LoadCallFun(SentenceSourceCode& vIn)
	{
		SignToPos;
		GetNewWord2(nextWord);
		CCallFunICode* pReturnICode = CICodeMgr::GetInstance()->New<CCallFunICode>(nextWord.nSourceWordsIndex);
		pReturnICode->strFunName = nextWord.word;
		GetWord2(nextWord);
		if (nextWord.word != "(")
		{
			AddErrorInfo(
				nextWord.nSourceWordsIndex,
				"call fun format error: (");
			return nullptr;
		}
		while (true)
		{
			auto *pCode = LoadOperator(vIn);
			if (pCode)
			{
				pReturnICode->AddICode(CCallFunICode::E_PARAM, pCode);
			}
			//else
			//{
			//	GetWord2(nextWord);
			//	if (nextWord.word == ")")
			//	{
			//		break;
			//	}
			//	return nullptr;
			//}
			GetWord2(nextWord);
			if (nextWord.word == ")")
			{
				break;
			}
			else if (pCode && nextWord.word == ",")
			{
				continue;
			}
			else
			{
				AddErrorInfo(
					nextWord.nSourceWordsIndex,
					"call fun format error: param");
				return nullptr;
			}
		}
		return pReturnICode;
	}

	CBaseICode* CScriptCodeLoader::LoadOperand(tagSourceWord& word)
	{
		CLoadVarICode* pICode = CICodeMgr::GetInstance()->New<CLoadVarICode>(word.nSourceWordsIndex);
		pICode->m_word = word;
		return pICode;
	}

	bool CScriptCodeLoader::CheckOperatorTree(CBaseICode** pNode)
	{
		if (pNode == nullptr)
			return false;
		COperatorICode* pOperNode = dynamic_cast<COperatorICode*>(*pNode);
		if (pOperNode == nullptr)
			return true;

		if (pOperNode->strOperator == "new")
		{
			if (pOperNode->pLeftOperand == nullptr && pOperNode->pRightOperand && pOperNode->pRightOperand->GetType() == E_I_CODE_OPERAND)
			{
				CLoadVarICode* pVarCode = (CLoadVarICode*)pOperNode->pRightOperand;
				CNewICode* pNewICode = CICodeMgr::GetInstance()->New<CNewICode>(pOperNode->m_unBeginSoureIndex);
				pNewICode->strClassType = pVarCode->m_word.word;
				pNewICode->SetFather(pOperNode->GetFather());
				*pNode = pNewICode;
				return true;
			}
			return false;
		}
		else if(pOperNode->strOperator == "-")
		{
			if (pOperNode->pLeftOperand == nullptr && pOperNode->pRightOperand)
			{
				CMinusICode* pMinusICode = CICodeMgr::GetInstance()->New<CMinusICode>(pOperNode->m_unBeginSoureIndex);
				if (CheckOperatorTree(&pOperNode->pRightOperand) == false)
				{
					return false;
				}
				pMinusICode->AddICode(0,pOperNode->pRightOperand);
				pMinusICode->SetFather(pOperNode->GetFather());
				*pNode = pMinusICode;
				return true;
			}
			//return false;
		}
		else if (pOperNode->strOperator == "=")
		{
			COperatorICode* pLeftCode = dynamic_cast<COperatorICode*>(pOperNode->pLeftOperand);
			if (pLeftCode && pLeftCode->strOperator == "->")
			{
				if (pLeftCode->pLeftOperand && pLeftCode->pRightOperand &&
					pLeftCode->pLeftOperand->GetType() == E_I_CODE_OPERAND&& 
					pLeftCode->pRightOperand->GetType() == E_I_CODE_OPERAND)
				{
					CLoadVarICode* pLeftOperand = (CLoadVarICode*)pLeftCode->pLeftOperand;
					CLoadVarICode* pRightOperand = (CLoadVarICode*)pLeftCode->pRightOperand;
					//设置类变量值
					CSetClassParamICode* pSetCode = CICodeMgr::GetInstance()->New<CSetClassParamICode>(pOperNode->m_unBeginSoureIndex);
					pSetCode->strClassVarName = pLeftOperand->m_word.word;
					pSetCode->strParamName = pRightOperand->m_word.word;

					if (CheckOperatorTree(&pOperNode->pRightOperand) == false)
					{
						return false;
					}
					pSetCode->AddICode(0,pOperNode->pRightOperand);
					pSetCode->SetFather(pOperNode->GetFather());
					*pNode = pSetCode;
					return true;
				}
				else
				{
					return false;
				}
			}
			else if (pOperNode->pLeftOperand && pOperNode->pLeftOperand->GetType() == E_I_CODE_OPERAND && pOperNode->pRightOperand)
			{
				CSaveVarICode *pSaveCode = CICodeMgr::GetInstance()->New<CSaveVarICode>(pOperNode->m_unBeginSoureIndex);
				CLoadVarICode* pLeftOperand = (CLoadVarICode*)pOperNode->pLeftOperand;
				pSaveCode->m_word = pLeftOperand->m_word;
				if (CheckOperatorTree(&pOperNode->pRightOperand) == false)
				{
					return false;
				}
				pSaveCode->AddICode(0, pOperNode->pRightOperand);
				pSaveCode->SetFather(pOperNode->GetFather());
				*pNode = pSaveCode;
				return true;
			}
		}
		else if (pOperNode->strOperator == "->")
		{
			if (pOperNode->pLeftOperand && pOperNode->pRightOperand && pOperNode->pLeftOperand->GetType() == E_I_CODE_OPERAND)
			{
				CLoadVarICode* pLeftOperand = (CLoadVarICode*)pOperNode->pLeftOperand;
				if (pOperNode->pRightOperand->GetType() == E_I_CODE_OPERAND)
				{
					CLoadVarICode* pRightOperand = (CLoadVarICode*)pOperNode->pRightOperand;
					//获取类变量值
					CGetClassParamICode* pGetCode = CICodeMgr::GetInstance()->New<CGetClassParamICode>(pOperNode->m_unBeginSoureIndex);
					pGetCode->strClassVarName = pLeftOperand->m_word.word;
					pGetCode->strParamName = pRightOperand->m_word.word;
					pGetCode->SetFather(pOperNode->GetFather());
					*pNode = pGetCode;
					return true;
				}
				else if (pOperNode->pRightOperand->GetType() == E_I_CODE_CALL)
				{
					CCallFunICode* pRightOperand = (CCallFunICode*)pOperNode->pRightOperand;
					CCallClassFunICode* pCallCode = CICodeMgr::GetInstance()->New<CCallClassFunICode>(pOperNode->m_unBeginSoureIndex);
					pCallCode->strClassVarName = pLeftOperand->m_word.word;
					pCallCode->strFunName = pRightOperand->strFunName;
					pCallCode->vParams = pRightOperand->vParams;
					for (unsigned int i = 0; i < pCallCode->vParams.size(); i++)
					{
						if (pCallCode->vParams[i])
							pCallCode->vParams[i]->SetFather(pCallCode);
					}
					pCallCode->SetFather(pOperNode->GetFather());
					*pNode = pCallCode;
					return true;
				}
			}
			return false;
		}

		if (pOperNode->pLeftOperand && pOperNode->pLeftOperand->GetType() == E_I_CODE_OPERATOR)
		{
			if (CheckOperatorTree(&pOperNode->pLeftOperand) == false)
			{
				return false;
			}
		}
		if (pOperNode->pRightOperand && pOperNode->pRightOperand->GetType() == E_I_CODE_OPERATOR)
		{
			if (CheckOperatorTree(&pOperNode->pRightOperand) == false)
			{
				return false;
			}
		}
		return true;
	}

	unsigned int CScriptCodeLoader::GetCodeIndex(const char* pStr)
	{
		auto it = m_mapString2CodeIndex.find(pStr);
		if (it == m_mapString2CodeIndex.end())
		{
			return -1;
		}
		return it->second;
	}

	VarInfo* CScriptCodeLoader::GetGlobalVarInfo(std::string name)
	{
		auto it = m_mapDicGlobalVar.find(name);
		if (it != m_mapDicGlobalVar.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	void CScriptCodeLoader::GetGlobalVar(vector<StackVarInfo>& vOut)
	{
		vOut = vGlobalNumVar;
	}
	CScriptCodeLoader::tagCodeData* CScriptCodeLoader::GetCode(const char* pName)
	{
		auto it = m_mapString2CodeIndex.find(pName);
		if (it == m_mapString2CodeIndex.end())
		{
			return nullptr;
		}
		return GetCode(it->second);
	}
	void CScriptCodeLoader::LoadXml(std::string filename)
	{
		//_finddata_t file;
		//long lf;

		//string strbuff = dir + "*.*";
		//if((lf = _findfirst(strbuff.c_str(), &file))==-1l)
		//{

		//}
		//else
		//{
		//	while ( _findnext( lf, &file ) == 0)
		//	{
		//		if(file.attrib == _A_SUBDIR)
		//		{
		//			if (strcmp(file.name,"..") == 0)
		//			{
		//				continue;
		//			}
		//			string newdir = dir + file.name + "\\";
		//			LoadXml(newdir);
		//		}
		//		else
		//		{
		//			string filename = dir + file.name;
		//			LoadFile(filename.c_str());
		//		}
		//	}
		//}
		//_findclose(lf);
		//std::string fullfile = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(filename);
		//std::string fullfile = cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath();;
		//fullfile += filename;
		//tinyxml2::XMLDocument xmldoc;
		//if (xmldoc.LoadFile(fullfile.c_str()))
		//{
		//	return ;
		//}

		//LoadFormBin();
#if 0
		std::string fullfile = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(filename);
		cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(fullfile);
		tinyxml2::XMLDocument xmldoc;
		if (xmldoc.Parse((const char*)data.getBytes(), data.getSize()) != tinyxml2::XML_NO_ERROR)
		{
			return;
		}

		tinyxml2::XMLElement* pRoot = xmldoc.RootElement();
		if (pRoot == nullptr)
		{
			return;
		}
		tinyxml2::XMLElement* pRoleElem = pRoot->FirstChildElement("script");
		for (; pRoleElem; pRoleElem = pRoleElem->NextSiblingElement("script"))
		{
			const char* pTemp = pRoleElem->Attribute("name");
			if (pTemp)
			{
				//std::string str = "script//";
				//str += pTemp;
				//LoadFile(str.c_str());
				LoadFile(pTemp);
			}
		}
		SaveToBin();
#endif
	}


	int CScriptCodeLoader::MakeICode2Code(int nMode)
	{
		//先遍历一遍注册信息
		auto it = m_mapString2Code.begin();
		for (; it != m_mapString2Code.end(); it++)
		{
			CFunICode* pCode = it->second;
			if (pCode)
			{
				tagCodeData code;
				code.funname = pCode->funname;
				code.filename = pCode->filename;
				m_mapString2CodeIndex[it->first] = m_vecCodeData.size();
				m_vecCodeData.push_back(code);
			}
		}
		it = m_mapString2Code.begin();
		for (; it != m_mapString2Code.end(); it++)
		{
			CFunICode* pCode = it->second;
			if (pCode)
			{
				auto itS2I = m_mapString2CodeIndex.find(it->first);
				if (itS2I != m_mapString2CodeIndex.end())
				{
					tagCodeData& code = m_vecCodeData[itS2I->second];
					pCode->MakeExeCode(code);
				}
				else
				{
					//TODO 报错
				}
			}
		}
		return 0;
	}

	void CScriptCodeLoader::ClearICode()
	{
		m_mapString2Code.clear();
		CICodeMgr::GetInstance()->Clear();
	}

	void CScriptCodeLoader::PrintAllCode(const char* pFilename)
	{
		FILE* fp = fopen(pFilename, "wb");
		if (fp == nullptr)
		{
			return;
		}
		for (auto it = m_vecCodeData.begin(); it != m_vecCodeData.end(); it++)
		{
			tagCodeData& data = *it;
			fputs("********************************\n", fp);
			fputs(data.funname.c_str(), fp);
			fputc('\n', fp);
			fputs("********************************\n", fp);
			unsigned int curSoureWordIndex = -1;
			for (unsigned int i = 0; i < data.vCodeData.size(); i++)
			{

				std::string str = PrintOneCode(data.vCodeData[i]);
				char strbuff[32] = { 0 };
				sprintf(strbuff, "[%d]\t", i);
				fputs(strbuff, fp);
				fputs(str.c_str(), fp);
#ifdef _SCRIPT_DEBUG
				if (curSoureWordIndex != data.vCodeData[i].nSoureWordIndex)
				{
					curSoureWordIndex = data.vCodeData[i].nSoureWordIndex;
					auto souceInfo = GetSourceWords(curSoureWordIndex);
					fputc('\t', fp);
					fputs(souceInfo.strLineWords.c_str(), fp);
				}
#endif
				fputc('\n', fp);
			}
		}
		fclose(fp);
	}

	std::string CScriptCodeLoader::PrintOneCode(CodeStyle code)
	{
		char strbuff[128] = { 0 };
		std::string str;
		switch (code.wInstruct)
		{
		case ECODE_ADD: //加
			sprintf(strbuff, "ADD\tsign:%d\textend:%d\tpos:%d",(int)code.cSign,(int)code.cExtend,(int)code.dwPos);
			break;
		case ECODE_SUM: //减
			sprintf(strbuff, "SUM\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_MUL: //乘
			sprintf(strbuff, "MUL\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_DIV://除
			sprintf(strbuff, "DIV\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_MOD: //求余
			sprintf(strbuff, "MOD\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_MINUS: //	取负数
			sprintf(strbuff, "MINUS\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_CMP_EQUAL://比较
			sprintf(strbuff, "CMP(==)\sign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_CMP_NOTEQUAL:
			sprintf(strbuff, "CMP(!=)\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_CMP_BIG:
			sprintf(strbuff, "CMP(>)\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_CMP_BIGANDEQUAL:
			sprintf(strbuff, "CMP(>=)\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_CMP_LESS:
			sprintf(strbuff, "CMP(<)\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_CMP_LESSANDEQUAL:
			sprintf(strbuff, "CMP(<=)\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				//位运算
		case ECODE_BIT_AND:
			sprintf(strbuff, "AND\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_BIT_OR:
			sprintf(strbuff, "OR\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_BIT_XOR:
			sprintf(strbuff, "XOR\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				/*************功能符************/
				// 压入变量到堆栈。
				//	cSign:	使用ESignType的定义.变量来源
				//	dwPos:	根据cSign的值表示值或地址
		case ECODE_PUSH:
			sprintf(strbuff, "PUSH\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				// 提取堆栈中的变量。
				//	cSign:	使用ESignType的定义，变量去处
				//	cExtend:寄存器索引ERegisterIndex
				//	dwPos:	根据cSign的值表示值或地址
		case ECODE_POP:
			sprintf(strbuff, "POP\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_STATEMENT_END: //语句结束
			sprintf(strbuff, "STATEND\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				//读取变量到寄存器。
				//	cSign:	使用ESignType的定义
				//	cExtend:寄存器索引ERegisterIndex
				//	dwPos:	根据cSign的值表示值或地址
		case ECODE_LOAD:
			sprintf(strbuff, "LOAD\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				// 移动寄存器的值
				//	cSign:	目的地类型
				//	cExtend:起点，寄存器索引ERegisterIndex
				//	dwPos:	终点，根据cSign的值表示值或地址
		case ECODE_MOVE:
			sprintf(strbuff, "MOVE\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				//读取类成员变量到寄存器。
				//	cSign:	类对象所在寄存器 ERegisterIndex
				//	cExtend:结果放入的寄存器 ERegisterIndex
				//	dwPos:	类成员变量的索引
		case ECODE_GET_CLASS_PARAM:
			sprintf(strbuff, "GET PARAM\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				// 设置寄存器内容到类成员变量。
				//	cSign:	类对象所在寄存器 ERegisterIndex
				//	cExtend:数值所在的寄存器 ERegisterIndex
				//	dwPos:	类成员变量的索引
		case ECODE_SET_CLASS_PARAM:
			sprintf(strbuff, "SET PARAM\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				//ECODE_JMP,//无条件跳转
				//ECODE_JMP_JUDGE,//m_JudgeRegister为真跳转

				//ECODE_BEGIN_CALL,//开始计算本次函数调用实际压入多少个参数
				//调用回调函数
				//cSign:0,返回值存放的寄存器 ERegisterIndex
				//cExtend:参数数量
				//dwPos:函数索引
		case ECODE_CALL_CALLBACK:
			sprintf(strbuff, "CALL BACK\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				//调用脚本函数 
				//cSign:0,返回值存放的寄存器 ERegisterIndex
				//cExtend:参数数量
				//dwPos:函数索引
		case ECODE_CALL_SCRIPT:
			sprintf(strbuff, "CALL\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				// 跳转
				//cSign:0,绝对地址跳转
				//		1,相对地址向后跳转
				//		2,相对地址向前跳转
				//dwPos:地址值
		case ECODE_JUMP:
			sprintf(strbuff, "JUMP\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				// 寄存器值为真跳转
				//cSign:0,绝对地址跳转
				//		1,相对地址向后跳转
				//		2,相对地址向前跳转
				//cExtend:寄存器索引 ERegisterIndex
				//dwPos:地址值
		case ECODE_JUMP_TRUE:
			sprintf(strbuff, "JUMP_T\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				// 寄存器值为假跳转
				//cSign:0,绝对地址跳转
				//		1,相对地址向后跳转
				//		2,相对地址向前跳转
				//cExtend:寄存器索引 ERegisterIndex
				//dwPos:地址值
		case ECODE_JUMP_FALSE:
			sprintf(strbuff, "JUMP_F\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				// if分支,检查指定寄存器上的值，如果非0则执行接下来的块
				//cSign:0,判断值存放的寄存器 ERegisterIndex
				//dwPos:本分支下执行的代码块大小
				//ECODE_BRANCH_IF,
				//ECODE_BRANCH_JUMP,
				//ECODE_BRANCH_ELSE,	// else分支，如果之前的if没有执行，则执行
				//ECODE_CYC_IF,		//专门用于循环条件的判断
				//ECODE_BLOCK,	//块开始标志 cSign: cExtend: dwPos:表示块大小
				//ECODE_BLOCK_CYC,//放在块尾，执行到此返回块头
				//ECODE_BREAK,	//退出此块
				// 退出函数
				//	cExtend:返回值所在寄存器索引ERegisterIndex
		case ECODE_RETURN:	//退出函数
			sprintf(strbuff, "RETURN\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_CLEAR_PARAM://清空堆栈里的参数
			sprintf(strbuff, "CLEAR\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				//调用类函数 
				// cSign:	类对象所在寄存器，返回值也会写入在此
				// cExtend:	参数数量,
				// dwPos:	类函数索引
		case ECODE_CALL_CLASS_FUN:
			sprintf(strbuff, "CALL CLASS\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				// 新建一个类实例
				//cSign:结果放入寄存器索引
				//dwPos:类类型Index
		case ECODE_NEW_CLASS: //新建一个类实例
			sprintf(strbuff, "NEW\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				// 释放一个类实例
				//cSign:使用ESignType的定义.变量来源
				//dwPos:根据cSign的值获取类指针所在的地址
		case ECODE_RELEASE_CLASS://释放一个类实例
			sprintf(strbuff, "RELERASE\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
				//下面是中间代码
		case ECODE_BREAK:
			sprintf(strbuff, "BREAK\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		case ECODE_CONTINUE:
			sprintf(strbuff, "CONTINUE\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
			break;
		}

		return strbuff;
	}

	void CScriptCodeLoader::SaveToBin()
	{
#if 0
		//#ifdef WIN32
				//std::string fullfile = cocos2d::CCFileUtils::getInstance()->fullPathForFilename("script.bin");
		FILE* fp = fopen("script.bin", "wb");
		if (fp)
		{
			tagByteArray vBuff;
			//版本
			AddInt2Bytes(vBuff, 0);

			//std::vector<VarPoint> vGlobalNumVar;//变量
			AddInt2Bytes(vBuff, vGlobalNumVar.size());
			for (unsigned int i = 0; i < vGlobalNumVar.size(); i++)
			{
				VarPoint& var = vGlobalNumVar[i];
				AddChar2Bytes(vBuff, var.cType);
				AddUShort2Bytes(vBuff, var.unArraySize);

				switch (var.cType)
				{
				case EScriptVal_Int:
				{
					for (unsigned short r = 0; r < var.unArraySize; r++)
						AddInt642Bytes(vBuff, var.pInt64[r]);
				}
				break;
				case EScriptVal_Double:
				{
					for (unsigned short r = 0; r < var.unArraySize; r++)
						AddDouble2Bytes(vBuff, var.pDouble[r]);
				}
				break;
				case EScriptVal_String:
				{
					for (unsigned short r = 0; r < var.unArraySize; r++)
						AddChar2Bytes(vBuff, var.pStr[r]);
				}
				break;
				}
			}

			//std::vector<tagCodeData> m_vecCodeData;
			AddInt2Bytes(vBuff, m_vecCodeData.size());
			for (unsigned int i = 0; i < m_vecCodeData.size(); i++)
			{
				tagCodeData& code = m_vecCodeData[i];
				AddInt2Bytes(vBuff, code.vStrConst.size());
				for (unsigned int strIndex = 0; strIndex < code.vStrConst.size(); strIndex++)
				{
					AddString2Bytes(vBuff, (char*)code.vStrConst[strIndex].c_str());
				}
				AddInt2Bytes(vBuff, code.vFloatConst.size());
				for (unsigned int FloatIndex = 0; FloatIndex < code.vFloatConst.size(); FloatIndex++)
				{
					AddFloat2Bytes(vBuff, code.vFloatConst[FloatIndex]);
				}
				AddInt2Bytes(vBuff, code.vNumVar.size());
				for (unsigned int indexNum = 0; indexNum < code.vNumVar.size(); indexNum++)
				{
					VarPoint& var = code.vNumVar[indexNum];
					AddChar2Bytes(vBuff, var.cType);
					AddUShort2Bytes(vBuff, var.unArraySize);

					switch (var.cType)
					{
					case EScriptVal_Int:
					{
						for (unsigned short r = 0; r < var.unArraySize; r++)
							AddInt642Bytes(vBuff, var.pInt64[r]);
					}
					break;
					case EScriptVal_Double:
					{
						for (unsigned short r = 0; r < var.unArraySize; r++)
							AddDouble2Bytes(vBuff, var.pDouble[r]);
					}
					break;
					case EScriptVal_String:
					{
						for (unsigned short r = 0; r < var.unArraySize; r++)
							AddChar2Bytes(vBuff, var.pStr[r]);
					}
					break;
					}
				}
				AddInt2Bytes(vBuff, code.vCodeData.size());
				for (unsigned int indexCode = 0; indexCode < code.vCodeData.size(); indexCode++)
				{
					CodeStyle& style = code.vCodeData[indexCode];
					AddInt642Bytes(vBuff, style.qwCode);
				}
			}

			AddInt2Bytes(vBuff, m_mapString2CodeIndex.size());
			std::map<std::string, int>::iterator itIndex = m_mapString2CodeIndex.begin();
			for (; itIndex != m_mapString2CodeIndex.end(); itIndex++)
			{
				AddString2Bytes(vBuff, (char*)itIndex->first.c_str());
				AddInt2Bytes(vBuff, itIndex->second);
			}
			fwrite(&vBuff[0], sizeof(char), vBuff.size(), fp);
			fclose(fp);
		}

#endif // 

	}
	void CScriptCodeLoader::LoadFormBin()
	{
#if 0
		std::string fullfile = cocos2d::CCFileUtils::getInstance()->fullPathForFilename("script.bin");
		cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(fullfile);
		char* pData = (char*)data.getBytes();
		int len = data.getSize();
		int pos = 0;
		if (len <= 0)
		{
			return;
		}
		int nVerison = DecodeBytes2Int(pData, pos, len);
		int nGlobalSize = DecodeBytes2Int(pData, pos, len);
		vGlobalNumVar.resize(nGlobalSize);
		for (int i = 0; i < nGlobalSize; i++)
		{
			VarPoint& var = vGlobalNumVar[i];
			var.cType = DecodeBytes2Char(pData, pos, len);
			var.unArraySize = DecodeBytes2Short(pData, pos, len);
			switch (var.cType)
			{
			case EScriptVal_Int:
			{
				var.pInt64 = new __int64[var.unArraySize];
				for (unsigned short r = 0; r < var.unArraySize; r++)
					var.pInt64[r] = DecodeBytes2Int64(pData, pos, len);
			}
			break;
			case EScriptVal_Double:
			{
				var.pDouble = new double[var.unArraySize];
				for (unsigned short r = 0; r < var.unArraySize; r++)
					var.pDouble[r] = DecodeBytes2Double(pData, pos, len);
			}
			break;
			case EScriptVal_String:
			{
				var.pStr = new char[var.unArraySize];
				for (unsigned short r = 0; r < var.unArraySize; r++)
					var.pStr[r] = DecodeBytes2Char(pData, pos, len);
			}
			break;
			}
		}
		int nCodeSize = DecodeBytes2Int(pData, pos, len);
		m_vecCodeData.resize(nCodeSize);
		for (int i = 0; i < nCodeSize; i++)
		{
			tagCodeData& code = m_vecCodeData[i];
			int nStringSize = DecodeBytes2Int(pData, pos, len);
			code.vStrConst.resize(nStringSize);
			for (int indexStr = 0; indexStr < nStringSize; indexStr++)
			{
				char strTemp[2048];
				DecodeBytes2String(pData, pos, len, strTemp, 2048);
				code.vStrConst[indexStr] = strTemp;
			}
			int nFloatSize = DecodeBytes2Int(pData, pos, len);
			code.vFloatConst.resize(nFloatSize);
			for (int indexFloat = 0; indexFloat < nFloatSize; indexFloat++)
			{
				code.vFloatConst[indexFloat] = DecodeBytes2Float(pData, pos, len);
			}
			int nVarSize = DecodeBytes2Int(pData, pos, len);
			code.vNumVar.resize(nVarSize);
			for (int indexNum = 0; indexNum < nVarSize; indexNum++)
			{
				VarPoint& var = code.vNumVar[indexNum];
				var.cType = DecodeBytes2Char(pData, pos, len);
				var.unArraySize = DecodeBytes2Short(pData, pos, len);
				switch (var.cType)
				{
				case EScriptVal_Int:
				{
					var.pInt64 = new __int64[var.unArraySize];
					for (unsigned short r = 0; r < var.unArraySize; r++)
						var.pInt64[r] = DecodeBytes2Int64(pData, pos, len);
				}
				break;
				case EScriptVal_Double:
				{
					var.pDouble = new double[var.unArraySize];
					for (unsigned short r = 0; r < var.unArraySize; r++)
						var.pDouble[r] = DecodeBytes2Double(pData, pos, len);
				}
				break;
				case EScriptVal_String:
				{
					var.pStr = new char[var.unArraySize];
					for (unsigned short r = 0; r < var.unArraySize; r++)
						var.pStr[r] = DecodeBytes2Char(pData, pos, len);
				}
				break;
				}
			}
			int nCodeDataSize = DecodeBytes2Int(pData, pos, len);
			code.vCodeData.resize(nCodeDataSize);
			for (int indexCode = 0; indexCode < nCodeDataSize; indexCode++)
			{
				code.vCodeData[indexCode].qwCode = DecodeBytes2Int64(pData, pos, len);
			}
		}
		int nDicSize = DecodeBytes2Int(pData, pos, len);
		for (int i = 0; i < nDicSize; i++)
		{
			char strTemp[2048];
			DecodeBytes2String(pData, pos, len, strTemp, 2048);
			int nIndex = DecodeBytes2Int(pData, pos, len);
			m_mapString2CodeIndex[strTemp] = nIndex;
		}
#endif
	}

#if _SCRIPT_DEBUG
	void CScriptCodeLoader::PartitionSourceWords(std::vector<char>& vSource)
	{
		m_vCharIndex2LineIndex.resize(vSource.size(),0);
		unsigned int nLineIndex = 1;
		char ch[2] = { 0,0 };
		std::string strCurWord;
		for (size_t i = 0; i < vSource.size(); i++)
		{
			bool bNewLine = false;
			if (vSource[i] == 10)
			{
				bNewLine = true;
			}
			else if (vSource[i] == 13)
			{
				if (i + 1 < vSource.size())
				{
					if (vSource[i + 1] == 10)
					{
						i++;
					}
				}
				bNewLine = true;
			}
			if (bNewLine)
			{
				tagSourceLineInfo info;
				info.strCurFileName = strCurFileName;
				info.nLineNum = nLineIndex++;
				info.strLineWords = strCurWord;
				m_vScoureLines.push_back(info);
				strCurWord.clear();
			}
			else
			{
				ch[0] = vSource[i];
				strCurWord += ch;
				m_vCharIndex2LineIndex[i] = m_vScoureLines.size();
			}

		}
		if (strCurWord.size() > 0)
		{
			tagSourceLineInfo info;
			info.strCurFileName = strCurFileName;
			info.nLineNum = nLineIndex++;
			info.strLineWords = strCurWord;
			m_vScoureLines.push_back(info);
			strCurWord.clear();
		}
	}

	unsigned int CScriptCodeLoader::GetSourceWordsIndex(unsigned int nIndex)
	{
		if (m_vCharIndex2LineIndex.size() > nIndex)
		{
			return m_vCharIndex2LineIndex[nIndex];
		}
		return 0;
	}

#endif

	const CScriptCodeLoader::tagSourceLineInfo &CScriptCodeLoader::GetSourceWords(unsigned int nIndex)
	{
#if _SCRIPT_DEBUG
		if (m_vScoureLines.size() > nIndex)
		{
			return m_vScoureLines[nIndex];
		}
#endif
		static tagSourceLineInfo emtpy;
		return emtpy;
	}

	CScriptCodeLoader::tagCodeData::tagCodeData()
	{
		nType = EICODE_FUN_DEFAULT;
		nDefaultReturnType = EScriptVal_None;
	}
}