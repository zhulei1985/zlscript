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

		while (m_vCurSourceSentence.size() > nStrIndex)
		{
			if (RunCompileState(m_vCurSourceSentence, nStrIndex) == ECompile_ERROR)
			{
				zlscript::CScriptDebugPrintMgr::GetInstance()->Print("ScriptLoad Error:");
				zlscript::CScriptDebugPrintMgr::GetInstance()->Print(strCurFileName+":"+strError);
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

		for (unsigned int i = 0; i < m_vecCodeData.size(); i++)
		{
			for (unsigned int j = 0; j < m_vecCodeData[i].vNumVar.size(); j++)
			{
				VarPoint& var = m_vecCodeData[i].vNumVar[j];
				switch (var.cType)
				{
				case EScriptVal_Int:
				{
					SAFE_DELETE_ARRAY(var.pInt64);
				}
				break;
				case EScriptVal_Double:
				{
					SAFE_DELETE_ARRAY(var.pDouble);
				}
				break;
				case  EScriptVal_String:
				{
					SAFE_DELETE_ARRAY(var.pStr);
				}
				break;
				}
			}
		}
		m_vecCodeData.clear();

		for (unsigned int i = 0; i < vGlobalNumVar.size(); i++)
		{
			VarPoint& var = vGlobalNumVar[i];
			switch (var.cType)
			{
			case EScriptVal_Int:
			{
				SAFE_DELETE_ARRAY(var.pInt64);
			}
			break;
			case EScriptVal_Double:
			{
				SAFE_DELETE_ARRAY(var.pDouble);
			}
			break;
			case  EScriptVal_String:
			{
				SAFE_DELETE_ARRAY(var.pStr);
			}
			break;
			}
		}
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
		case ';'://分号作为一句的结束
			//m_vLexicalData.push_back(m_vCurSourceSentence);
			//m_vCurSourceSentence.clear();
			return EReturn | ENeedLoadNewChar;
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
	bool CScriptCodeLoader::RunCompileState(SentenceSourceCode& vIn, unsigned int& pos)
	{
		if (LoadDefineFunState(vIn, pos) == ECompile_ERROR)
		{
			return false;
		}
		return true;
	}


	int CScriptCodeLoader::LoadDefineFunState(SentenceSourceCode& vIn, unsigned int& pos)
	{
		while (m_stackBlockLayer.size())
		{
			m_stackBlockLayer.pop();
		}

		unsigned int curPos = pos;

		if (vIn.size() - curPos < 2)
		{
			strError = "LoadDefineFunState(Incomplete)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}
		int nFunType = EICODE_FUN_DEFAULT;
		string strHeadCheck = vIn[curPos].word;
		{
			auto it = m_mapDicFunToICode.find(strHeadCheck);
			if (it != m_mapDicFunToICode.end())
			{
				nFunType = it->second;
				curPos++;
			}
		}
		string strVarType = vIn[curPos++].word;
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
		string strFunName = vIn[curPos++].word;


		if (m_mapString2CodeIndex.find(strFunName) != m_mapString2CodeIndex.end())
		{
			tagCodeData& funCode = m_vecCodeData[m_mapString2CodeIndex[strFunName]];
			if (funCode.nType != EICODE_FUN_NO_CODE || !funCode.vCodeData.empty())
			{
				nErrorWordPos = curPos;
				strError = "LoadDefineFunState(Function name already exists)";
				return ECompile_ERROR;
			}

		}

		//判断变量名是否重复

		if (m_mapDicGlobalVar.find(strFunName) != m_mapDicGlobalVar.end())
		{
			nErrorWordPos = curPos;
			strError = "LoadDefineFunState(Variable name already exists)";
			return ECompile_ERROR;
		}

		if (CScriptCallBackFunion::GetFunIndex(strFunName) >= 0)
		{
			nErrorWordPos = curPos;
			strError = "LoadDefineFunState(Callback Function name already exists)";
			return ECompile_ERROR;
		}

		//如果语句结束，说明只是定义了一个全局变量
		if (vIn.size() - curPos == 0 || vIn[curPos].word != "(")
		{
			//接下来检查下一步
			if (vIn.size() - curPos > 0)
			{
				int nArraySize = 1;
				//数组声明
				if (vIn[curPos].word == "[")
				{
					if (vIn.size() - curPos >= 3 && vIn[curPos + 2].word == "]")
					{
						nArraySize = atoi(vIn[curPos + 1].word.c_str());
						curPos += 3;
					}
					else
					{
						nErrorWordPos = curPos;
						strError = "LoadDefineFunState(Array declaration)";
						return ECompile_ERROR;
					}
				}

				VarInfo info;
				info.nVarInfo = 0;
				info.cGlobal = 1;
				info.wSize = nArraySize;

				//创建全局变量
				VarPoint varpoint;
				switch (nVarType)
				{
				case EScriptVal_Int:
					info.cType = EScriptVal_Int;

					varpoint.cType = EScriptVal_Int;
					varpoint.pInt64 = new __int64[nArraySize];
					break;
				case EScriptVal_Double:
					info.cType = EScriptVal_Double;

					varpoint.cType = EScriptVal_Double;
					varpoint.pDouble = new double[nArraySize];
					break;
				case EScriptVal_String:
					info.cType = EScriptVal_String;

					varpoint.cType = EScriptVal_String;
					varpoint.pStr = new char[nArraySize * MAXSIZE_STRING];
					break;
				default:
					//检查是否是类指针
					int nClassType = CScriptSuperPointerMgr::GetInstance()->GetClassType(strVarType);
					if (nClassType > 0)
					{
						varpoint.cType = EScriptVal_ClassPointIndex;
						varpoint.unArraySize = nClassType;
					}
					else
						varpoint.cType = EScriptVal_None;
					break;
				}
				info.dwPos = vGlobalNumVar.size();
				m_mapDicGlobalVar[strFunName] = info.nVarInfo;

				vGlobalNumVar.push_back(varpoint);
				pos = curPos;

				SetVarDefaultValue(vIn, pos, info.nVarInfo);
			}
		}
		//如果接下来的是括号，说明是函数定义
		else if (vIn[curPos].word == "(")
		{
			//一个新函数，临时变量重置
			ClearStackTempVarInfo();
			NewTempVarLayer();
			//清空代码
			m_vTempCodeData.vCodeData.clear();
			m_vTempCodeData.vNumVar.clear();

			curPos += 1;

			vector<CodeStyle> vData;
			int VarIndex = 0;
			while (true)
			{
				if (vIn[curPos].word == ")")
				{
					break;
				}


				if (DefineTempVar(vIn, curPos) <= 0)
				{
					return ECompile_ERROR;
				}

				CodeStyle code;
				code.qwCode = 0;
				code.cSign = 1;
				code.wInstruct = ECODE_EVALUATE;
				code.cExtend = 0;
				code.dwPos = VarIndex;
#if _SCRIPT_DEBUG
				code.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
				vData.push_back(code);

				if (vIn[curPos].word == ",")
				{
					curPos += 1;
				}
				else if (vIn[curPos].word != ")")
				{
					nErrorWordPos = curPos;
					strError = "LoadDefineFunState(Function parameter definition format error)";
					return ECompile_ERROR;
				}
				VarIndex++;
			}

			if (vIn[curPos + 1].word == "{")
			{
				curPos = curPos + 1;
				vector<CodeStyle> vBlockCode;
				vector<CodeStyle> vBackCode;
				if (LoadBlockState(vIn, curPos, vBlockCode, vBackCode) != ECompile_ERROR)
				{

					for (unsigned int i = 0; i < vData.size(); i++)
					{
						m_vTempCodeData.vCodeData.push_back(vData[i]);
					}

					for (unsigned int i = 0; i < vBlockCode.size(); i++)
					{
						m_vTempCodeData.vCodeData.push_back(vBlockCode[i]);
					}
				}
				else
				{
					nErrorWordPos = curPos;
					return ECompile_ERROR;
				}
			}
			else
			{
				curPos++;
			}

			//将数据放入代码库
			m_vTempCodeData.nType = nFunType;
#if _SCRIPT_DEBUG
			m_vTempCodeData.filename = strCurFileName;
#endif
			m_vTempCodeData.funname = strFunName;
			if (m_mapString2CodeIndex.find(strFunName) != m_mapString2CodeIndex.end())
			{
				if (m_vTempCodeData.vCodeData.size())
				{
					tagCodeData& funCode = m_vecCodeData[m_mapString2CodeIndex[strFunName]];
					funCode = m_vTempCodeData;
				}
			}
			else
			{
				m_mapString2CodeIndex[strFunName] = m_vecCodeData.size();
				m_vecCodeData.push_back(m_vTempCodeData);
			}

			pos = curPos;
		}

		return ECompile_Return;
	}

	int CScriptCodeLoader::SetVarDefaultValue(SentenceSourceCode& vIn, unsigned int& pos, __int64 info)
	{
		VarInfo varinfo;
		varinfo.nVarInfo = info;

		if (vIn[pos].word == "=")
		{
			pos++;
			if (vIn[pos].word == "{")
			{
				//说明是给数组赋值

				int index = 0;
				VarPoint& point = vGlobalNumVar[varinfo.dwPos];
				while (true)
				{
					pos++;
					if (vIn[pos].word == "}")
					{
						break;
					}
					else if (vIn[pos].word != "," && index < varinfo.wSize)
					{
						switch (point.cType)
						{
						case EScriptVal_Int:
						{
							point.pInt64[index++] = atoi(vIn[pos].word.c_str());
						}
						break;
						case EScriptVal_Double:
						{
							point.pDouble[index++] = atof(vIn[pos].word.c_str());
						}
						break;
						case EScriptVal_String:
						{
							strcpy(point.pStr + (MAXSIZE_STRING * index), vIn[pos].word.c_str());
							index++;
						}
						break;
						case EScriptVal_ClassPointIndex:
						{
							if (vIn[pos].word == "nullptr")
							{
								point.nClassPointIndex = 0;
							}
						}
						break;
						}
					}
				}
				pos++;
			}
			else
			{

				VarPoint& point = vGlobalNumVar[varinfo.dwPos];

				switch (point.cType)
				{
				case EScriptVal_Int:
				{
					point.pInt64[0] = atoi(vIn[pos].word.c_str());
				}
				break;
				case EScriptVal_Double:
				{
					point.pDouble[0] = atof(vIn[pos].word.c_str());
				}
				break;
				case EScriptVal_String:
				{
					strcpy(point.pStr, vIn[pos].word.c_str());
				}
				break;
				}
				pos++;
			}
		}
		return ECompile_Return;
	}

	bool CScriptCodeLoader::CheckVarName(string varName)
	{
		if (varName == "")
		{
			return false;
		}
		bool bResult = true;
		//if (varName[0])
		//{
		//}

		//for (int i = 0; i < varName.size(); i++)
		//{
		//	char ch = varName[i];
		//	if (ch)
		//	{
		//	}
		//}
		return bResult;
	}
	int CScriptCodeLoader::DefineTempVar(SentenceSourceCode& vIn, unsigned int& pos/*,vector<CodeStyle>  &vOut*/)
	{
		unsigned int curPos = pos;
		string strVarType = vIn[curPos].word;
		int nParameterType = m_mapDicVarTypeToICode[vIn[curPos].word];
		string strname = vIn[curPos + 1].word;
		string strsign = vIn[curPos + 2].word;

		if (nParameterType == EICode_NONE)
		{
			if (CScriptSuperPointerMgr::GetInstance()->GetClassType(strVarType) <= 0)
			{
				nErrorWordPos = curPos;
				strError = "DefineTempVar(Class type error when defining temporary variable)";
				return ECompile_ERROR;
			}
		}
		if (!CheckVarName(strname))
		{
			nErrorWordPos = curPos;
			strError = "DefineTempVar(Illegal variable name when defining temporary variable)";
			return ECompile_ERROR;
		}
		//检查这个变量名是否已经存在
		if (CheckDefTempVar(strname.c_str()))
		{
			nErrorWordPos = curPos;
			strError = "DefineTempVar(Variable name already exists when defining temporary variable)";
			return ECompile_ERROR;
		}

		curPos += 2;
		int TempPos = curPos - 1;
		int nArraySize = 1;

		if (strsign == "[")
		{
			if (vIn.size() - curPos >= 3 && vIn[curPos + 2].word == "]")
			{
				nArraySize = atoi(vIn[curPos + 1].word.c_str());
				curPos += 3;
			}
			else
			{
				nErrorWordPos = curPos;
				strError = "DefineTempVar(Array declaration is malformed when defining temporary variables)";
				return -1;
			}
		}

		//CodeStyle defineCode;
		//defineCode.qwCode = 0;
		//defineCode.dwPos = nArraySize;

		VarPoint varpoint;
		varpoint.unArraySize = (unsigned short)nArraySize;
		switch (nParameterType)
		{
		case EScriptVal_Int:
			varpoint.cType = EScriptVal_Int;
			varpoint.pInt64 = new __int64[nArraySize];

			//defineCode.wInstruct = ECODE_Define_INT;
			break;
		case EScriptVal_Double:
			varpoint.cType = EScriptVal_Double;
			varpoint.pDouble = new double[nArraySize];

			//defineCode.wInstruct = ECODE_Define_INT;
			break;
		case EScriptVal_String:
			varpoint.cType = EScriptVal_String;
			varpoint.pStr = new char[nArraySize * MAXSIZE_STRING];

			//defineCode.wInstruct = ECODE_Define_INT;
			break;
		default:
			//检查是否是类指针
			int nClassType = CScriptSuperPointerMgr::GetInstance()->GetClassType(strVarType);
			if (nClassType > 0)
			{
				varpoint.cType = EScriptVal_ClassPointIndex;
				varpoint.unArraySize = nClassType;
			}
			else
				varpoint.cType = EScriptVal_None;

			//defineCode.wInstruct = ECODE_NONE;
			break;

		}
		//int nReturn = m_vTempCodeData.vNumVar.size();
		//tagVarName2Pos &mapDic = m_stackTempVarInfo.top();
		//mapDic[strname] = m_vTempCodeData.vNumVar.size();
		NewTempVarIndex(strname.c_str(), m_vTempCodeData.vNumVar.size());
		m_vTempCodeData.vNumVar.push_back(varpoint);


		//vOut.push_back(defineCode);
		//如果接下来是赋值，倒回
		if (vIn[curPos].word == "=")
		{
			curPos = TempPos;
		}
		//else if (vIn[curPos].word == ")")
		//{
		//	curPos = TempPos;
		//}
		pos = curPos;
		return ECompile_Return;
	}

	int CScriptCodeLoader::QueryTempVar(string varName)
	{
		//检查这个变量名是否已经存在

		return GetTempVarIndex(varName.c_str());

	}

	int CScriptCodeLoader::LoadBlockState(SentenceSourceCode& vIn, unsigned int& pos, vector<CodeStyle>& vOut, vector<CodeStyle> vBackCode)
	{
		m_stackBlockLayer.push(0);
		unsigned int curPos = pos;

		if (vIn.size() - curPos <= 0)
		{
			nErrorWordPos = curPos;
			strError = "LoadBlockState(Block format error, insufficient length)";
			return ECompile_ERROR;
		}
		if (vIn[curPos].word != "{")
		{
			nErrorWordPos = curPos;
			strError = "LoadBlockState(Block format error)";
			return ECompile_ERROR;
		}
		NewTempVarLayer();
		vector<CodeStyle> vTempCode;

		curPos++;

		int nReturn = 0;
		while (vIn.size() > curPos)
		{
			if (vIn[curPos].word == "}")
			{
				curPos++;
				break;
			}
			else if (vIn[curPos].word == "{")
			{
				vector<CodeStyle> vBackCode;
				nReturn = LoadBlockState(vIn, curPos, vTempCode, vBackCode);
			}
			else if (vIn[curPos].word == "if")
			{
				nReturn = LoadIfSentence(vIn, curPos, vTempCode);
			}
			else if (vIn[curPos].word == "else")
			{
				nReturn = LoadIfSentence(vIn, curPos, vTempCode);
			}
			else if (vIn[curPos].word == "for")
			{

			}
			else if (vIn[curPos].word == "while")
			{
				nReturn = LoadWhileSentence(vIn, curPos, vTempCode);
			}
			else if (vIn[curPos].word == "switch")
			{

			}
			else if (vIn[curPos].word == "int" || vIn[curPos].word == "float" || vIn[curPos].word == "string")
			{
				nReturn = DefineTempVar(vIn, curPos);
			}
			else if (CScriptSuperPointerMgr::GetInstance()->GetClassType(vIn[curPos].word) > 0)
			{
				nReturn = DefineTempVar(vIn, curPos);
			}
			else if (vIn[curPos].word == "return")
			{
				curPos++;
				CodeStyle ClearCode;
				ClearCode.qwCode = 0;
				ClearCode.wInstruct = ECODE_CLEAR_PARAM;
#if _SCRIPT_DEBUG
				ClearCode.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
				vTempCode.push_back(ClearCode);

				nReturn = LoadReturnFormulaRecursion(vIn, curPos, vTempCode);

				CodeStyle breakCode;
				breakCode.qwCode = 0;
				breakCode.wInstruct = ECODE_RETURN;
				breakCode.dwPos = vTempCode.size();
#if _SCRIPT_DEBUG
				breakCode.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
				vTempCode.push_back(breakCode);
			}
			else if (vIn[curPos].word == "break")
			{
				curPos++;
				CodeStyle breakCode;
				breakCode.qwCode = 0;
				breakCode.wInstruct = ECODE_BREAK;
				breakCode.dwPos = vTempCode.size();
#if _SCRIPT_DEBUG
				breakCode.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
				vTempCode.push_back(breakCode);

				nReturn = ECompile_Return;
			}
			else if (vIn[curPos].word == "delete")//释放一个类实例
			{
				curPos++;
				//先检查是否是类指针
				int nClassIndex = QueryTempVar(vIn[curPos].word);
				char cSign = 0;
				VarPoint* pVar = nullptr;
				if (nClassIndex >= 0)
				{
					pVar = &m_vTempCodeData.vNumVar[nClassIndex];
					cSign = 2;
				}
				else if (m_mapDicGlobalVar.find(vIn[curPos].word) != m_mapDicGlobalVar.end())
				{
					VarInfo info;
					info.nVarInfo = m_mapDicGlobalVar[vIn[curPos].word];
					nClassIndex = info.dwPos;
					pVar = &vGlobalNumVar[nClassIndex];
					cSign = 1;
				}
				if (pVar && pVar->cType == EScriptVal_ClassPointIndex)
				{
					CodeStyle code;
					code.qwCode = 0;
#if _SCRIPT_DEBUG
					code.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
					code.wInstruct = ECODE_PUSH;
					code.cSign = cSign;
					code.cExtend = 0;
					code.dwPos = nClassIndex;
					vTempCode.push_back(code);
				}
				else
				{
					//不是类指针，错误
					nReturn == ECompile_ERROR;
				}

				CodeStyle deleteCode;
				deleteCode.qwCode = 0;
				deleteCode.wInstruct = ECODE_RELEASE_CLASS;
				deleteCode.dwPos = vTempCode.size();
#if _SCRIPT_DEBUG
				deleteCode.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
				vTempCode.push_back(deleteCode);

				nReturn = ECompile_Return;
				curPos++;
			}
			else
			{
				if (m_mapString2CodeIndex.find(vIn[curPos].word) != m_mapString2CodeIndex.end())
				{
					nReturn = LoadCallFunState(vIn, curPos, vTempCode);
				}
				//回调函数
				else if (CScriptCallBackFunion::GetFunIndex(vIn[curPos].word) >= 0)
				{
					nReturn = LoadCallFunState(vIn, curPos, vTempCode);
				}
				else
				{
					//什么都不是，那就可能是算式了
					nReturn = LoadFormulaSentence(vIn, curPos, vTempCode);
				}

			}
			if (nReturn == ECompile_ERROR)
			{
				nErrorWordPos = curPos;
				strError = "LoadBlockState(Statement classification error)";
				return ECompile_ERROR;
			}
		}


		//将预订的代码压入
		for (unsigned int i = 0; i < vBackCode.size(); i++)
		{
			vTempCode.push_back(vBackCode[i]);
		}

		//将代码压入输出
		CodeStyle code;
		code.qwCode = 0;
		code.wInstruct = ECODE_BLOCK;
		code.dwPos = vTempCode.size();
#if _SCRIPT_DEBUG
		code.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
		vOut.push_back(code);

		for (unsigned int i = 0; i < vTempCode.size(); i++)
		{
			vOut.push_back(vTempCode[i]);
		}

		pos = curPos;

		RemoveTempVarLayer();
		m_stackBlockLayer.pop();
		return ECompile_Return;
	}

	int CScriptCodeLoader::LoadCallFunState(SentenceSourceCode& vIn, unsigned int& pos, vector<CodeStyle>& vOut)
	{
		unsigned int curPos = pos;

		if (vIn.size() - curPos <= 0)
		{
			strError = "LoadCallFunState(Function call, statement too short)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}
		string FunName = vIn[curPos].word;
		vector<CodeStyle> vCode;

		CodeStyle callCode;
		callCode.qwCode = 0;
		callCode.wInstruct = ECODE_CALL;
#if _SCRIPT_DEBUG
		callCode.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif

		int nCallFunIndex = CScriptCallBackFunion::GetFunIndex(FunName);
		if (nCallFunIndex >= 0)
		{
			callCode.cSign = 0;
			callCode.dwPos = nCallFunIndex;
		}
		else if (m_mapString2CodeIndex.find(FunName) != m_mapString2CodeIndex.end())
		{
			callCode.cSign = 1;
			callCode.dwPos = m_mapString2CodeIndex[FunName];
		}
		else
		{
			strError = "LoadCallFunState(Function call, function not found)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}


		curPos++;
		if (vIn.size() - curPos <= 0)
		{
			strError = "LoadCallFunState(Function call, statement too short)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}
		if (vIn[curPos].word == "(")
		{
			//函数参数要倒着取才行

			stack<tagCodeSection> sTempCode;
			int nParamNum = 0;
			while (true)
			{
				curPos++;
				if (vIn.size() - curPos <= 0)
				{
					strError = "LoadCallFunState(Function call, parameter statement too short)";
					nErrorWordPos = curPos;
					return ECompile_ERROR;
				}
				if (vIn[curPos].word == ")")
				{
					curPos++;
					break;
				}
				else if (vIn[curPos].word == ",")
				{

				}
				else if (vIn[curPos + 1].word == "," || vIn[curPos + 1].word == ")")
				{
					//直接读取变量和常量
					tagCodeSection vTemp;
					if (LoadAndPushNumVar(vIn, curPos, vTemp) == ECompile_ERROR)
					{
						return ECompile_ERROR;
					}
					sTempCode.push(vTemp);
					curPos--;
					nParamNum++;
				}
				else
				{
					tagCodeSection vTemp;
					if (LoadFormulaSentence(vIn, curPos, vTemp) == ECompile_ERROR)
					{
						return ECompile_ERROR;
					}
					sTempCode.push(vTemp);
					curPos--;
					nParamNum++;
				}
			}

			while (!sTempCode.empty())
			{
				tagCodeSection& vTemp = sTempCode.top();
				for (unsigned int i = 0; i < vTemp.size(); i++)
				{
					vCode.push_back(vTemp[i]);
				}
				sTempCode.pop();
			}
			callCode.cExtend = nParamNum;
		}
		vCode.push_back(callCode);

		for (unsigned int i = 0; i < vCode.size(); i++)
		{
			vOut.push_back(vCode[i]);
		}
		pos = curPos;
		return ECompile_Return;
	}
	int CScriptCodeLoader::LoadIfSentence(SentenceSourceCode& vIn, unsigned int& pos, vector<CodeStyle>& vOut)
	{
		unsigned int curPos = pos;
		if (vIn.size() - curPos <= 0)
		{
			nErrorWordPos = curPos;
			strError = "LoadIfSentence(If statement definition format error)";
			return ECompile_ERROR;
		}
		if (vIn[curPos].word != "if")
		{
			nErrorWordPos = curPos;
			strError = "LoadIfSentence(If statement definition format error, not if statement)";
			return ECompile_ERROR;
		}

		CodeStyle ifcode;
		ifcode.qwCode = 0;
		ifcode.wInstruct = ECODE_BRANCH_IF;
		ifcode.cSign = (unsigned char)m_stackBlockLayer.size();
#if _SCRIPT_DEBUG
		ifcode.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif

		//读取条件
		vector<CodeStyle> vBracketCode;
		curPos++;
		CodeStyle varTypeCode;
		varTypeCode.qwCode = 0;
		varTypeCode.wInstruct = ECODE_INT;
#if _SCRIPT_DEBUG
		varTypeCode.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
		vBracketCode.push_back(varTypeCode);

		vOut.push_back(varTypeCode);
		if (LoadBracket(vIn, curPos, vBracketCode) == ECompile_ERROR)
		{
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}

		//读取内容
		vector<CodeStyle> vBlockCode;
		vBlockCode.clear();
		if (vIn[curPos].word != "{")
		{
			nErrorWordPos = curPos;
			strError = "LoadIfSentence(If statement definition format error)";
			return ECompile_ERROR;
		}
		vector<CodeStyle> vBackCode;
		if (LoadBlockState(vIn, curPos, vBlockCode, vBackCode) == ECompile_ERROR)
		{
			return ECompile_ERROR;
		}

		for (unsigned int i = 0; i < vBracketCode.size(); i++)
		{
			vOut.push_back(vBracketCode[i]);
		}

		ifcode.dwPos = vBlockCode.size() + 1;
		vOut.push_back(ifcode);

		for (unsigned int i = 0; i < vBlockCode.size(); i++)
		{
			vOut.push_back(vBlockCode[i]);
		}

		if (vIn[curPos].word == "else")
		{
			CodeStyle elsecode;
			elsecode.qwCode = 0;
#if _SCRIPT_DEBUG
			elsecode.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
			elsecode.wInstruct = ECODE_BRANCH_ELSE;
			elsecode.cSign = (unsigned char)m_stackBlockLayer.size();
			curPos++;
			
			vector<CodeStyle> vElseCode;
			if (vIn[curPos].word == "{")
			{
				vector<CodeStyle> vBackCode;
				if (LoadBlockState(vIn, curPos, vElseCode, vBackCode) == ECompile_ERROR)
				{
					return ECompile_ERROR;
				}
			}
			else if(vIn[curPos].word == "if")
			{
				if (LoadIfSentence(vIn, curPos, vElseCode) == ECompile_ERROR)
				{
					return ECompile_ERROR;
				}
			}
			else
			{
				strError = "LoadIfSentence(If statement definition format error)";
				return ECompile_ERROR;
			}
			elsecode.dwPos = 1 + vElseCode.size();
			vOut.push_back(elsecode);
			for (unsigned int i = 0; i < vElseCode.size(); i++)
			{
				vOut.push_back(vElseCode[i]);
			}
		}

		pos = curPos;

		return ECompile_Return;
	}
	//读取while语句
	int CScriptCodeLoader::LoadWhileSentence(SentenceSourceCode& vIn, unsigned int& pos, vector<CodeStyle>& vOut)
	{
		unsigned int curPos = pos;
		unsigned int nSize = 0;

		if (vIn.size() - curPos <= 0)
		{
			strError = "LoadWhileSentence(While statement read failed)";
			return ECompile_ERROR;
		}

		if (vIn[curPos].word != "while")
		{
			strError = "LoadWhileSentence(While statement format error)";
			return ECompile_ERROR;
		}
		curPos++;

		vector<CodeStyle> vTempCode;
		CodeStyle varTypeCode;
		varTypeCode.qwCode = 0;
		varTypeCode.wInstruct = ECODE_INT;
#if _SCRIPT_DEBUG
		varTypeCode.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
		vTempCode.push_back(varTypeCode);
		if (LoadBracket(vIn, curPos, vTempCode) == ECompile_ERROR)
		{
			return ECompile_ERROR;
		}

		for (unsigned int i = 0; i < vTempCode.size(); i++)
		{
			vOut.push_back(vTempCode[i]);
		}
		nSize += vTempCode.size();

		vTempCode.clear();

		//要在块尾加入返回块头的指令
		vector<CodeStyle> vBackCode;
		CodeStyle backcode;
		backcode.qwCode = 0;
		backcode.wInstruct = ECODE_BLOCK_CYC;
#if _SCRIPT_DEBUG
		backcode.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
		vBackCode.push_back(backcode);
		if (LoadBlockState(vIn, curPos, vTempCode, vBackCode) == ECompile_ERROR)
		{
			return ECompile_ERROR;
		}

		//给ECODE_BLOCK_CYC设置返回的量
		nSize += vTempCode.size();
		vTempCode[vTempCode.size() - 1].dwPos = nSize;

		CodeStyle code;
		code.qwCode = 0;
		code.wInstruct = ECODE_CYC_IF;
		code.dwPos = vTempCode.size() + 1;
#if _SCRIPT_DEBUG
		code.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
		vOut.push_back(code);

		for (unsigned int i = 0; i < vTempCode.size(); i++)
		{
			vOut.push_back(vTempCode[i]);
		}

		pos = curPos;
		return ECompile_Return;
	}
	int CScriptCodeLoader::LoadAndPushVar(SentenceSourceCode& vIn, unsigned int& pos, vector<CodeStyle>& vOut)
	{
		unsigned int curPos = pos;
		if (vIn.size() - curPos > 0)
		{
			string strName = vIn[curPos].word;

			//脚本函数
			if (vIn.size() - curPos > 2 && vIn[curPos + 1].word == "(")
			{
				//脚本函数
				if (m_mapString2CodeIndex.find(strName) != m_mapString2CodeIndex.end())
				{
					LoadCallFunState(vIn, curPos, vOut);
				}
				//回调函数
				else if (CScriptCallBackFunion::GetFunIndex(strName) >= 0)
				{
					LoadCallFunState(vIn, curPos, vOut);
				}
				else
				{
					//TODO:检查函数调用语句是否完整，定义新函数
					m_mapString2CodeIndex[strName] = m_vecCodeData.size();
					tagCodeData data;
					data.funname = strName;
					data.nType = EICODE_FUN_NO_CODE;
					m_vecCodeData.push_back(data);
					//然后再读取
					LoadCallFunState(vIn, curPos, vOut);
				}
			}
			else
			{
				int nArraySize = 0;
				CodeStyle code;
				code.qwCode = 0;
#if _SCRIPT_DEBUG
				code.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
				int nIndexVar = QueryTempVar(strName);
				if (nIndexVar >= 0)//临时变量
				{
					code.wInstruct = ECODE_PUSH;
					code.cSign = 2;
					code.cExtend = nArraySize;
					code.dwPos = nIndexVar;
					vOut.push_back(code);
				}
				else if (m_mapDicGlobalVar.find(strName) != m_mapDicGlobalVar.end())//全局变量
				{
					code.wInstruct = ECODE_PUSH;
					code.cSign = 1;
					code.cExtend = nArraySize;
					VarInfo info;
					info.nVarInfo = m_mapDicGlobalVar[strName];
					code.dwPos = info.dwPos;
					vOut.push_back(code);
				}
				else if (strName[0] == '\"')//字符串
				{
					char strbuff[MAXSIZE_STRING];
					memset(strbuff, 0, sizeof(strbuff));
					int strIndex = 0;
					for (unsigned int i = 1; i < strName.size() - 1; i++)
					{
						if (strIndex < MAXSIZE_STRING - 1)
						{
							strbuff[strIndex++] = strName[i];
						}
					}
					m_vTempCodeData.vStrConst.push_back(strbuff);
					code.wInstruct = ECODE_PUSH;
					code.cSign = 3;
					code.cExtend = 0;
					code.dwPos = m_vTempCodeData.vStrConst.size() - 1;
					vOut.push_back(code);
				}
				else
				{
					code.wInstruct = ECODE_PUSH;
					code.cSign = 0;
					code.cExtend = 0;
					code.dwPos = atoi(strName.c_str());
					vOut.push_back(code);
				}
				curPos++;
			}

			pos = curPos;
			return ECompile_Return;
		}
		else
		{
			return ECompile_ERROR;
		}
	}
	int CScriptCodeLoader::LoadAndPushNumVar(SentenceSourceCode& vIn, unsigned int& pos, vector<CodeStyle>& vOut)
	{
		unsigned int curPos = pos;
		if (vIn.size() - curPos > 0)
		{
			string strName = vIn[curPos].word;
			if (vIn.size() - curPos > 2 && vIn[curPos+1].word=="(")
			{
				//脚本函数
				if (m_mapString2CodeIndex.find(strName) != m_mapString2CodeIndex.end())
				{
					LoadCallFunState(vIn, curPos, vOut);
				}
				//回调函数
				else if (CScriptCallBackFunion::GetFunIndex(strName) >= 0)
				{
					LoadCallFunState(vIn, curPos, vOut);
				}
				else
				{
					//TODO:检查函数调用语句是否完整，定义新函数
					m_mapString2CodeIndex[strName] = m_vecCodeData.size();
					tagCodeData data;
					data.funname = strName;
					data.nType = EICODE_FUN_NO_CODE;
					m_vecCodeData.push_back(data);
					//然后再读取
					LoadCallFunState(vIn, curPos, vOut);
				}
			}
			else if (vIn[curPos].nFlag == E_WORD_FLAG_STRING)
			{
				//字符串
				CodeStyle code;
				m_vTempCodeData.vStrConst.push_back(vIn[curPos].word);
				code.wInstruct = ECODE_PUSH;
				code.cSign = 3;
				code.cExtend = 0;
				code.dwPos = m_vTempCodeData.vStrConst.size() - 1;
#if _SCRIPT_DEBUG
				code.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
				vOut.push_back(code);
				pos++;
				return ECompile_Return;
			}
			else
			{
				int nArraySize = 0;
				CodeStyle code;
				code.qwCode = 0;
#if _SCRIPT_DEBUG
				code.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
				int nIndexVar = QueryTempVar(strName);
				if (nIndexVar >= 0)//临时变量
				{
					code.wInstruct = ECODE_PUSH;
					code.cSign = 2;
					code.cExtend = nArraySize;
					code.dwPos = nIndexVar;
					vOut.push_back(code);
				}
				else if (m_mapDicGlobalVar.find(strName) != m_mapDicGlobalVar.end())//全局变量
				{
					code.wInstruct = ECODE_PUSH;
					code.cSign = 1;
					code.cExtend = nArraySize;
					VarInfo info;
					info.nVarInfo = m_mapDicGlobalVar[strName];
					code.dwPos = info.dwPos;
					vOut.push_back(code);
				}
				else
				{
					bool isStr = false;
					bool isFloat = false;
					for (unsigned int i = 0; i < strName.size(); i++)
					{
						if ((strName[i] >= '0' && strName[i] <= '9'))
						{
							//是数字
						}
						else if (strName[i] == '.')
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
						m_vTempCodeData.vFloatConst.push_back(stod(strName.c_str()));
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
						code.dwPos = atoi(strName.c_str());
						vOut.push_back(code);
					}
				}
				curPos++;
			}

			pos = curPos;
			return ECompile_Return;
		}
		else
		{
			strError = "LoadWhileSentence(Constant or variable read error)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}
	}
	int CScriptCodeLoader::LoadAndPushClassPoint(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut)
	{
		unsigned int curPos = pos;
		if (vIn.size() - curPos > 0)
		{
			string strName = vIn[curPos].word;
			if (vIn[curPos].nFlag == E_WORD_FLAG_STRING)
			{
				//不接受字符串
				strError = "LoadWhileSentence(String is not accepted when reading a class pointer)";
				nErrorWordPos = curPos;
				return ECompile_ERROR;
			}
			if (vIn.size() - curPos > 2 && vIn[curPos + 1].word == "(")
			{
				//脚本函数
				if (m_mapString2CodeIndex.find(strName) != m_mapString2CodeIndex.end())
				{
					LoadCallFunState(vIn, curPos, vOut);
				}
				//回调函数
				else if (CScriptCallBackFunion::GetFunIndex(strName) >= 0)
				{
					LoadCallFunState(vIn, curPos, vOut);
				}
				else
				{
					//TODO:检查函数调用语句是否完整，定义新函数
					m_mapString2CodeIndex[strName] = m_vecCodeData.size();
					tagCodeData data;
					data.funname = strName;
					data.nType = EICODE_FUN_NO_CODE;
					m_vecCodeData.push_back(data);
					//然后再读取
					LoadCallFunState(vIn, curPos, vOut);
				}
			}
			else if (vIn[curPos + 1].word == "->")
			{
				if (LoadCallClassFunn(vIn, curPos, vOut) == ECompile_ERROR)
				{
					//生成压值进堆栈的代码失败
					return ECompile_ERROR;
				}
			}
			else
			{
				int nArraySize = 0;
				CodeStyle code;
				code.qwCode = 0;
#if _SCRIPT_DEBUG
				code.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
				int nIndexVar = QueryTempVar(strName);
				if (nIndexVar >= 0)//临时变量
				{
					code.wInstruct = ECODE_PUSH;
					code.cSign = 2;
					code.cExtend = nArraySize;
					code.dwPos = nIndexVar;
					vOut.push_back(code);
				}
				else if (m_mapDicGlobalVar.find(strName) != m_mapDicGlobalVar.end())//全局变量
				{
					code.wInstruct = ECODE_PUSH;
					code.cSign = 1;
					code.cExtend = nArraySize;
					VarInfo info;
					info.nVarInfo = m_mapDicGlobalVar[strName];
					code.dwPos = info.dwPos;
					vOut.push_back(code);
				}
				else
				{
					//不接受常量
					strError = "LoadWhileSentence(Constants are not accepted when reading class pointers)";
					nErrorWordPos = curPos;
					return ECompile_ERROR;
				}
				curPos++;
			}

			pos = curPos;
			return ECompile_Return;
		}
		else
		{
			strError = "LoadWhileSentence(Format error reading class pointer)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}
	}
	int CScriptCodeLoader::LoadCallClassFunn(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut)
	{
		CodeStyle classcode;
		classcode.qwCode = 0;
		classcode.wInstruct = ECODE_PUSH;
#if _SCRIPT_DEBUG
		classcode.nSourseWordIndex = GetSourceLineIndex(vIn, pos);
#endif
		CodeStyle callCode;
		callCode.qwCode = 0;
		callCode.wInstruct = ECODE_CALL_CLASS_FUN;
#if _SCRIPT_DEBUG
		callCode.nSourseWordIndex = GetSourceLineIndex(vIn, pos);
#endif
		unsigned int curPos = pos;
		if (vIn.size() - curPos < 3)
		{
			return ECompile_ERROR;
		}

		int nClassIndex = QueryTempVar(vIn[curPos].word);
		VarPoint* pVar = nullptr;
		if (nClassIndex >= 0)
		{
			pVar = &m_vTempCodeData.vNumVar[nClassIndex];
			classcode.cSign = 2;
			classcode.cExtend = 0;
			classcode.dwPos = nClassIndex;
		}
		else if (m_mapDicGlobalVar.find(vIn[curPos].word) != m_mapDicGlobalVar.end())
		{
			VarInfo info;
			info.nVarInfo = m_mapDicGlobalVar[vIn[curPos].word];
			nClassIndex = info.dwPos;
			pVar = &vGlobalNumVar[nClassIndex];
			classcode.cSign = 1;
			classcode.cExtend = 0;
			classcode.dwPos = nClassIndex;
		}
		if (pVar && pVar->cType == EScriptVal_ClassPointIndex)
		{
			//callCode.dwPos = pVar->nClassPointIndex;
		}
		else
		{
			strError = "LoadCallClassFunn(Non class pointer when calling class function)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}
		curPos++;
		if (vIn[curPos].word != "->")
		{
			strError = "LoadCallClassFunn(When calling a class function, the class pointer cannot have an operator other than)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}
		curPos++;
		int nFunIndex = CScriptSuperPointerMgr::GetInstance()->GetClassFunIndex(pVar->unArraySize, vIn[curPos].word);
		if (nFunIndex < 0)
		{
			strError = "LoadCallClassFunn(Class function not registered when calling class function)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}
		callCode.dwPos = nFunIndex;
		curPos++;
		if (vIn.size() - curPos <= 0)
		{
			strError = "LoadCallClassFunn(Insufficient statement length when calling class function)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}
		vector<CodeStyle> vCode;
		if (vIn[curPos].word == "(")
		{
			//函数参数要倒着取才行

			stack<tagCodeSection> sTempCode;
			int nParamNum = 0;
			while (true)
			{
				curPos++;
				if (vIn.size() - curPos <= 0)
				{
					strError = "LoadCallClassFunn(When calling a class function, the length of the statement taking the parameter is insufficient)";
					nErrorWordPos = curPos;
					return ECompile_ERROR;
				}
				if (vIn[curPos].word == ")")
				{
					curPos++;
					break;
				}
				else if (vIn[curPos].word == ",")
				{

				}
				else if (vIn[curPos + 1].word == "," || vIn[curPos + 1].word == ")")
				{
					//直接读取变量和常量
					tagCodeSection vTemp;
					if (LoadAndPushNumVar(vIn, curPos, vTemp) == ECompile_ERROR)
					{
						return ECompile_ERROR;
					}
					sTempCode.push(vTemp);
					curPos--;
					nParamNum++;
				}
				else
				{
					tagCodeSection vTemp;
					if (LoadFormulaSentence(vIn, curPos, vTemp) == ECompile_ERROR)
					{
						return ECompile_ERROR;
					}
					sTempCode.push(vTemp);
					curPos--;
					nParamNum++;
				}
			}

			while (!sTempCode.empty())
			{
				tagCodeSection& vTemp = sTempCode.top();
				for (unsigned int i = 0; i < vTemp.size(); i++)
				{
					vCode.push_back(vTemp[i]);
				}
				sTempCode.pop();
			}
			callCode.cExtend = nParamNum;
		}
		vCode.push_back(classcode);
		vCode.push_back(callCode);

		for (unsigned int i = 0; i < vCode.size(); i++)
		{
			vOut.push_back(vCode[i]);
		}
		pos = curPos;
		return ECompile_Return;
	}
	int CScriptCodeLoader::LoadFormulaSentence(SentenceSourceCode& vIn, unsigned int& pos, vector<CodeStyle>& vOut)
	{

		unsigned int curPos = pos;
		if (vIn.size() - curPos < 2)
		{
			strError = "LoadFormulaSentence(Wrong format of arithmetic statement definition)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}

		int nArraySize = 0;
		bool isEvaluate = false;
		bool isClassPoint = false;
		int nVarType = EScriptVal_None;
		CodeStyle code;
		code.qwCode = 0;
#if _SCRIPT_DEBUG
		code.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
		if (vIn[curPos + 1].word == "=")
		{
			//先查询临时变量
			int nVarIndex = QueryTempVar(vIn[curPos].word);
			if (nVarIndex >= 0)
			{
				code.wInstruct = ECODE_EVALUATE;
				code.cSign = 1;
				code.cExtend = nArraySize;
				code.dwPos = nVarIndex;

				VarPoint& pVar = m_vTempCodeData.vNumVar[nVarIndex];
				if (pVar.cType == EScriptVal_None)
				{
					strError = "LoadFormulaSentence(Temporary variable query failed)";
					nErrorWordPos = curPos;
					return ECompile_ERROR;
				}

				//CodeStyle varTypeCode;
				//varTypeCode.qwCode = 0;
				nVarType = pVar.cType;
				switch (pVar.cType)
				{
				case EScriptVal_Int:
					//varTypeCode.wInstruct = ECODE_INT;
					break;
				case EScriptVal_Double:
					//varTypeCode.wInstruct = ECODE_DOUDLE;
					break;
				case EScriptVal_String:
					//varTypeCode.wInstruct = ECODE_STRING;
					break;
				case EScriptVal_ClassPointIndex:
					//varTypeCode.wInstruct = ECODE_CLASSPOINT;
					isClassPoint = true;
					break;
				}
				//vOut.push_back(varTypeCode);
			}
			else if (m_mapDicGlobalVar.find(vIn[curPos].word) != m_mapDicGlobalVar.end())
			{
				VarInfo info;
				info.nVarInfo = m_mapDicGlobalVar[vIn[curPos].word];
				nVarIndex = info.dwPos;
				code.wInstruct = ECODE_EVALUATE;
				code.cSign = 0;
				code.cExtend = nArraySize;
				code.dwPos = nVarIndex;

				VarPoint pVar = vGlobalNumVar[nVarIndex];
				if (pVar.cType == EScriptVal_None)
				{
					strError = "LoadFormulaSentence(Global variable query failed)";
					nErrorWordPos = curPos;
					return ECompile_ERROR;
				}

				nVarType = pVar.cType;
				//CodeStyle varTypeCode;
				//varTypeCode.qwCode = 0;
				switch (pVar.cType)
				{
				case EScriptVal_Int:
					//varTypeCode.wInstruct = ECODE_INT;
					break;
				case EScriptVal_Double:
					//varTypeCode.wInstruct = ECODE_DOUDLE;
					break;
				case EScriptVal_String:
					//varTypeCode.wInstruct = ECODE_STRING;
					break;
				case EScriptVal_ClassPointIndex:
					//varTypeCode.wInstruct = ECODE_CLASSPOINT;
					isClassPoint = true;
					break;
				}
				//vOut.push_back(varTypeCode);
			}
			else
			{
				strError = "LoadFormulaSentence(Wrong assignment object)";
				nErrorWordPos = curPos;
				return ECompile_ERROR;
			}

			isEvaluate = true;
			curPos = curPos + 2;
		}
		//if (isStr)
		//{

		//}
		//else
		if (isClassPoint)
		{
			std::string strName = vIn[curPos].word;
			if (strName == "new")
			{
				if (vIn.size() - curPos < 2)
				{
					//错误
					return ECompile_ERROR;
				}
				std::string strClassName = vIn[curPos+1].word;
				CodeStyle newCode;
				newCode.qwCode = 0;
				newCode.wInstruct = ECODE_NEW_CLASS;
				newCode.dwPos = CScriptSuperPointerMgr::GetInstance()->GetClassType(strClassName);
#if _SCRIPT_DEBUG
				newCode.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
				vOut.push_back(newCode);
				curPos += 2;
			}
			else if (vIn.size() - curPos > 2 && vIn[curPos + 1].word == "(")
			{
				//脚本函数
				if (m_mapString2CodeIndex.find(strName) != m_mapString2CodeIndex.end())
				{
					LoadCallFunState(vIn, curPos, vOut);
				}
				//回调函数
				else if (CScriptCallBackFunion::GetFunIndex(strName) >= 0)
				{
					LoadCallFunState(vIn, curPos, vOut);
				}
				else
				{
					//TODO:检查函数调用语句是否完整，定义新函数
					m_mapString2CodeIndex[strName] = m_vecCodeData.size();
					tagCodeData data;
					data.funname = strName;
					data.nType = EICODE_FUN_NO_CODE;
					m_vecCodeData.push_back(data);
					//然后再读取
					LoadCallFunState(vIn, curPos, vOut);
				}
			}
			else
			{
				int nClassIndex = QueryTempVar(vIn[curPos].word);
				VarPoint* pVar = nullptr;
				if (nClassIndex >= 0)
				{
					pVar = &m_vTempCodeData.vNumVar[nClassIndex];
				}
				else if (m_mapDicGlobalVar.find(vIn[curPos].word) != m_mapDicGlobalVar.end())
				{
					VarInfo info;
					info.nVarInfo = m_mapDicGlobalVar[vIn[curPos].word];
					nClassIndex = info.dwPos;
					pVar = &vGlobalNumVar[nClassIndex];
				}
				if (pVar && pVar->cType == EScriptVal_ClassPointIndex)
				{
					if (LoadAndPushClassPoint(vIn, curPos, vOut) == ECompile_ERROR)
					{
						return ECompile_ERROR;
					}
				}
				else
				{
					strError = "LoadFormulaSentence(Class pointer variable should be handled)";
					nErrorWordPos = curPos;
					return ECompile_ERROR;
				}

			}
		}
		else
		{
			if (LoadFormulaRecursion(vIn, curPos, vOut, nVarType) == ECompile_ERROR)
			{
				return ECompile_ERROR;
			}
		}

		if (isEvaluate)
		{
			vOut.push_back(code);
		}


		pos = curPos;
		return ECompile_Return;
	}

	int CScriptCodeLoader::LoadFormulaRecursion(SentenceSourceCode& vIn, unsigned int& pos, vector<CodeStyle>& vOut, int nVarType, int nLevel)
	{
		//TODO 添加算式默认类型
		unsigned int curPos = pos;
		if (vIn.size() - curPos <= 0)
		{
			strError = "LoadFormulaSentence(Wrong format of arithmetic statement definition)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}

		//if (vIn[curPos].word == "(")
		//{
		//	//一上来就遇到了括号
		//	if (LoadBracket(vIn,curPos,vOut) == ECompile_ERROR)
		//	{
		//		return ECompile_ERROR;
		//	}
		//}

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
#if _SCRIPT_DEBUG
			unsigned int nSourseWordIndex;
#endif
			void addCode(vector<CodeStyle>& vOut)
			{
				CBaseNode::addCode(vOut);

				CodeStyle code;
				code.qwCode = 0;
				code.wInstruct = unSign;
#if _SCRIPT_DEBUG
				code.nSourseWordIndex = nSourseWordIndex;
#endif
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
		while (vIn.size() - curPos > 1)
		{
			CBaseNode* pNode = nullptr;
			if (vIn[curPos].word == "(")
			{
				nState = 1;
				CVarNode* pVarNode = new CVarNode;
				//unsigned int nTempPos = curPos;
				if (LoadBracket(vIn, curPos, vOut) == ECompile_ERROR)
				{
					nResult = ECompile_ERROR;
					break;
				}
				nLastNodeType = E_NODE_VAR;
				pNode = pVarNode;
			}
			else if (m_mapDicSignToPRI.find(vIn[curPos].word) == m_mapDicSignToPRI.end())
			{
				if (nState == 1)
				{
					nResult = ECompile_Return;
					break;
				}
				nState = 1;
				CVarNode* pVarNode = new CVarNode;
				nLastNodeType = E_NODE_VAR;
				//unsigned int nTempPos = curPos;
				//先检查是否是类指针
				int nClassIndex = QueryTempVar(vIn[curPos].word);
				VarPoint* pVar = nullptr;
				if (nClassIndex >= 0)
				{
					pVar = &m_vTempCodeData.vNumVar[nClassIndex];
				}
				else if (m_mapDicGlobalVar.find(vIn[curPos].word) != m_mapDicGlobalVar.end())
				{
					VarInfo info;
					info.nVarInfo = m_mapDicGlobalVar[vIn[curPos].word];
					nClassIndex = info.dwPos;
					pVar = &vGlobalNumVar[nClassIndex];
				}
				if (pVar && pVar->cType == EScriptVal_ClassPointIndex)
				{
					if (vIn[curPos + 1].word == "->")
					{
						if (LoadCallClassFunn(vIn, curPos, pVarNode->vTempCode) == ECompile_ERROR)
						{
							//生成压值进堆栈的代码失败
							nResult = ECompile_ERROR;
							break;
						}
					}
					else
					{
						//strError = "类指针不能参与四则运算";
						//nResult = ECompile_ERROR;
						//break;
						CodeStyle code;
						code.qwCode = 0;
#if _SCRIPT_DEBUG
						code.nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
						code.wInstruct = ECODE_PUSH;
						code.cSign = 2;
						code.cExtend = 0;
						code.dwPos = nClassIndex;
						pVarNode->vTempCode.push_back(code);
						curPos++;
					}
				}
				else if (LoadAndPushNumVar(vIn, curPos, pVarNode->vTempCode) == ECompile_ERROR)
				{
					//生成压值进堆栈的代码失败
					nResult = ECompile_ERROR;
					break;
				}
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
					pSignNode->unSign = m_mapDicSingleSignToEnum[vIn[curPos].word];
				}
				else
				{
					pSignNode->nLevel = m_mapDicSignToPRI[vIn[curPos].word];
					pSignNode->unSign = m_mapDicSignToEnum[vIn[curPos].word];
				}
				nLastNodeType = E_NODE_SIGN;
#if _SCRIPT_DEBUG
				pSignNode->nSourseWordIndex = GetSourceLineIndex(vIn, curPos);
#endif
				pNode = pSignNode;
				curPos++;
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
		//默认变量类型
	//	CodeStyle varTypeCode;
	//	varTypeCode.qwCode = 0;
	//#if _SCRIPT_DEBUG
	//	varTypeCode.nSourseWordIndex = GetSourceWordsIndex(vIn, curPos);
	//#endif
	//	switch (nVarType)
	//	{
	//	case EScriptVal_Int:
	//		varTypeCode.wInstruct = ECODE_INT;
	//		break;
	//	case EScriptVal_Double:
	//		varTypeCode.wInstruct = ECODE_DOUDLE;
	//		break;
	//	case EScriptVal_String:
	//		varTypeCode.wInstruct = ECODE_STRING;
	//		break;
	//	}
	//	vOut.push_back(varTypeCode);

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
		pos = curPos;
		return nResult;
	}

	int CScriptCodeLoader::LoadReturnFormulaRecursion(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut)
	{
		unsigned int curPos = pos;
		if (m_nCurFunVarType == EScriptVal_ClassPointIndex)
		{
			//脚本函数
			std::string strName = vIn[curPos].word;
			if (vIn.size() - curPos > 2 && vIn[curPos + 1].word == "(")
			{
				//脚本函数
				if (m_mapString2CodeIndex.find(strName) != m_mapString2CodeIndex.end())
				{
					LoadCallFunState(vIn, curPos, vOut);
				}
				//回调函数
				else if (CScriptCallBackFunion::GetFunIndex(strName) >= 0)
				{
					LoadCallFunState(vIn, curPos, vOut);
				}
				else
				{
					//TODO:检查函数调用语句是否完整，定义新函数
					m_mapString2CodeIndex[strName] = m_vecCodeData.size();
					tagCodeData data;
					data.funname = strName;
					data.nType = EICODE_FUN_NO_CODE;
					m_vecCodeData.push_back(data);
					//然后再读取
					LoadCallFunState(vIn, curPos, vOut);
				}
			}
			else
			{
				int nClassIndex = QueryTempVar(vIn[curPos].word);
				VarPoint* pVar = nullptr;
				if (nClassIndex >= 0)
				{
					pVar = &m_vTempCodeData.vNumVar[nClassIndex];
				}
				else if (m_mapDicGlobalVar.find(vIn[curPos].word) != m_mapDicGlobalVar.end())
				{
					VarInfo info;
					info.nVarInfo = m_mapDicGlobalVar[vIn[curPos].word];
					nClassIndex = info.dwPos;
					pVar = &vGlobalNumVar[nClassIndex];
				}
				if (pVar && pVar->cType == EScriptVal_ClassPointIndex)
				{
					if (LoadAndPushClassPoint(vIn, curPos, vOut) == ECompile_ERROR)
					{
						return ECompile_ERROR;
					}
				}
				else
				{
					strError = "LoadReturnFormulaRecursion(Class pointer variable should be handled)";
					nErrorWordPos = curPos;
					return ECompile_ERROR;
				}

			}
		}
		else
		{
			if (LoadFormulaRecursion(vIn, curPos, vOut, m_nCurFunVarType) == ECompile_ERROR)
			{
				return ECompile_ERROR;
			}
		}

		pos = curPos;
		return ECompile_Return;
	}

	int CScriptCodeLoader::LoadBracket(SentenceSourceCode& vIn, unsigned int& pos, vector<CodeStyle>& vOut)//读取括号内
	{
		unsigned int curPos = pos;
		if (vIn.size() - curPos <= 0)
		{
			strError = "LoadBracket(Wrong definition format in parentheses)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}

		//先确认一下是在括号里
		if (vIn[curPos].word != "(")
		{
			strError = "LoadBracket(Wrong definition format in parentheses)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}
		curPos++;

		//如果接下来就是括号结束
		if (vIn[curPos].word == ")")
		{
			curPos++;
			pos = curPos;
			return ECompile_Return;
		}

		if (vIn.size() - curPos > 0)
		{
			if (LoadFormulaRecursion(vIn, curPos, vOut, EScriptVal_None) == ECompile_ERROR)
			{
				return ECompile_ERROR;
			}
		}

		if (vIn.size() - curPos <= 0)
		{
			strError = "LoadBracket(Wrong definition format in parentheses)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}

		//结尾没有括号，格式错误
		if (vIn[curPos].word != ")")
		{
			strError = "LoadBracket(Wrong definition format in parentheses)";
			nErrorWordPos = curPos;
			return ECompile_ERROR;
		}

		curPos++;
		pos = curPos;
		return ECompile_Return;
	}
	void CScriptCodeLoader::GetGlobalVar(vector<CScriptCodeLoader::VarPoint>& vOut)
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

	void CScriptCodeLoader::ClearStackTempVarInfo()
	{
		while (!m_ListTempVarInfo.empty())
			m_ListTempVarInfo.pop_back();
	}

	void CScriptCodeLoader::NewTempVarLayer()
	{
		tagVarName2Pos mapdic;
		m_ListTempVarInfo.push_back(mapdic);
	}

	void CScriptCodeLoader::RemoveTempVarLayer()
	{
		if (m_ListTempVarInfo.size())
			m_ListTempVarInfo.pop_back();
	}

	bool CScriptCodeLoader::CheckDefTempVar(const char* pStr)
	{
		auto rit = m_ListTempVarInfo.rbegin();
		if (rit != m_ListTempVarInfo.rend())
		{
			tagVarName2Pos& mapdic = *rit;
			auto itDic = mapdic.find(pStr);
			if (itDic != mapdic.end())
			{
				return true;
			}
		}
		return false;
	}

	unsigned int CScriptCodeLoader::GetTempVarIndex(const char* pStr)
	{
		if (pStr == nullptr)
		{
			return m_nTempVarIndexError;
		}
		unsigned int result = m_nTempVarIndexError;
		auto rit = m_ListTempVarInfo.rbegin();
		for (; rit != m_ListTempVarInfo.rend(); rit++)
		{
			tagVarName2Pos& mapdic = *rit;
			auto itDic = mapdic.find(pStr);
			if (itDic != mapdic.end())
			{
				return itDic->second;
			}
		}
		return result;
	}

	bool CScriptCodeLoader::NewTempVarIndex(const char* pStr, unsigned int nIndex)
	{
		if (pStr == nullptr)
		{
			return false;
		}
		tagVarName2Pos& mapDic = m_ListTempVarInfo.back();
		mapDic[pStr] = nIndex;
		return true;
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
		std::function<unsigned int(unsigned int , unsigned int)> fun;
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
		
		return fun(0, m_vSourceWords.size()-1);
	}
	unsigned int CScriptCodeLoader::GetSourceLineIndex(SentenceSourceCode& vIn, unsigned int pos)
	{
		if (vIn.size() <= 0)
		{
			return 0;
		}
		if (pos < vIn.size())
		{
			return vIn[pos].nSourceWordsIndex;
		}
		return vIn[vIn.size() - 1].nSourceWordsIndex;
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
	}
}