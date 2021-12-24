#include <string>

#include "ScriptCompiler.h"

namespace zlscript
{
	CScriptCompiler::CScriptCompiler()
	{
		InitLexicalAnalysisFun();
		m_mapLAFun.insert({'_',std::bind(&CScriptCompiler::LoadKeyState,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
	}
	CScriptCompiler::~CScriptCompiler()
	{
	}

	bool CScriptCompiler::LexicalAnalysis(char* pData, unsigned int size)
	{
		std::string strbuff;
		for (unsigned int i = 0; i < size; )
		{
			auto range = m_mapLAFun.equal_range(pData[i]);
			if (range.first == range.second)
			{
				i++;
			}
			else
			{
				for (auto it = range.first; it != range.second; it++)
				{
					LexicalAnalysisFun pFun = it->second;
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
		auto RegisterFun = [&](char ch,auto fun) {
			m_mapLAFun.insert({ ch,std::bind(fun,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3) });

		};
		RegisterFun('ch',&CScriptCompiler::LoadKeyState);
	}

	bool CScriptCompiler::LoadSignState(char* pData, unsigned int size, unsigned int& index)
	{
		std::string strOut;
		char ch = pData[index];
		switch (ch)
		{
		case '!':
			strOut.push_back(ch);
			if (pData[index + 1] == '=')
			{
				index++;
				//strOut.p
			}
			break;
		case '<':
		case '=':
		case '/':
		case '-':
			strOut.push_back(ch);
			break;
		case '(':
			break;
		}
		while (true)
		{
			char ch = pData[index];
			switch (ch)
			{
			case '>':
			case '<':
			case '=':
			case '!':
			case '/':
			case '-':
				strOut.push_back(ch);
				if (strOut.size() == 1)
				{
					index++;
				}
				break;
			}
		}
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