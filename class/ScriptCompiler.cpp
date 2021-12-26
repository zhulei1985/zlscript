#include <string>

#include "ScriptCompiler.h"

namespace zlscript
{
	CScriptCompiler::CScriptCompiler()
	{
		InitLexicalAnalysisFun();
	}
	CScriptCompiler::~CScriptCompiler()
	{
	}

	bool CScriptCompiler::LexicalAnalysis(char* pData, unsigned int size)
	{
		std::string strbuff;
		for (unsigned int i = 0; i < size; )
		{
			auto listIt = m_mapLAFun.find(pData[i]);
			if (listIt != m_mapLAFun.end() && listIt->second.empty())
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
		
		return false;
	}

	void CScriptCompiler::InitLexicalAnalysisFun()
	{
		auto RegisterFun = [&](char ch,auto fun, bool bPushFront = false) {
			auto list = m_mapLAFun[ch];
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
		RegisterFun('[', &CScriptCompiler::LoadSignState);
		RegisterFun(']', &CScriptCompiler::LoadSignState);
		RegisterFun('@', &CScriptCompiler::LoadSignState);
		RegisterFun(';', &CScriptCompiler::LoadSignState);
		RegisterFun(':', &CScriptCompiler::LoadSignState);
		RegisterFun('.', &CScriptCompiler::LoadSignState);
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
		case '@':
		case ';':
		case ':':
		case '.':
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
				return false;
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


#if _SCRIPT_DEBUG
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

#endif

}