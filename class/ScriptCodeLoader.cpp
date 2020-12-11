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

#pragma warning(disable : 4996) 

using namespace std;
namespace zlscript
{
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
		nErrorWordPos = 0;
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
				zlscript::CScriptDebugPrintMgr::GetInstance()->Print(strCurFileName + ":" + strError);
#ifdef  _SCRIPT_DEBUG
				zlscript::CScriptDebugPrintMgr::GetInstance()->Print(GetSourceWords(GetSourceLineIndex(m_vCurSourceSentence, nErrorWordPos)));
#endif //  _SCRIPT_DEBUG
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

		//符号
		m_mapDicSignToEnum["=="] = m_mapDic2KeyWord["=="] = ECODE_CMP_EQUAL;//==
		m_mapDicSignToEnum["!="] = m_mapDic2KeyWord["!="] = ECODE_CMP_NOTEQUAL;//!=
		m_mapDicSignToEnum[">"] = m_mapDic2KeyWord[">"] = ECODE_CMP_BIG;//>
		m_mapDicSignToEnum[">="] = m_mapDic2KeyWord[">="] = ECODE_CMP_BIGANDEQUAL;//>=
		m_mapDicSignToEnum["<"] = m_mapDic2KeyWord["<"] = ECODE_CMP_LESS;//<
		m_mapDicSignToEnum["<="] = m_mapDic2KeyWord["<="] = ECODE_CMP_LESSANDEQUAL;//<=

		m_mapDicSignToEnum["&"] = m_mapDic2KeyWord["&"] = ECODE_BIT_AND;
		m_mapDicSignToEnum["|"] = m_mapDic2KeyWord["|"] = ECODE_BIT_OR;
		m_mapDicSignToEnum["^"] = m_mapDic2KeyWord["^"] = ECODE_BIT_XOR;

		m_mapDicSignToPRI["=="] = 2;//==
		m_mapDicSignToPRI["!="] = 2;//!=
		m_mapDicSignToPRI[">"] = 2;//>
		m_mapDicSignToPRI[">="] = 2;//>=
		m_mapDicSignToPRI["<"] = 2;//<
		m_mapDicSignToPRI["<="] = 2;//<=

		m_mapDicSignToPRI["&"] = 1;//>=
		m_mapDicSignToPRI["|"] = 1;//<
		m_mapDicSignToPRI["^"] = 1;//<=

		m_mapDic2KeyWord["="] = SCRIPTSENTENCE_EVALUATE;// 赋值=

		m_mapDicSignToEnum["+"] = m_mapDic2KeyWord["+"] = ECODE_ADD; //加+
		m_mapDicSignToEnum["-"] = m_mapDic2KeyWord["-"] = ECODE_SUM; //减-
		m_mapDicSignToEnum["*"] = m_mapDic2KeyWord["*"] = ECODE_MUL; //乘*
		m_mapDicSignToEnum["/"] = m_mapDic2KeyWord["/"] = ECODE_DIV; //除/
		m_mapDicSignToEnum["%"] = m_mapDic2KeyWord["%"] = ECODE_MOD; //求余%

		m_mapDicSingleSignToEnum["-"] = ECODE_MINUS;

		m_mapDicSignToPRI["+"] = 3; //加+
		m_mapDicSignToPRI["-"] = 3; //减-
		m_mapDicSignToPRI["*"] = 4; //乘*
		m_mapDicSignToPRI["/"] = 4; //除/
		m_mapDicSignToPRI["%"] = 4; //求余%

		m_mapDicSignToEnum["->"] = m_mapDic2KeyWord["->"] = ECODE_CALL_CLASS_FUN; //调用类函数
		m_mapDicSignToPRI["->"] = 4; //调用类函数

		m_mapDic2KeyWord["int"] = EScriptVal_Int;
		m_mapDic2KeyWord["float"] = EScriptVal_Double;
		m_mapDic2KeyWord["string"] = EScriptVal_String;
		//*********************中间码的字典**************************//
		m_mapDicSentenceToICode["global"] = EICode_Global_Define;
		m_mapDicVarTypeToICode["void"] = EScriptVal_None;
		m_mapDicVarTypeToICode["int"] = EScriptVal_Int;
		m_mapDicVarTypeToICode["float"] = EScriptVal_Double;
		m_mapDicVarTypeToICode["string"] = EScriptVal_String;

