#include <string>
#include "ScriptIntermediateCode.h"
#include "ScriptCompiler.h"

namespace zlscript
{
	CScriptCompiler::CScriptCompiler()
	{
		InitLexicalAnalysisFun();
		InitCodeCompile();
	}
	CScriptCompiler::~CScriptCompiler()
	{
	}

	unsigned short CScriptCompiler::GetVarType(tagSourceWord varWord)
	{
		if (varWord.nFlag == E_WORD_FLAG_STRING)
		{
			return EScriptVal_String;
		}
		else if (varWord.nFlag == E_WORD_FLAG_NUMBER)
		{
			for (unsigned int i = 0; i < varWord.word.size(); i++)
			{
				if (varWord.word[i] == '.')
				{
					//是浮点
					return EScriptVal_Double;
				}
			}
			return EScriptVal_Int;
		}
		return EScriptVal_None;
	}

	bool CScriptCompiler::LexicalAnalysis(char* pData, unsigned int size)
	{
		std::string strbuff;
		for (unsigned int i = 0; i < size; )
		{
			char ch = pData[i];
			auto listIt = m_mapLAFun.find(pData[i]);
			if (listIt == m_mapLAFun.end() || listIt->second.empty())
			{
				i++;
			}
			else
			{
				auto list = listIt->second;
				for (auto it = list.begin(); it != list.end(); it++)
				{
					LexicalAnalysisFun pFun = *it;
					unsigned int beginIndex = i;
					if (pFun(pData, size, i))
					{
						break;
					}
					else
					{
						i = beginIndex;
					}
				}
			}
		}
		return true;
	}
	bool CScriptCompiler::Compile(char* pData, unsigned int size)
	{

#if _SCRIPT_DEBUG
		PartitionSourceWords(pData, size);
#endif


		if (!LexicalAnalysis(pData, size))
		{
			return false;
		}
		
		while (m_vCurSourceSentence.size())
		{
			if (!RunCompileState(m_vCurSourceSentence, E_CODE_SCOPE_OUTSIDE, nullptr, 0))
			{
//				zlscript::CScriptDebugPrintMgr::GetInstance()->Print("ScriptLoad Error:");
//
//				for (unsigned int i = 0; i < m_vError.size(); i++)
//				{
//					zlscript::CScriptDebugPrintMgr::GetInstance()->Print(strCurFileName + ":" + m_vError[i].strError);
//#ifdef  _SCRIPT_DEBUG
//					auto souceInfo = GetSourceWords(m_vError[i].nErrorWordPos);
//					zlscript::CScriptDebugPrintMgr::GetInstance()->Print("Debug", "file:%s,line:%d,word:%s",
//						souceInfo.strCurFileName.c_str(), souceInfo.nLineNum, souceInfo.strLineWords.c_str());
//#endif //  _SCRIPT_DEBUG
//				}

				break;
			}
		}
		return true;
	}

