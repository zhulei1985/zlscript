#pragma once
#include <vector>
#include <unordered_map>
#include <functional>
#include "ScriptCompileInfo.h"
#include "CICodeMgr.h"
namespace zlscript
{
	class CBaseICode;
	class CBaseICodeMgr;
	template<class T>
	class CICodeMgr;
	class CScriptCompiler
	{
	public:
		CScriptCompiler();
		~CScriptCompiler();

		unsigned short GetVarType(tagSourceWord varWord);

		bool LexicalAnalysis(char* pData, unsigned int size);

		bool Compile(char* pData, unsigned int size);

	private:
		void InitLexicalAnalysisFun();
		//读取符号
		bool LoadSignState(char* pData, unsigned int size, unsigned int& index);
		//读取关键字
		bool LoadKeyState(char* pData, unsigned int size, unsigned int& index);
		//读取数值
		bool LoadNumberState(char* pData, unsigned int size, unsigned int& index);
		//读取字符串
		bool LoadStringState(char* pData, unsigned int size, unsigned int& index);
		//略过注释 一行注释
		bool LoadSkipAnnotateState(char* pData, unsigned int size, unsigned int& index);
		//略过注释2 /* */一块注释
		bool LoadSkipAnnotate2State(char* pData, unsigned int size, unsigned int& index);

		typedef std::function< bool(char* pData,unsigned int size,unsigned int &index)> LexicalAnalysisFun;
		typedef std::list<LexicalAnalysisFun> ListLexicalAnalysisFun;
		std::unordered_map<char, ListLexicalAnalysisFun> m_mapLAFun;
	private:

		std::string strFileName;
		//词法分析后的源码
		SentenceSourceCode m_vCurSourceSentence;

	public:
		//生成中间码
		enum E_CODE_SCOPE
		{
			E_CODE_SCOPE_OUTSIDE = 1,//块外范围
			E_CODE_SCOPE_STATEMENT = 2,//语句
			E_CODE_SCOPE_EXPRESSION = 4,//表达式
			E_CODE_SCOPE_MEMBER = 8,//(表达式)成员
			E_CODE_SCOPE_OPERATOR = 16,//操作符
		};

	public:
		void InitCodeCompile();
		template<class T>
		bool AddCodeCompile(int nScopeType);
		bool RunCompileState(SentenceSourceCode& vIn, E_CODE_SCOPE scopeType, CBaseICode* pFather, int addType);

		//bool CheckVarName(string varName);
	private:
		typedef std::list<CBaseICodeMgr*> ListICodeMgr;
		std::unordered_map<int, ListICodeMgr> m_mapICodeMgr;
	public:
		void AddErrorInfo(unsigned int pos, std::string error)
		{
			m_vError.push_back(tagErrorInfo(pos, error));
		}
		void ClearErrorInfo()
		{
			m_vError.clear();
		}
	private:
		struct tagErrorInfo
		{
			tagErrorInfo(unsigned int pos, std::string error)
			{
				nErrorWordPos = pos;
				strError = error;
			}
			unsigned int nErrorWordPos;
			std::string strError;
		};
		std::vector<tagErrorInfo> m_vError;

#if _SCRIPT_DEBUG
		std::vector<unsigned int> m_vCharIndex2LineIndex;

		std::vector<tagSourceLineInfo> m_vScoureLines;

		//记录源码，作为调试信息
		void PartitionSourceWords(char* pSource, unsigned int size);
		unsigned int GetSourceWordsIndex(unsigned int nIndex);
#endif
	};

	template<class T>
	inline bool CScriptCompiler::AddCodeCompile(int nScopeType)
	{
		if (nScopeType & E_CODE_SCOPE_OUTSIDE)
		{
			ListICodeMgr& list = m_mapICodeMgr[E_CODE_SCOPE_OUTSIDE];
			auto pMgr = new CICodeMgr<T>();
			list.push_back(pMgr);
		}
		if (nScopeType & E_CODE_SCOPE_STATEMENT)
		{
			ListICodeMgr& list = m_mapICodeMgr[E_CODE_SCOPE_STATEMENT];
			auto pMgr = new CICodeMgr<T>();
			list.push_back(pMgr);
		}
		if (nScopeType & E_CODE_SCOPE_EXPRESSION)
		{
			ListICodeMgr& list = m_mapICodeMgr[E_CODE_SCOPE_EXPRESSION];
			auto pMgr = new CICodeMgr<T>();
			list.push_back(pMgr);
		}
		if (nScopeType & E_CODE_SCOPE_MEMBER)
		{
			ListICodeMgr& list = m_mapICodeMgr[E_CODE_SCOPE_MEMBER];
			auto pMgr = new CICodeMgr<T>();
			list.push_back(pMgr);
		}
		if (nScopeType & E_CODE_SCOPE_OPERATOR)
		{
			ListICodeMgr& list = m_mapICodeMgr[E_CODE_SCOPE_OPERATOR];
			auto pMgr = new CICodeMgr<T>();
			list.push_back(pMgr);
		}
		return false;
	}
}