		m_mapDicFunToICode["intact"] = EICODE_FUN_CAN_BREAK;
	}
	void CScriptCodeLoader::clear()
	{
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


#define GetNewWord(word) \
	if (vIn.size() == 0) \
	{ \
		strError = "(Unexpected end of statement)"; \
		return ECompile_ERROR; \
	} \
	tagSourceWord word = vIn.front(); \
	vIn.pop_front();

#define GetWord(word) \
	if (vIn.size() == 0) \
	{ \
		strError = "(Unexpected end of statement)"; \
		return ECompile_ERROR; \
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
		GetNewWord(strType);
		GetNewWord(strName);

		int nParameterType = m_mapDicVarTypeToICode[strType.word];
		int nClassType = 0;
		if (nParameterType == EScriptVal_None)
		{
			nClassType = CScriptSuperPointerMgr::GetInstance()->GetClassType(strType.word);
			if (nClassType <= 0)
			{
				nErrorWordPos = strType.nSourceWordsIndex;
				strError = "DefineTempVar(Class type error when defining temporary variable)";
				return ECompile_ERROR;
			}
			nParameterType = EScriptVal_ClassPointIndex;
		}
		if (!CheckVarName(strName.word))
		{
			nErrorWordPos = strName.nSourceWordsIndex;
			strError = "DefineTempVar(Illegal variable name when defining temporary variable)";
			return ECompile_ERROR;
		}
		//检查这个变量名是否已经存在
		if (pCode->CheckTempVar(strName.word.c_str()))
		{
			nErrorWordPos = strName.nSourceWordsIndex;
			strError = "DefineTempVar(Variable name already exists when defining temporary variable)";
			return ECompile_ERROR;
		}

		StackVarInfo defVar;//默认值
		defVar.cType = nParameterType;

		pCode->SetTempVarIndex(strName.word.c_str(), m_vTempCodeData.vNumVar.size(), nParameterType, nClassType);
		m_vTempCodeData.vNumVar.push_back(defVar);
		return ECompile_Return;
	}

	int CScriptCodeLoader::LoadDefineTempVar(SentenceSourceCode& vIn, CBaseICode* pCode)
	{
		GetNewWord(strType);
		GetNewWord(strName);

		int nParameterType = m_mapDicVarTypeToICode[strType.word];
		int nClassType = 0;
		if (nParameterType == EScriptVal_None)
		{
			nClassType = CScriptSuperPointerMgr::GetInstance()->GetClassType(strType.word);
			if (nClassType <= 0)
			{
				nErrorWordPos = strType.nSourceWordsIndex;
				strError = "DefineTempVar(Class type error when defining temporary variable)";
				return ECompile_ERROR;
			}
			nParameterType = EScriptVal_ClassPointIndex;
		}

		if (!CheckVarName(strName.word))
		{
			nErrorWordPos = strName.nSourceWordsIndex;
			strError = "DefineTempVar(Illegal variable name when defining temporary variable)";
			return ECompile_ERROR;
		}
		//检查这个变量名是否已经存在
		if (pCode->CheckTempVar(strName.word.c_str()))
		{
			nErrorWordPos = strName.nSourceWordsIndex;
			strError = "DefineTempVar(Variable name already exists when defining temporary variable)";
			return ECompile_ERROR;
		}
		StackVarInfo varpoint;
		varpoint.cType = nParameterType;
		varpoint.Int64 = 0;
		pCode->SetTempVarIndex(strName.word.c_str(), m_vTempCodeData.vNumVar.size(), nParameterType, nClassType);
		m_vTempCodeData.vNumVar.push_back(varpoint);

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
		GetNewWord(strHeadCheck);

		int nFunType = EICODE_FUN_DEFAULT;
		{
			auto it = m_mapDicFunToICode.find(strHeadCheck.word);
			if (it != m_mapDicFunToICode.end())
			{
				nFunType = it->second;
			}
		}
		string strVarType;
		if (nFunType != EICODE_FUN_DEFAULT)
		{
			GetNewWord(varType);
			strVarType = varType.word;
		}
		else
		{
			strVarType = strHeadCheck.word;
		}

		int nVarType = m_mapDicVarTypeToICode[strVarType];
		if (nVarType == EICode_NONE)
		{
			int nClassType = CScriptSuperPointerMgr::GetInstance()->GetClassType(strVarType);
			if (nClassType > 0)
			{
				nVarType = EScriptVal_ClassPointIndex;
			}
		}
		m_nCurFunVarType = nVarType;

		GetNewWord(wordFunName);


		if (m_mapString2CodeIndex.find(wordFunName.word) != m_mapString2CodeIndex.end())
		{
			tagCodeData& funCode = m_vecCodeData[m_mapString2CodeIndex[wordFunName.word]];
			if (funCode.nType != EICODE_FUN_NO_CODE || !funCode.vCodeData.empty())
			{
				nErrorWordPos = wordFunName.nSourceWordsIndex;
				strError = "LoadDefineFunState(Function name already exists)";
				return ECompile_ERROR;
			}

		}

		//判断变量名是否重复

		if (m_mapDicGlobalVar.find(wordFunName.word) != m_mapDicGlobalVar.end())
		{
			nErrorWordPos = wordFunName.nSourceWordsIndex;
			strError = "LoadDefineFunState(Variable name already exists)";
			return ECompile_ERROR;
		}

		if (CScriptCallBackFunion::GetFunIndex(wordFunName.word) >= 0)
		{
			nErrorWordPos = wordFunName.nFlag;
			strError = "LoadDefineFunState(Callback Function name already exists)";
			return ECompile_ERROR;
		}

		GetNewWord(nextWord);

		//如果语句结束，说明只是定义了一个全局变量
		if (nextWord.word != "(")
		{
			//GetWord(nextWord);
			StackVarInfo defVar;//默认值
			defVar.cType = nVarType;
			if (nextWord.word == "=")
			{
				//有初始化值，只支持常量赋值
				GetWord(nextWord);
				switch (nVarType)
				{
				case EScriptVal_Int:
				{
					defVar.Int64 = atoi(nextWord.word.c_str());
				}
				break;
				case EScriptVal_Double:
				{
					defVar.Double = atof(nextWord.word.c_str());
				}
				break;
				case EScriptVal_String:
				{
					defVar.Int64 = StackVarInfo::s_strPool.NewString(nextWord.word.c_str());
				}
				break;
				case EScriptVal_ClassPointIndex:
				{
					if (nextWord.word == "nullptr")
					{
						defVar.Int64 = 0;
					}
				}
				GetWord(nextWord);
				}
				if (nextWord.word != ";")
				{
					nErrorWordPos = wordFunName.nFlag;
					strError = "Format error defining global variable";
					return ECompile_ERROR;
				}
				VarInfo info;
				info.cType = nVarType;
				info.cGlobal = 1;
				info.wExtend = m_nCurFunVarType;
				info.dwPos = vGlobalNumVar.size();
				m_mapDicGlobalVar[wordFunName.word] = info;

				vGlobalNumVar.push_back(defVar);

			}
		}
		//如果接下来的是括号，说明是函数定义
		else
		{
			m_vTempCodeData.vCodeData.clear();
			m_vTempCodeData.vNumVar.clear();
			m_vTempCodeData.vFloatConst.clear();
			m_vTempCodeData.vStrConst.clear();
			//一个新函数
			m_pFun_ICode = CICodeMgr::GetInstance()->New<CFunICode>();

			int VarIndex = 0;
			while (true)
			{
				GetWord(nextWord);
				if (nextWord.word == ")")
				{
					break;
				}
				RevertWord(nextWord);

				if (LoadDefineFunctionParameter(vIn, m_pFun_ICode) <= 0)
				{
					return ECompile_ERROR;
				}

				GetWord(nextWord);

				if (nextWord.word == ")")
				{
					break;
				}
				else if (nextWord.word != ",")
				{
					nErrorWordPos = nextWord.nSourceWordsIndex;
					strError = "LoadDefineFunState(Function parameter definition format error)";
					return ECompile_ERROR;
				}
			}

			GetWord(nextWord);
			if (nextWord.word == "{")
			{
				RevertWord(nextWord);
				if (LoadBlockState(vIn, m_pFun_ICode, 0) != ECompile_ERROR)
				{
					m_pFun_ICode->MakeExeCode(m_vTempCodeData.vCodeData);
					m_pFun_ICode = nullptr;
					CICodeMgr::GetInstance()->Clear();
				}
				else
				{
					return ECompile_ERROR;
				}
			}
			else if (nextWord.word == ";")
			{
				//说明只是声明了函数，还没有定义
			}

			//将数据放入代码库


			m_vTempCodeData.filename = strCurFileName;
			m_vTempCodeData.funname = wordFunName.word;
			m_vTempCodeData.nDefaultReturnType = m_nCurFunVarType;

			if (m_mapString2CodeIndex.find(wordFunName.word) != m_mapString2CodeIndex.end())
			{
				if (m_vTempCodeData.vCodeData.size())
				{
					tagCodeData& funCode = m_vecCodeData[m_mapString2CodeIndex[wordFunName.word]];
					funCode = m_vTempCodeData;
				}
			}
			else
			{
				m_vTempCodeData.nType = nFunType;
				m_mapString2CodeIndex[wordFunName.word] = m_vecCodeData.size();
				m_vecCodeData.push_back(m_vTempCodeData);
			}



		}



		return ECompile_Return;
	}
	int CScriptCodeLoader::LoadBlockState(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	{
		CBlockICode* pBlockICode = CICodeMgr::GetInstance()->New<CBlockICode>();
		if (pBlockICode == nullptr)
		{
			return ECompile_ERROR;
		}
		pCode->AddICode(nType, pBlockICode);

		GetNewWord(nextWord);
		if (nextWord.word != "{")
		{
			nErrorWordPos = nextWord.nFlag;
			strError = "LoadBlockState(Block format error)";
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
			else if (nextWord.word == "int" || nextWord.word == "float" || nextWord.word == "string"
				|| CScriptSuperPointerMgr::GetInstance()->GetClassType(nextWord.word) > 0)
			{
				RevertWord(nextWord);
				nReturn = LoadDefineTempVar(vIn, pBlockICode);
				if (nReturn == ECompile_Next)
					nReturn = LoadOneSentence(vIn, pBlockICode, 0);
			}
			else
			{
				RevertWord(nextWord);
				nReturn = LoadOneSentence(vIn, pBlockICode, 0);
			}
		}

		return nReturn;
	}
	int CScriptCodeLoader::LoadIfSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	{
		CIfICode* pIfICode = CICodeMgr::GetInstance()->New<CIfICode>();
		if (pIfICode == nullptr)
		{
			return ECompile_ERROR;
		}
		pCode->AddICode(nType, pIfICode);

		GetNewWord(nextWord);
		if (nextWord.word != "if")
		{
			nErrorWordPos = nextWord.nFlag;
			strError = "LoadIfSentence(Block format error)";
			return ECompile_ERROR;
		}

		GetWord(nextWord);
		if (nextWord.word != "(")
		{
			nErrorWordPos = nextWord.nFlag;
			strError = "LoadIfSentence(Block format error)";
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
		if (LoadOneSentence(vCondCode, pIfICode, 0,"") == ECompile_ERROR)
		{
			return ECompile_ERROR;
		}

		GetWord(nextWord);
		if (nextWord.word != "{")
		{
			nErrorWordPos = nextWord.nFlag;
			strError = "LoadIfSentence(Block format error)";
			return ECompile_ERROR;
		}
		RevertWord(nextWord);
		if (LoadBlockState(vIn, pIfICode, 1) == ECompile_ERROR)
		{
			return ECompile_ERROR;
		}
		GetWord(nextWord);
		if (nextWord.word == "else")
		{
			GetWord(nextWord);
			if (nextWord.word == "if")
			{
				RevertWord(nextWord);
				if (LoadIfSentence(vIn, pIfICode, 2) == ECompile_ERROR)
				{
					return ECompile_ERROR;
				}
			}
			else if (nextWord.word == "{")
			{
				RevertWord(nextWord);
				if (LoadBlockState(vIn, pIfICode, 2) == ECompile_ERROR)
				{
					return ECompile_ERROR;
				}
			}
			else
			{
				nErrorWordPos = nextWord.nFlag;
				strError = "LoadIfSentence(Block format error)";
				return ECompile_ERROR;
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
		return 0;
	}
	int CScriptCodeLoader::LoadWhileSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	{
		CWhileICode* pWhileICode = CICodeMgr::GetInstance()->New<CWhileICode>();
		if (pWhileICode == nullptr)
		{
			return ECompile_ERROR;
		}
		pCode->AddICode(nType, pWhileICode);

		GetNewWord(nextWord);
		if (nextWord.word != "while")
		{
			nErrorWordPos = nextWord.nFlag;
			strError = "LoadWhileSentence(Block format error)";
			return ECompile_ERROR;
		}

		GetWord(nextWord);
		if (nextWord.word != "(")
		{
			nErrorWordPos = nextWord.nFlag;
			strError = "LoadWhileSentence(Block format error)";
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
		if (LoadOneSentence(vCondCode, pWhileICode, 0,"") == ECompile_ERROR)
		{
			return ECompile_ERROR;
		}
		GetWord(nextWord);
		if (nextWord.word != "{")
		{
			nErrorWordPos = nextWord.nFlag;
			strError = "LoadIfSentence(Block format error)";
			return ECompile_ERROR;
		}
		RevertWord(nextWord);
		if (LoadBlockState(vIn, pWhileICode, 1) == ECompile_ERROR)
		{
			return ECompile_ERROR;
		}
		return ECompile_Return;
	}
	int CScriptCodeLoader::LoadSwitchSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	{
		return 0;
	}
	int CScriptCodeLoader::LoadReturnSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType)
	{
		CReturnICode* pReturnICode = CICodeMgr::GetInstance()->New<CReturnICode>();
		if (pReturnICode == nullptr)
		{
			return ECompile_ERROR;
		}
		pCode->AddICode(nType, pReturnICode);
		GetNewWord(nextWord);
		if (nextWord.word != "return")
		{
			nErrorWordPos = nextWord.nFlag;
			strError = "LoadReturnSentence:format error)";
			return ECompile_ERROR;
		}

		GetWord(nextWord);
		if (nextWord.word == ";")
		{
			return ECompile_Return;
		}
		RevertWord(nextWord);
		//什么都不是，那就可能是算式了
		if (LoadOneSentence(vIn, pReturnICode, 0) == ECompile_ERROR)
		{
			return ECompile_Return;
		}
		return ECompile_Return;
	}

	int CScriptCodeLoader::LoadOneSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType, std::string endFlag)
	{
		CSentenceICode* pSentenceICode = CICodeMgr::GetInstance()->New<CSentenceICode>();
		if (pSentenceICode == nullptr)
		{
			return ECompile_ERROR;
		}
		pCode->AddICode(nType, pSentenceICode);

		GetNewWord(nextWord);
		if (nextWord.word == ";")
		{
			return ECompile_Return;
		}
		if (nextWord.word == "delete")
		{
			GetWord(nextWord);
			int nClassIndex = QueryTempVar(nextWord.word, pSentenceICode);
			char cSign = 0;
			StackVarInfo* pVar = nullptr;
			if (nClassIndex >= 0)
			{
				pVar = &m_vTempCodeData.vNumVar[nClassIndex];
				cSign = 2;
			}
			else if (m_mapDicGlobalVar.find(nextWord.word) != m_mapDicGlobalVar.end())
			{
				VarInfo info = m_mapDicGlobalVar[nextWord.word];
				nClassIndex = info.dwPos;
				pVar = &vGlobalNumVar[nClassIndex];
				cSign = 1;
			}
			if (pVar && pVar->cType == EScriptVal_ClassPointIndex)
			{
				CodeStyle code;
				code.qwCode = 0;
				code.wInstruct = ECODE_PUSH;
				code.cSign = cSign;
				code.cExtend = 0;
				code.dwPos = nClassIndex;
				pSentenceICode->AddExeCode(code);
			}
			else
			{
				//不是类指针，错误
				nErrorWordPos = nextWord.nFlag;
				strError = "delete: no class point";
				return ECompile_ERROR;
			}
			CodeStyle deleteCode;
			deleteCode.qwCode = 0;
			deleteCode.wInstruct = ECODE_RELEASE_CLASS;
			pSentenceICode->AddExeCode(deleteCode);

			GetWord(nextWord);
			if (nextWord.word != ";")
			{
				nErrorWordPos = nextWord.nFlag;
				strError = "delete: format error";
				return ECompile_ERROR;
			}
		}
		else if (nextWord.word == "break")
		{
			GetWord(nextWord);

			if (nextWord.word != ";")
			{
				nErrorWordPos = nextWord.nFlag;
				strError = "break: format error";
				return ECompile_ERROR;
			}
			CodeStyle breakCode;
			breakCode.qwCode = 0;
			breakCode.wInstruct = ECODE_BREAK;
			pSentenceICode->AddExeCode(breakCode);
		}
		else
		{
			std::vector<CodeStyle> vTempCode;
			int nReturn = ECompile_ERROR;
			//if (m_mapString2CodeIndex.find(nextWord.word) != m_mapString2CodeIndex.end())
			//{
			//	RevertWord(nextWord);
			//	//脚本函数调用
			//	nReturn = LoadCallFunState(vIn, pSentenceICode, vTempCode);
			//}
			////回调函数
			//else if (CScriptCallBackFunion::GetFunIndex(nextWord.word) >= 0)
			//{
			//	RevertWord(nextWord);
			//	nReturn = LoadCallFunState(vIn, pSentenceICode, vTempCode);
			//}
			//else
			{
				bool isEvaluate = false;
				CodeStyle code;
				if (vIn.size() > 0)
				{
					GetNewWord(flagWord);
					if (flagWord.word == "=")
					{
						//赋值
						isEvaluate = true;
						//查询临时变量
						int nVarIndex = QueryTempVar(nextWord.word, pSentenceICode);
						if (nVarIndex != g_nTempVarIndexError)
						{
							code.qwCode = 0;
							code.wInstruct = ECODE_EVALUATE;
							code.cSign = 1;
							code.cExtend = 0;
							code.dwPos = nVarIndex;
						}
						else if(m_mapDicGlobalVar.find(nextWord.word) != m_mapDicGlobalVar.end())
						{
							VarInfo info = m_mapDicGlobalVar[nextWord.word];
							code.qwCode = 0;
							code.wInstruct = ECODE_EVALUATE;
							code.cSign = 0;
							code.cExtend = 0;
							code.dwPos = info.dwPos;
						}
					}
					else
					{
						RevertWord(flagWord);
						RevertWord(nextWord);
					}
				}
				else
				{
					RevertWord(nextWord);
				}

				//什么都不是，那就可能是算式了
				nReturn = LoadFormulaSentence(vIn, pSentenceICode, vTempCode);
				if (isEvaluate)
				{
					vTempCode.push_back(code);
				}
			}
			if (!endFlag.empty())
			{
				GetWord(nextWord);
				if (nextWord.word != endFlag)
				{
					nErrorWordPos = nextWord.nFlag;
					strError = "Sentence format error";
					return ECompile_ERROR;
				}
			}
			else
			{
				pSentenceICode->SetClear(false);
			}

			for (auto it = vTempCode.begin(); it != vTempCode.end(); it++)
			{
				pSentenceICode->AddExeCode(*it);
			}
			return nReturn;
		}
		return ECompile_Return;
	}
	int CScriptCodeLoader::LoadCallFunState(SentenceSourceCode& vIn, CBaseICode* pCode, std::vector<CodeStyle>& vOut)
	{
		GetNewWord(FunName);

		CodeStyle begincode;
		begincode.qwCode = 0;
		begincode.wInstruct = ECODE_BEGIN_CALL;

		CodeStyle callCode;
		callCode.qwCode = 0;
		callCode.wInstruct = ECODE_CALL;

		int nCallFunIndex = CScriptCallBackFunion::GetFunIndex(FunName.word);
		if (nCallFunIndex >= 0)
		{
			callCode.cSign = 0;
			callCode.dwPos = nCallFunIndex;
		}
		else if (m_mapString2CodeIndex.find(FunName.word) != m_mapString2CodeIndex.end())
		{
			callCode.cSign = 1;
			callCode.dwPos = m_vTempCodeData.vCallFunName.size();
			m_vTempCodeData.vCallFunName.push_back(FunName.word);
		}
		else
		{
			strError = "LoadCallFunState(Function call, function not found)";
			nErrorWordPos = FunName.nSourceWordsIndex;
			return ECompile_ERROR;
		}
		GetNewWord(nextWord);
		if (nextWord.word != "(")
		{
			nErrorWordPos = nextWord.nFlag;
			strError = "LoadCallFunState(Block format error)";
			return ECompile_ERROR;
		}
		//函数参数要倒着取才行
		stack<tagCodeSection> sTempCode;
		int nParamNum = 0;
		while (true)
		{
			int nBracketSize = 0;
			SentenceSourceCode vParmCode;
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
				else if (nextWord.word == ",")
				{
					if (nBracketSize <= 0)
					{
						break;
					}
				}
				vParmCode.push_back(nextWord);
			}

			if (vParmCode.size() > 0)
			{
				tagCodeSection vTemp;
				if (LoadFormulaSentence(vParmCode, pCode, vTemp) == ECompile_ERROR)
				{
					return ECompile_ERROR;
				}
				sTempCode.push(vTemp);
				nParamNum++;
			}

			if (nextWord.word == ")")
			{
				break;
			}

		}
		vOut.push_back(begincode);
		while (!sTempCode.empty())
		{
			tagCodeSection& vTemp = sTempCode.top();
			for (unsigned int i = 0; i < vTemp.size(); i++)
			{
				vOut.push_back(vTemp[i]);
			}
			sTempCode.pop();
		}
		callCode.cExtend = nParamNum;
		vOut.push_back(callCode);


		return ECompile_Return;
	}

	int CScriptCodeLoader::LoadCallClassFun(SentenceSourceCode& vIn, CBaseICode* pCode, std::vector<CodeStyle>& vOut)
	{
		CodeStyle begincode;
		begincode.qwCode = 0;
		begincode.wInstruct = ECODE_BEGIN_CALL;

		CodeStyle classcode;
		classcode.qwCode = 0;
		classcode.wInstruct = ECODE_PUSH;

		CodeStyle callCode;
		callCode.qwCode = 0;
		callCode.wInstruct = ECODE_CALL_CLASS_FUN;

		GetNewWord(FunName);
		int nClassIndex = QueryTempVar(FunName.word, pCode);
		int nClassType = 0;
		VarInfo info;
		StackVarInfo* pVar = nullptr;
		if (nClassIndex >= 0)
		{
			info = pCode->GetTempVarInfo(FunName.word.c_str());
			pVar = &m_vTempCodeData.vNumVar[nClassIndex];
			classcode.cSign = 2;
			classcode.cExtend = 0;
			classcode.dwPos = nClassIndex;
		}
		else if (m_mapDicGlobalVar.find(FunName.word) != m_mapDicGlobalVar.end())
		{
			info = m_mapDicGlobalVar[FunName.word];
			nClassIndex = info.dwPos;
			pVar = &vGlobalNumVar[nClassIndex];
			classcode.cSign = 1;
			classcode.cExtend = 0;
			classcode.dwPos = nClassIndex;
		}
		if (pVar && pVar->cType == EScriptVal_ClassPointIndex)
		{
			callCode.dwPos = info.wExtend;
		}
		else
		{
			strError = "LoadCallClassFunn(Non class pointer when calling class function)";
			nErrorWordPos = FunName.nSourceWordsIndex;
			return ECompile_ERROR;
		}
		GetNewWord(nextWord);
		if (nextWord.word != "->")
		{
			strError = "LoadCallClassFunn(When calling a class function, the class pointer cannot have an operator other than)";
			nErrorWordPos = nextWord.nSourceWordsIndex;
			return ECompile_ERROR;
		}
		GetWord(nextWord);
		int nFunIndex = CScriptSuperPointerMgr::GetInstance()->GetClassFunIndex(info.wExtend, nextWord.word);
		if (nFunIndex < 0)
		{
			strError = "LoadCallClassFunn(Class function not registered when calling class function)";
			nErrorWordPos = nextWord.nSourceWordsIndex;
			return ECompile_ERROR;
		}
		callCode.dwPos = nFunIndex;

		GetWord(nextWord);
		if (nextWord.word != "(")
		{
			strError = "LoadCallClassFunn()";
			nErrorWordPos = nextWord.nSourceWordsIndex;
			return ECompile_ERROR;
		}

		stack<tagCodeSection> sTempCode;
		int nParamNum = 0;
		while (true)
		{
			int nBracketSize = 0;
			SentenceSourceCode vParmCode;
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
				else if (nextWord.word == ",")
				{
					if (nBracketSize <= 0)
					{
						break;
					}
				}
				vParmCode.push_back(nextWord);
			}
			if (vParmCode.size() > 0)
			{
				tagCodeSection vTemp;
				if (LoadFormulaSentence(vParmCode, pCode, vTemp) == ECompile_ERROR)
				{
					return ECompile_ERROR;
				}
				sTempCode.push(vTemp);
				nParamNum++;
			}
			if (nextWord.word == ")")
			{
				break;
			}

		}

		vOut.push_back(begincode);

		while (!sTempCode.empty())
		{
			tagCodeSection& vTemp = sTempCode.top();
			for (unsigned int i = 0; i < vTemp.size(); i++)
			{
				vOut.push_back(vTemp[i]);
			}
			sTempCode.pop();
		}
		callCode.cExtend = nParamNum;

		vOut.push_back(classcode);
		vOut.push_back(callCode);

		return ECompile_Return;
	}

	int CScriptCodeLoader::LoadFormulaSentence(SentenceSourceCode& vIn, CBaseICode* pCode, std::vector<CodeStyle>& vOut)
	{
		GetNewWord(nextWord);
		if (nextWord.word == "new")
		{
			GetNewWord(ClassName);
			CodeStyle newCode;
			newCode.qwCode = 0;
			newCode.wInstruct = ECODE_NEW_CLASS;
			newCode.dwPos = CScriptSuperPointerMgr::GetInstance()->GetClassType(ClassName.word);

			vOut.push_back(newCode);
			return ECompile_Return;
		}
		RevertWord(nextWord);
		//先检查是不是运算符
		enum
		{
			E_NODE_NONE,
			E_NODE_SIGN,
			E_NODE_VAR,
		};
		int nLastNodeType = E_NODE_NONE;
		struct CBaseNode
		{
			CBaseNode()
			{
				pFather = nullptr;
				pLeft = nullptr;
				pRight = nullptr;
			}
			virtual int GetType() = 0;
			virtual void ReleaseSelf()
			{
				if (pLeft)
				{
					pLeft->ReleaseSelf();
				}
				if (pRight)
				{
					pRight->ReleaseSelf();
				}
				delete this;
			}
			CBaseNode* pFather;
			CBaseNode* pLeft;
			CBaseNode* pRight;

			virtual void addCode(vector<CodeStyle>& vOut)
			{
				if (pLeft)
				{
					pLeft->addCode(vOut);
				}
				if (pRight)
				{
					pRight->addCode(vOut);
				}
			}
		};
		struct CSignNode : public CBaseNode
		{
			int GetType()
			{
				return E_NODE_SIGN;
			}
			unsigned short unSign;
			int nLevel;

			void addCode(vector<CodeStyle>& vOut)
			{
				CBaseNode::addCode(vOut);

				CodeStyle code;
				code.qwCode = 0;
				code.wInstruct = unSign;

				vOut.push_back(code);
			}
		};
		struct CVarNode : public CBaseNode
		{
			int GetType()
			{
				return E_NODE_VAR;
			}
			vector<CodeStyle> vTempCode;
			void addCode(vector<CodeStyle>& vOut)
			{
				CBaseNode::addCode(vOut);

				for (size_t i = 0; i < vTempCode.size(); i++)
				{
					vOut.push_back(vTempCode[i]);
				}
			}
		};
		CBaseNode* pRootNode = nullptr;
		int nResult = ECompile_Return;
		int nState = 0;
		while (true)
		{
			if (vIn.empty())
			{
				break;
			}
			GetNewWord(nextWord);

			CBaseNode* pNode = nullptr;
			if (nextWord.word == ";")
			{
				RevertWord(nextWord);
				break;
			}
			else if (nextWord.word == "(")
			{
				nState = 1;
				RevertWord(nextWord);
				CVarNode* pVarNode = new CVarNode;
				//unsigned int nTempPos = curPos;
				if (LoadBracket(vIn, pCode, vOut) == ECompile_ERROR)
				{
					nResult = ECompile_ERROR;
					break;
				}
				nLastNodeType = E_NODE_VAR;
				pNode = pVarNode;
			}
			else if (m_mapDicSignToPRI.find(nextWord.word) == m_mapDicSignToPRI.end())
			{
				if (nState == 1)
				{
					nResult = ECompile_Return;
					break;
				}
				nState = 1;
				CVarNode* pVarNode = new CVarNode;
				nLastNodeType = E_NODE_VAR;

					RevertWord(nextWord);
					if (LoadAndPushNumVar(vIn, pCode, pVarNode->vTempCode) == ECompile_ERROR)
					{
						//生成压值进堆栈的代码失败
						nResult = ECompile_ERROR;
						break;
					}
				//}
				pNode = pVarNode;
			}
			else
			{
				if (nState == 2)
				{
					nResult = ECompile_Return;
					break;
				}
				nState = 2;
				CSignNode* pSignNode = new CSignNode;
				if (nLastNodeType != E_NODE_VAR)
				{
					pSignNode->nLevel = 0;
					pSignNode->unSign = m_mapDicSingleSignToEnum[nextWord.word];
				}
				else
				{
					pSignNode->nLevel = m_mapDicSignToPRI[nextWord.word];
					pSignNode->unSign = m_mapDicSignToEnum[nextWord.word];
				}
				nLastNodeType = E_NODE_SIGN;
				pNode = pSignNode;
			}
			if (pRootNode == nullptr)
			{
				pRootNode = pNode;
			}
			else
			{
				if (pNode->GetType() == E_NODE_VAR)
				{
					//放在最右侧
					CBaseNode* pNowNode = pRootNode;

					while (pNowNode)
					{
						if (pNowNode->pRight == nullptr)
						{
							pNowNode->pRight = pNode;
							pNode->pFather = pNowNode;
							break;
						}
						pNowNode = pNowNode->pRight;
					}
				}
				else if (pNode->GetType() == E_NODE_SIGN)
				{

					if (pRootNode->GetType() == E_NODE_VAR)
					{
						pRootNode->pFather = pNode;
						pNode->pLeft = pRootNode;
						pRootNode = pNode;
					}
					else
					{
						CBaseNode* pNowNode = pRootNode;
						while (pNowNode)
						{
							if (pNowNode->GetType() == E_NODE_SIGN)
							{
								if (((CSignNode*)pNowNode)->nLevel >= ((CSignNode*)pNode)->nLevel)
								{
									pNode->pLeft = pNowNode;
									pNode->pFather = pNowNode->pFather;
									pNowNode->pFather = pNode;
									if (pNowNode == pRootNode)
									{
										pRootNode = pNode;
									}
									break;
								}
							}
							else if (pNowNode->GetType() == E_NODE_VAR)
							{
								if (pNowNode->pFather)
								{
									pNowNode->pFather->pRight = pNode;
								}
								pNode->pFather = pNowNode->pFather;
								pNowNode->pFather = pNode;
								pNode->pLeft = pNowNode;
							}
							else if (pNowNode->pRight == nullptr)
							{
								pNowNode->pRight = pNode;
								pNode->pFather = pNowNode;
							}
							pNowNode = pNowNode->pRight;
						}
					}

				}
			}
		}

		if (nResult != ECompile_ERROR)
		{
			if (pRootNode)
			{
				pRootNode->addCode(vOut);
			}
		}
		//释放二叉树
		if (pRootNode)
		{
			pRootNode->ReleaseSelf();
		}
		return nResult;
	}

	int CScriptCodeLoader::LoadAndPushNumVar(SentenceSourceCode& vIn, CBaseICode* pCode, std::vector<CodeStyle>& vOut)
	{
		GetNewWord(varName);
		if (vIn.size() > 0)
		{
			GetNewWord(nextWord);
			if (nextWord.word == "->")
			{
				RevertWord(nextWord);
				RevertWord(varName);
				if (LoadCallClassFun(vIn, pCode, vOut) == ECompile_ERROR)
				{
					return ECompile_ERROR;
				}
				return ECompile_Return;
			}
			else if (nextWord.word == "(")
			{
				RevertWord(nextWord);
				//脚本函数
				if (m_mapString2CodeIndex.find(varName.word) != m_mapString2CodeIndex.end())
				{
					RevertWord(varName);
					if (LoadCallFunState(vIn, pCode, vOut) == ECompile_ERROR)
					{
						return ECompile_ERROR;
					}
				}
				//回调函数
				else if (CScriptCallBackFunion::GetFunIndex(varName.word) >= 0)
				{
					RevertWord(varName);
					if (LoadCallFunState(vIn, pCode, vOut) == ECompile_ERROR)
					{
						return ECompile_ERROR;
					}
				}
				else
				{
					//TODO:检查函数调用语句是否完整，定义新函数
					m_mapString2CodeIndex[varName.word] = m_vecCodeData.size();
					tagCodeData data;
					data.funname = varName.word;
					data.nType = EICODE_FUN_NO_CODE;
					m_vecCodeData.push_back(data);
					//然后再读取
					RevertWord(varName);
					if (LoadCallFunState(vIn, pCode, vOut) == ECompile_ERROR)
					{
						return ECompile_ERROR;
					}
				}
				return ECompile_Return;
			}
			RevertWord(nextWord);
		}
		//if (vIn.empty())
		{
			if (varName.nFlag == E_WORD_FLAG_STRING)
			{
				//字符串
				CodeStyle code;
				m_vTempCodeData.vStrConst.push_back(varName.word);
				code.wInstruct = ECODE_PUSH;
				code.cSign = 3;
				code.cExtend = 0;
				code.dwPos = m_vTempCodeData.vStrConst.size() - 1;
				vOut.push_back(code);
			}
			else
			{
				CodeStyle code;
				code.qwCode = 0;
				int nIndexVar = QueryTempVar(varName.word, pCode);
				if (nIndexVar >= 0)//临时变量
				{
					code.wInstruct = ECODE_PUSH;
					code.cSign = 2;
					code.cExtend = 0;
					code.dwPos = nIndexVar;
					vOut.push_back(code);
				}
				else if (m_mapDicGlobalVar.find(varName.word) != m_mapDicGlobalVar.end())//全局变量
				{
					code.wInstruct = ECODE_PUSH;
					code.cSign = 1;
					code.cExtend = 0;
					VarInfo info = m_mapDicGlobalVar[varName.word];
					code.dwPos = info.dwPos;
					vOut.push_back(code);
				}
				else
				{
					bool isStr = false;
					bool isFloat = false;
					for (unsigned int i = 0; i < varName.word.size(); i++)
					{
						if ((varName.word[i] >= '0' && varName.word[i] <= '9'))
						{
							//是数字
						}
						else if (varName.word[i] == '.')
						{
							//是点
							isFloat = true;
						}
						else
						{
							isStr = true;
							break;
						}
					}
					if (isFloat)
					{
						m_vTempCodeData.vFloatConst.push_back(stod(varName.word.c_str()));
						code.wInstruct = ECODE_PUSH;
						code.cSign = 4;
						code.cExtend = 1;
						code.dwPos = m_vTempCodeData.vFloatConst.size() - 1;
						vOut.push_back(code);
					}
					else
					{
						code.wInstruct = ECODE_PUSH;
						code.cSign = 0;
						code.cExtend = 0;
						code.dwPos = atoi(varName.word.c_str());
						vOut.push_back(code);
					}
				}
			}
		}

		return ECompile_Return;
	}

	int CScriptCodeLoader::LoadBracket(SentenceSourceCode& vIn, CBaseICode* pCode, std::vector<CodeStyle>& vOut)
	{
		GetNewWord(nextWord);
		if (nextWord.word != "(")
		{
			strError = "LoadBracket(Wrong definition format in parentheses)";
			nErrorWordPos = nextWord.nSourceWordsIndex;
			return ECompile_ERROR;
		}
		GetWord(nextWord);
		if (nextWord.word == ")")
		{
			return ECompile_Return;
		}

		if (LoadFormulaSentence(vIn, pCode, vOut) == ECompile_ERROR)
		{
			return ECompile_ERROR;
		}
		GetWord(nextWord);
		//结尾没有括号，格式错误
		if (nextWord.word != ")")
		{
			strError = "LoadBracket(Wrong definition format in parentheses)";
			nErrorWordPos = nextWord.nSourceWordsIndex;
			return ECompile_ERROR;
		}

		return ECompile_Return;
	}

	unsigned int CScriptCodeLoader::GetCodeIndex(const char* pStr)
	{
		map<string, int>::iterator it = m_mapString2CodeIndex.find(pStr);
		if (it == m_mapString2CodeIndex.end())
		{
			return -1;
		}
		return it->second;
	}

	void CScriptCodeLoader::GetGlobalVar(vector<StackVarInfo>& vOut)
	{
		vOut = vGlobalNumVar;
	}
	CScriptCodeLoader::tagCodeData* CScriptCodeLoader::GetCode(const char* pName)
	{
		map<string, int>::iterator it = m_mapString2CodeIndex.find(pName);
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
		m_vSourceWords.clear();
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
				tagSourceWordInfo wordsinfo;
				wordsinfo.nEndIndex = i;
				wordsinfo.nSourceLineIndex = m_vScoureLines.size();
				m_vSourceWords.push_back(wordsinfo);
				tagSourceLineInfo info;
				info.nLineNum = nLineIndex++;
				info.strLineWords = strCurWord;
				m_vScoureLines.push_back(info);
				strCurWord.clear();
			}
			else
			{
				ch[0] = vSource[i];
				strCurWord += ch;
			}

		}
		if (m_vScoureLines.size() > 0)
		{
			tagSourceWordInfo wordsinfo;
			wordsinfo.nEndIndex = vSource.size();
			wordsinfo.nSourceLineIndex = m_vScoureLines.size();
			m_vSourceWords.push_back(wordsinfo);
			tagSourceLineInfo info;
			info.nLineNum = nLineIndex++;
			info.nLineNum = nLineIndex++;
			info.strLineWords = strCurWord;
			m_vScoureLines.push_back(info);
			strCurWord.clear();
		}
	}
	unsigned int CScriptCodeLoader::GetSourceWordsIndex(unsigned int nIndex)
	{
		std::function<unsigned int(unsigned int, unsigned int)> fun;
		fun = [&](unsigned int BeginPos, unsigned int EndPos) {
			if (EndPos - BeginPos > 1)
			{
				unsigned int nMidPos = BeginPos + (EndPos - BeginPos) / 2;
				if (m_vSourceWords[nMidPos].nEndIndex == nIndex)
				{
					return m_vSourceWords[nMidPos].nSourceLineIndex;
				}
				else if (m_vSourceWords[nMidPos].nEndIndex > nIndex)
				{
					return fun(BeginPos, nMidPos);
				}
				else
				{
					return fun(nMidPos, EndPos);
				}
			}
			else
			{
				if (m_vSourceWords[EndPos].nEndIndex >= nIndex)
				{
					return m_vSourceWords[EndPos].nSourceLineIndex;
				}
				else
				{
					return m_vSourceWords[BeginPos].nSourceLineIndex;
				}
			}
			return  m_vScoureLines.size();
		};

		return fun(0, m_vSourceWords.size() - 1);
	}
	unsigned int CScriptCodeLoader::GetSourceLineIndex(SentenceSourceCode& vIn, unsigned int pos)
	{
		//if (vIn.size() <= 0)
		//{
		//	return 0;
		//}
		//if (pos < vIn.size())
		//{
		//	return vIn[pos].nSourceWordsIndex;
		//}
		//return vIn[vIn.size() - 1].nSourceWordsIndex;
		return 0;
	}
#endif

	std::string CScriptCodeLoader::GetSourceWords(unsigned int nIndex)
	{
#if _SCRIPT_DEBUG
		if (m_vScoureLines.size() > nIndex)
		{
			return m_vScoureLines[nIndex].strLineWords;
		}
#endif
		return std::string();
	}

	CScriptCodeLoader::tagCodeData::tagCodeData()
	{
		nType = EICODE_FUN_DEFAULT;
		nDefaultReturnType = EScriptVal_None;
	}
}