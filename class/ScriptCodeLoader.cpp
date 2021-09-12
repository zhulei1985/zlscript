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
		//添加顺序很重要，会影响编译结果
		AddCodeCompile<CDefGlobalVarICode>(E_CODE_SCOPE_OUTSIDE);
		AddCodeCompile<CFunICode>(E_CODE_SCOPE_OUTSIDE);
		AddCodeCompile<CBlockICode>(E_CODE_SCOPE_STATEMENT);

		AddCodeCompile<CDefTempVarICode>(E_CODE_SCOPE_STATEMENT);
		AddCodeCompile<CTestSignICode>(E_CODE_SCOPE_STATEMENT);
		AddCodeCompile<CIfICode>(E_CODE_SCOPE_STATEMENT);
		AddCodeCompile<CWhileICode>(E_CODE_SCOPE_STATEMENT);
		AddCodeCompile<CContinueICode>(E_CODE_SCOPE_STATEMENT);
		AddCodeCompile<CBreakICode>(E_CODE_SCOPE_STATEMENT);
		AddCodeCompile<CReturnICode>(E_CODE_SCOPE_STATEMENT);
		AddCodeCompile<CDeleteICode>(E_CODE_SCOPE_STATEMENT);
		AddCodeCompile<CSentenceICode>(E_CODE_SCOPE_STATEMENT);

		AddCodeCompile<CExpressionICode>(E_CODE_SCOPE_EXPRESSION);

		AddCodeCompile<CBracketsICode>(E_CODE_SCOPE_MEMBER);
		AddCodeCompile<CNewICode>(E_CODE_SCOPE_MEMBER);
		AddCodeCompile<CCallFunICode>(E_CODE_SCOPE_MEMBER);
		AddCodeCompile<CCallClassFunICode>(E_CODE_SCOPE_MEMBER);
		AddCodeCompile<CMinusICode>(E_CODE_SCOPE_MEMBER);
		AddCodeCompile<CGetClassParamICode>(E_CODE_SCOPE_MEMBER);
		AddCodeCompile<CLoadVarICode>(E_CODE_SCOPE_MEMBER);

		AddCodeCompile<COperatorICode>(E_CODE_SCOPE_OPERATOR);
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
			if (!RunCompileState(m_vCurSourceSentence, E_CODE_SCOPE_OUTSIDE, nullptr,0))
			{
				zlscript::CScriptDebugPrintMgr::GetInstance()->Print("ScriptLoad Error:");

				for (unsigned int i = 0; i < m_vError.size(); i++)
				{
					zlscript::CScriptDebugPrintMgr::GetInstance()->Print(strCurFileName + ":" + m_vError[i].strError);
#ifdef  _SCRIPT_DEBUG
					auto souceInfo = GetSourceWords(m_vError[i].nErrorWordPos);
					zlscript::CScriptDebugPrintMgr::GetInstance()->Print("Debug", "file:%s,line:%d,word:%s",
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
		m_mapDicVarTypeToICode["void"] = EScriptVal_Void;
		m_mapDicVarTypeToICode["int"] = EScriptVal_Int;
		m_mapDicVarTypeToICode["float"] = EScriptVal_Double;
		m_mapDicVarTypeToICode["string"] = EScriptVal_String;

		m_mapDicFunToICode["intact"] = EICODE_FUN_CAN_BREAK;
	}
	unsigned short CScriptCodeLoader::GetVarType(tagSourceWord varWord)
	{
		if (varWord.nFlag == E_WORD_FLAG_STRING)
		{
			return EScriptVal_String;
		}
		bool isFloat = false;
		for (unsigned int i = 0; i < varWord.word.size(); i++)
		{
			if ((varWord.word[i] >= '0' && varWord.word[i] <= '9'))
			{
				//是数字
			}
			else if (varWord.word[i] == '.')
			{
				//是点
				isFloat = true;
			}
			else
			{
				return EScriptVal_None;
			}
		}
		if (isFloat)
		{
			return EScriptVal_Double;
		}

		return EScriptVal_Int;
	}
	unsigned short CScriptCodeLoader::GetVarType(std::string type, unsigned short& classtype)
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



	bool CScriptCodeLoader::RunCompileState(SentenceSourceCode& vIn, E_CODE_SCOPE scopeType, CBaseICode* pFather, int addType)
	{
		SignToPos();

		auto& list = m_mapICodeMgr[scopeType];
		bool bResult = false;
		for (auto it = list.begin(); it != list.end(); it++)
		{
			auto pMgr = *it;
			if (pMgr)
			{
				auto pICode = pMgr->New(this, nBeginSourceWordIndex);
				if (pICode)
				{
					pICode->SetFather(pFather);
					if (pICode->Compile(vIn))
					{
						if (pFather)
						{
							pFather->AddICode(addType, pICode);
						}
						bResult = true;
						break;
					}
					pMgr->Release(pICode);
				}
			}
		}
		//if (LoadDefineFunState(vIn) == ECompile_ERROR)
		//{
		//	return false;
		//}
		if (bResult)
		{
			ClearErrorInfo();
		}
		return bResult;
	}


	bool CScriptCodeLoader::AddGlobalVar(std::string name, unsigned short type, unsigned short typeExtend)
	{
		if (m_mapDicGlobalVar.find(name) != m_mapDicGlobalVar.end())
		{
			if ((int)m_mapDicGlobalVar[name].cType != (char)type
				|| m_mapDicGlobalVar[name].wExtend != typeExtend)
			{
				return false;
			}
		}
		else
		{
			VarInfo info;
			info.cType = (char)type;
			info.cGlobal = 1;
			if ((char)type == EScriptVal_ClassPoint)
				info.wExtend = typeExtend;
			else
				info.wExtend = 0;
			info.dwPos = vGlobalNumVar.size();
			m_mapDicGlobalVar[name] = info;
			StackVarInfo defVar;//默认值
			defVar.cType = (char)type;
			vGlobalNumVar.push_back(defVar);
		}
		return true;
	}

	bool CScriptCodeLoader::SetGlobalVar(std::string name, StackVarInfo& var)
	{
		auto it = m_mapDicGlobalVar.find(name);
		if (it != m_mapDicGlobalVar.end())
		{
			if (it->second.dwPos >= vGlobalNumVar.size())
			{
				return false;
			}
			vGlobalNumVar[it->second.dwPos] = var;
			return true;
		}
		return false;
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
	tagCodeData* CScriptCodeLoader::GetCode(const char* pName)
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


	bool CScriptCodeLoader::SetFunICode(std::string name, CFunICode* pCode)
	{
		CFunICode* pOld = m_mapString2Code[name];
		if (pOld == nullptr)
		{
			m_mapString2Code[name] = pCode;
			return true;
		}
		else if (pOld && pCode)
		{
			if (pOld->vBodyCode.empty())
			{
				m_mapString2Code[name] = pCode;
				return true;
			}
		}
		return false;
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
				code.m_mapFunAttribute = pCode->m_mapFunAttribute;
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
		//CICodeMgr::GetInstance()->Clear();
		for (auto itMap = m_mapICodeMgr.begin(); itMap != m_mapICodeMgr.end(); itMap++)
		{
			ListICodeMgr &listMgr = itMap->second;
			for (auto itList = listMgr.begin(); itList != listMgr.end(); itList++)
			{
				CBaseICodeMgr* pMgr = *itList;
				if (pMgr)
				{
					pMgr->Clear();
				}
			}
		}
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
			for (auto pCode = data.pBeginCode; pCode; pCode = pCode->m_pNext)
			{

				std::string str = pCode->GetCodeString();// = PrintOneCode(pCode);
				char strbuff[32] = { 0 };
				sprintf(strbuff, "[%d]\t", pCode->nCodeIndex);
				fputs(strbuff, fp);
				fputs(str.c_str(), fp);
#ifdef _SCRIPT_DEBUG
				if (curSoureWordIndex != pCode->nSoureWordIndex)
				{
					curSoureWordIndex = pCode->nSoureWordIndex;
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

	//std::string CScriptCodeLoader::PrintOneCode(CodeStyle code)
	//{
	//	char strbuff[128] = { 0 };
	//	std::string str;
	//	switch (code.wInstruct)
	//	{
	//	case ECODE_ADD: //加
	//		sprintf(strbuff, "ADD\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_SUM: //减
	//		sprintf(strbuff, "SUM\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_MUL: //乘
	//		sprintf(strbuff, "MUL\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_DIV://除
	//		sprintf(strbuff, "DIV\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_MOD: //求余
	//		sprintf(strbuff, "MOD\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_MINUS: //	取负数
	//		sprintf(strbuff, "MINUS\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_CMP_EQUAL://比较
	//		sprintf(strbuff, "CMP(==)\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_CMP_NOTEQUAL:
	//		sprintf(strbuff, "CMP(!=)\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_CMP_BIG:
	//		sprintf(strbuff, "CMP(>)\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_CMP_BIGANDEQUAL:
	//		sprintf(strbuff, "CMP(>=)\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_CMP_LESS:
	//		sprintf(strbuff, "CMP(<)\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_CMP_LESSANDEQUAL:
	//		sprintf(strbuff, "CMP(<=)\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//		//位运算
	//	case ECODE_BIT_AND:
	//		sprintf(strbuff, "AND\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_BIT_OR:
	//		sprintf(strbuff, "OR\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_BIT_XOR:
	//		sprintf(strbuff, "XOR\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//		/*************功能符************/
	//		// 压入变量到堆栈。
	//		//	cSign:	使用ESignType的定义.变量来源
	//		//	dwPos:	根据cSign的值表示值或地址
	//	case ECODE_PUSH:
	//		sprintf(strbuff, "PUSH\ttVarType(sign):%s\tpos:%d", GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//		// 提取堆栈中的变量。
	//		//	cSign:	使用ESignType的定义，变量去处
	//		//	cExtend:寄存器索引ERegisterIndex
	//		//	dwPos:	根据cSign的值表示值或地址
	//	case ECODE_POP:
	//		sprintf(strbuff, "POP\ttVarType(sign):%s\tpos:%d", GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//	case ECODE_STATEMENT_END: //语句结束
	//		sprintf(strbuff, "STATEND\t");
	//		break;
	//		//读取变量到寄存器。
	//		//	cSign:	使用ESignType的定义
	//		//	cExtend:寄存器索引ERegisterIndex
	//		//	dwPos:	根据cSign的值表示值或地址
	//	case ECODE_LOAD:
	//		sprintf(strbuff, "LOAD\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//		// 移动寄存器的值
	//		//	cSign:	目的地类型
	//		//	cExtend:起点，寄存器索引ERegisterIndex
	//		//	dwPos:	终点，根据cSign的值表示值或地址
	//	case ECODE_MOVE:
	//		sprintf(strbuff, "MOVE\tRegisterIndex(extend):%s\tVarType(sign):%s\tpos:%d", GetRegisterName(code.cExtend).c_str(), GetSignPosTypeName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//		//读取类成员变量到寄存器。
	//		//	cSign:	类对象所在寄存器 ERegisterIndex
	//		//	cExtend:结果放入的寄存器 ERegisterIndex
	//		//	dwPos:	类成员变量的索引
	//	case ECODE_GET_CLASS_PARAM:
	//		sprintf(strbuff, "GET PARAM\tResultReg(extend):%s\tclassReg(sign):%s\tParamIdx(pos):%d", GetRegisterName(code.cExtend).c_str(), GetRegisterName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//		// 设置寄存器内容到类成员变量。
	//		//	cSign:	类对象所在寄存器 ERegisterIndex
	//		//	cExtend:数值所在的寄存器 ERegisterIndex
	//		//	dwPos:	类成员变量的索引
	//	case ECODE_SET_CLASS_PARAM:
	//		sprintf(strbuff, "SET PARAM\tVarReg(extend):%s\tclassReg(sign):%s\tParamIdx(pos):%d", GetRegisterName(code.cExtend).c_str(), GetRegisterName(code.cSign).c_str(), (int)code.dwPos);
	//		break;
	//		//ECODE_JMP,//无条件跳转
	//		//ECODE_JMP_JUDGE,//m_JudgeRegister为真跳转

	//		//ECODE_BEGIN_CALL,//开始计算本次函数调用实际压入多少个参数
	//		//调用回调函数
	//		//cSign:0,返回值存放的寄存器 ERegisterIndex
	//		//cExtend:参数数量
	//		//dwPos:函数索引
	//	case ECODE_CALL_CALLBACK:
	//		sprintf(strbuff, "CALL BACK\tResultReg(sign):%s\tParamSize(extend):%d\tFunIndex(pos):%d", GetRegisterName(code.cSign).c_str(), (int)code.cExtend, (int)code.dwPos);
	//		break;
	//		//调用脚本函数 
	//		//cSign:0,返回值存放的寄存器 ERegisterIndex
	//		//cExtend:参数数量
	//		//dwPos:函数索引
	//	case ECODE_CALL_SCRIPT:
	//		sprintf(strbuff, "CALL\tResultReg(sign):%s\tParamSize(extend):%d\tFunIndex(pos):%d", GetRegisterName(code.cSign).c_str(), (int)code.cExtend, (int)code.dwPos);
	//		break;
	//		// 跳转
	//		//cSign:0,绝对地址跳转
	//		//		1,相对地址向后跳转
	//		//		2,相对地址向前跳转
	//		//dwPos:地址值
	//	case ECODE_JUMP:
	//		sprintf(strbuff, "JUMP\ttype(sign):%d\tpos:%d", (int)code.cSign, (int)code.dwPos);
	//		break;
	//		// 寄存器值为真跳转
	//		//cSign:0,绝对地址跳转
	//		//		1,相对地址向后跳转
	//		//		2,相对地址向前跳转
	//		//cExtend:寄存器索引 ERegisterIndex
	//		//dwPos:地址值
	//	case ECODE_JUMP_TRUE:
	//		sprintf(strbuff, "JUMP_T\tVarReg(extend):%s\ttype(sign):%d\tpos:%d", GetRegisterName(code.cSign).c_str(), (int)code.cExtend, (int)code.dwPos);
	//		break;
	//		// 寄存器值为假跳转
	//		//cSign:0,绝对地址跳转
	//		//		1,相对地址向后跳转
	//		//		2,相对地址向前跳转
	//		//cExtend:寄存器索引 ERegisterIndex
	//		//dwPos:地址值
	//	case ECODE_JUMP_FALSE:
	//		sprintf(strbuff, "JUMP_F\tVarReg(extend):%s\ttype(sign):%d\tpos:%d", GetRegisterName(code.cSign).c_str(), (int)code.cExtend, (int)code.dwPos);
	//		break;
	//		// if分支,检查指定寄存器上的值，如果非0则执行接下来的块
	//		//cSign:0,判断值存放的寄存器 ERegisterIndex
	//		//dwPos:本分支下执行的代码块大小
	//		//ECODE_BRANCH_IF,
	//		//ECODE_BRANCH_JUMP,
	//		//ECODE_BRANCH_ELSE,	// else分支，如果之前的if没有执行，则执行
	//		//ECODE_CYC_IF,		//专门用于循环条件的判断
	//		//ECODE_BLOCK,	//块开始标志 cSign: cExtend: dwPos:表示块大小
	//		//ECODE_BLOCK_CYC,//放在块尾，执行到此返回块头
	//		//ECODE_BREAK,	//退出此块
	//		// 退出函数
	//		//	cExtend:返回值所在寄存器索引ERegisterIndex
	//	case ECODE_RETURN:	//退出函数
	//		sprintf(strbuff, "RETURN\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
	//		break;
	//	case ECODE_CLEAR_PARAM://清空堆栈里的参数
	//		sprintf(strbuff, "CLEAR\t");
	//		break;
	//		//调用类函数 
	//		// cSign:	类对象所在寄存器，返回值也会写入在此
	//		// cExtend:	参数数量,
	//		// dwPos:	类函数索引
	//	case ECODE_CALL_CLASS_FUN:
	//		sprintf(strbuff, "CALL CLASS\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
	//		break;
	//		// 新建一个类实例
	//		//cSign:结果放入寄存器索引
	//		//dwPos:类类型Index
	//	case ECODE_NEW_CLASS: //新建一个类实例
	//		sprintf(strbuff, "NEW\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
	//		break;
	//		// 释放一个类实例
	//		//cSign:使用ESignType的定义.变量来源
	//		//dwPos:根据cSign的值获取类指针所在的地址
	//	case ECODE_RELEASE_CLASS://释放一个类实例
	//		sprintf(strbuff, "RELERASE\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
	//		break;
	//		//下面是中间代码
	//	case ECODE_BREAK:
	//		sprintf(strbuff, "BREAK\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
	//		break;
	//	case ECODE_CONTINUE:
	//		sprintf(strbuff, "CONTINUE\tsign:%d\textend:%d\tpos:%d", (int)code.cSign, (int)code.cExtend, (int)code.dwPos);
	//		break;
	//	}

	//	return strbuff;
	//}

	//std::string CScriptCodeLoader::GetSignPosTypeName(char Idx)
	//{
	//	switch (Idx)
	//	{
	//	case ESIGN_VALUE_INT:
	//		return "VALUE_INT";
	//	case ESIGN_POS_GLOBAL_VAR:
	//		return "GLOBAL_VAR";
	//	case ESIGN_POS_LOACL_VAR:
	//		return "LOACL_VAR";
	//	case ESIGN_POS_CONST_STRING:
	//		return "CONST_STRING";
	//	case ESIGN_POS_CONST_FLOAT:
	//		return "CONST_FLOAT";
	//	case ESIGN_POS_CONST_INT64:
	//		return "CONST_INT64";
	//	case ESIGN_REGISTER:
	//		return "REGISTER";
	//	}
	//	return "unknow";
	//}

	//std::string CScriptCodeLoader::GetRegisterName(char regIdx)
	//{
	//	switch (regIdx)
	//	{
	//	case R_A:
	//		return "R_A";
	//	case R_B:
	//		return "R_B";
	//	case R_C:
	//		return "R_C";
	//	case R_D:
	//		return "R_D";
	//	}
	//	return "unknow";
	//}

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
		m_vCharIndex2LineIndex.resize(vSource.size(), 0);
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

	const CScriptCodeLoader::tagSourceLineInfo& CScriptCodeLoader::GetSourceWords(unsigned int nIndex)
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

}