	void CScriptCompiler::InitLexicalAnalysisFun()
	{
		auto RegisterFun = [&](char ch,auto fun, bool bPushFront = false) {
			auto &list = m_mapLAFun[ch];
			if (bPushFront)
			{
				list.push_front(std::bind(fun, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			}
			else
			{
				list.push_back(std::bind(fun, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			}
		};
		RegisterFun('_',&CScriptCompiler::LoadKeyState);
		for (char ch = '0'; ch <= '9'; ch++)
		{
			RegisterFun(ch, &CScriptCompiler::LoadNumberState);
		}
		for (char ch = 'A'; ch <= 'Z'; ch++)
		{
			RegisterFun(ch, &CScriptCompiler::LoadKeyState);
		}
		for (char ch = 'a'; ch <= 'z'; ch++)
		{
			RegisterFun(ch, &CScriptCompiler::LoadKeyState);
		}

		RegisterFun('"', &CScriptCompiler::LoadStringState);
		RegisterFun('/', &CScriptCompiler::LoadSkipAnnotateState);
		RegisterFun('/', &CScriptCompiler::LoadSkipAnnotate2State);

		RegisterFun('+', &CScriptCompiler::LoadSignState);
		RegisterFun('-', &CScriptCompiler::LoadSignState);
		RegisterFun('!', &CScriptCompiler::LoadSignState);
		RegisterFun('=', &CScriptCompiler::LoadSignState);
		RegisterFun('<', &CScriptCompiler::LoadSignState);
		RegisterFun('>', &CScriptCompiler::LoadSignState);
		RegisterFun('*', &CScriptCompiler::LoadSignState);
		RegisterFun('/', &CScriptCompiler::LoadSignState);
		RegisterFun('%', &CScriptCompiler::LoadSignState);
		RegisterFun('(', &CScriptCompiler::LoadSignState);
		RegisterFun(')', &CScriptCompiler::LoadSignState);
		RegisterFun('{', &CScriptCompiler::LoadSignState);
		RegisterFun('}', &CScriptCompiler::LoadSignState);
		RegisterFun('[', &CScriptCompiler::LoadSignState);
		RegisterFun(']', &CScriptCompiler::LoadSignState);
		RegisterFun('@', &CScriptCompiler::LoadSignState);
		RegisterFun(';', &CScriptCompiler::LoadSignState);
		RegisterFun(':', &CScriptCompiler::LoadSignState);
		RegisterFun('.', &CScriptCompiler::LoadSignState);
		RegisterFun(',', &CScriptCompiler::LoadSignState);
	}

	bool CScriptCompiler::LoadSignState(char* pData, unsigned int size, unsigned int& index)
	{
		std::string strOut;
		char ch = pData[index];
		switch (ch)
		{
		case '+':
			strOut.push_back(ch);
			index++;
			if (pData[index] == '+')
			{
				strOut.push_back(pData[index]);
				index++;
			}
			break;
		case '-':
			strOut.push_back(ch);
			index++;
			if (pData[index] == '>')
			{
				strOut.push_back(pData[index]);
				index++;
			}
			break;
		case '!':
			strOut.push_back(ch);
			index++;
			if (pData[index] == '=')
			{
				strOut.push_back(pData[index]);
				index++;
			}
			break;
		case '=':
			strOut.push_back(ch);
			index++;
			if (pData[index] == '=')
			{
				strOut.push_back(pData[index]);
				index++;
			}
			break;
		case '<':
			strOut.push_back(ch);
			index++;
			if (pData[index] == '=')
			{
				strOut.push_back(pData[index]);
				index++;
			}
			break;
		case '>':
			strOut.push_back(ch);
			index++;
			if (pData[index] == '=')
			{
				strOut.push_back(pData[index]);
				index++;
			}
			break;
		case '*':
		case '/':
		case '%':
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
		case '@':
		case ';':
		case ':':
		case '.':
		case ',':
			index++;
			strOut.push_back(ch);
			break;
		default:
			return false;
		}
		tagSourceWord word;
		word.word = strOut;
		word.nFlag = E_WORD_FLAG_NORMAL;
#if _SCRIPT_DEBUG
		word.nSourceWordsIndex = GetSourceWordsIndex(index);
#endif
		m_vCurSourceSentence.push_back(word);
		return true;
	}

	bool CScriptCompiler::LoadKeyState(char* pData, unsigned int size, unsigned int& index)
	{
		auto checkFun = [](char ch) {
			if (ch == '_')
			{
				return true;
			}
			if (ch >= 'A' && ch <= 'Z')
			{
				return true;
			}
			if (ch >= 'a' && ch <= 'z')
			{
				return true;
			}
			if (ch >= '0' && ch <= '9')
			{
				return true;
			}
			return false;
		};
		std::string strOut;
		while (index < size)
		{
			char ch = pData[index];
			if (checkFun(ch))
			{
				index++;
				strOut.push_back(ch);
			}
			else
			{
				tagSourceWord word;
				word.word = strOut;
				word.nFlag = E_WORD_FLAG_NORMAL;
#if _SCRIPT_DEBUG
				word.nSourceWordsIndex = GetSourceWordsIndex(index);
#endif
				m_vCurSourceSentence.push_back(word);
				return true;
			}
		}
		return false;
	}

	bool CScriptCompiler::LoadNumberState(char* pData, unsigned int size, unsigned int& index)
	{
		auto checkFun = [](char ch) {
			if (ch >= '0' && ch <= '9')
			{
				return true;
			}
			return false;
		};
		std::string strOut;
		if (pData[index] == '0' && (index+1 < size))
		{
			if (pData[index + 1] == 'x')
			{
				strOut.push_back(pData[index++]);
				strOut.push_back(pData[index++]);
			}
			else if (!checkFun(pData[index + 1]))
			{
				strOut.push_back(pData[index++]);
				tagSourceWord word;
				word.word = strOut;
				word.nFlag = E_WORD_FLAG_NUMBER;
#if _SCRIPT_DEBUG
				word.nSourceWordsIndex = GetSourceWordsIndex(index);
#endif
				m_vCurSourceSentence.push_back(word);
				return true;
			}
		}

		bool hasPoint = false;
		while (index < size)
		{
			if (pData[index] == '.')
			{
				if (hasPoint == false)
				{
					hasPoint = true;
					strOut.push_back(pData[index++]);
				}
				else
				{
					//只能有一个‘.’
					return false;
				}
			}
			else if (checkFun(pData[index]))
			{
				strOut.push_back(pData[index++]);
			}
			else
			{
				break;
			}
		}
		tagSourceWord word;
		word.word = strOut;
		word.nFlag = E_WORD_FLAG_NUMBER;
#if _SCRIPT_DEBUG
		word.nSourceWordsIndex = GetSourceWordsIndex(index);
#endif
		m_vCurSourceSentence.push_back(word);
		return true;
	}

	bool CScriptCompiler::LoadStringState(char* pData, unsigned int size, unsigned int& index)
	{
		if (pData[index] != '"')
		{
			return false;
		}
		std::string strOut;
		index++;
		while (index < size)
		{
			if (pData[index] == '"')
			{
				index++;
				tagSourceWord word;
				word.word = strOut;
				word.nFlag = E_WORD_FLAG_STRING;
#if _SCRIPT_DEBUG
				word.nSourceWordsIndex = GetSourceWordsIndex(index);
#endif
				m_vCurSourceSentence.push_back(word);
				return true;
			}
			strOut.push_back(pData[index]);
			index++;
		}
		return false;
	}

	bool CScriptCompiler::LoadSkipAnnotateState(char* pData, unsigned int size, unsigned int& index)
	{
		if (pData[index++] != '/')
		{
			return false;
		}
		if (pData[index++] != '/')
		{
			return false;
		}
		
		while (index < size)
		{
			if (pData[index] == 13)
			{
				index++;
				break;
			}
			if (pData[index] == 10)
			{
				index++;
				break;
			}
			index++;
		}
		return true;
	}

	bool CScriptCompiler::LoadSkipAnnotate2State(char* pData, unsigned int size, unsigned int& index)
	{
		if (pData[index++] != '/')
		{
			return false;
		}
		if (pData[index++] != '*')
		{
			return false;
		}
		while (index < size)
		{
			if (pData[index] == '*' && pData[index+1] == '/')
			{
				index += 2;
				break;
			}

			index++;
		}
		return true;
	}


	bool CScriptCompiler::RunCompileState(SentenceSourceCode& vIn, E_CODE_SCOPE scopeType, CBaseICode* pFather, int addType)
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

	void CScriptCompiler::InitCodeCompile()
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
	}



	void CScriptCompiler::PartitionSourceWords(char* pSource, unsigned int size)
	{
		m_vCharIndex2LineIndex.resize(size, 0);
		unsigned int nLineIndex = 1;
		char ch[2] = { 0,0 };
		std::string strCurWord;
		for (unsigned int i = 0; i < size; i++)
		{
			bool bNewLine = false;
			if (pSource[i] == 10)
			{
				bNewLine = true;
			}
			else if (pSource[i] == 13)
			{
				if (i + 1 < size)
				{
					if (pSource[i + 1] == 10)
					{
						i++;
					}
				}
				bNewLine = true;
			}
			if (bNewLine)
			{
				tagSourceLineInfo info;
				info.nLineNum = nLineIndex++;
				info.strLineWords = strCurWord;
				m_vScoureLines.push_back(info);
				strCurWord.clear();
			}
			else
			{
				ch[0] = pSource[i];
				if (strCurWord.size() > 0 || ((unsigned int)ch[0]) > 32)//过滤多余的空格
				{
					strCurWord += ch;
				}
				m_vCharIndex2LineIndex[i] = m_vScoureLines.size();
			}

		}
		if (strCurWord.size() > 0)
		{
			tagSourceLineInfo info;
			info.nLineNum = nLineIndex++;
			info.strLineWords = strCurWord;
			m_vScoureLines.push_back(info);
			strCurWord.clear();
		}
	}

	unsigned int CScriptCompiler::GetSourceWordsIndex(unsigned int nIndex)
	{
		if (m_vCharIndex2LineIndex.size() > nIndex)
		{
			return m_vCharIndex2LineIndex[nIndex];
		}
		return 0;
	}

}