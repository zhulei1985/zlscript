#pragma once
#include <vector>
#include <unordered_map>
#include <functional>

namespace zlscript
{
	//一行源码
	struct tagSourceLineInfo
	{
		tagSourceLineInfo()
		{
			nLineNum = 0;
		}
		unsigned int nLineNum;
		std::string strLineWords;
	};
	enum E_SOURCE_WORD_FLAG
	{
		E_WORD_FLAG_NORMAL,
		E_WORD_FLAG_STRING,
	};
	//词法分析后的
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

	class CScriptCompiler
	{
	public:
		CScriptCompiler();
		~CScriptCompiler();


		bool LexicalAnalysis(char* pData, unsigned int size);

		bool Compile(char* pData, unsigned int size);

	private:
		void InitLexicalAnalysisFun();
		//读取符号
		bool LoadSignState(char* pData, unsigned int size, unsigned int& index);
		//读取关键字
		bool LoadKeyState(char* pData, unsigned int size, unsigned int& index);
		//读取字符串
		bool LoadStringState(char* pData, unsigned int size, unsigned int& index);
		//略过注释 一行注释
		bool LoadSkipAnnotateState(char* pData, unsigned int size, unsigned int& index);
		//略过注释2 /* */一块注释
		bool LoadSkipAnnotate2State(char* pData, unsigned int size, unsigned int& index);

		typedef std::function< bool(char* pData,unsigned int size,unsigned int &index)> LexicalAnalysisFun;
		std::unordered_multimap<char, LexicalAnalysisFun> m_mapLAFun;
	private:
#if _SCRIPT_DEBUG
		std::vector<unsigned int> m_vCharIndex2LineIndex;

		std::vector<tagSourceLineInfo> m_vScoureLines;

		//记录源码，作为调试信息
		void PartitionSourceWords(char* pSource, unsigned int size);
		unsigned int GetSourceWordsIndex(unsigned int nIndex);
#endif
		//词法分析后的源码
		SentenceSourceCode m_vCurSourceSentence;

		std::string strFileName;
	};
}