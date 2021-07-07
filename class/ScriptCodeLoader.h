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
 //#include "scriptcommon.h"
//#include "ScriptIntermediateCode.h"
#include "ScriptStack.h"
#include "ScriptCodeStyle.h"
namespace zlscript
{
	struct VarInfo
	{
		VarInfo()
		{
			cType = 0;
			cGlobal = 0;
			wExtend = 0;
			dwPos = 0;
		}
		//__int64 nVarInfo;

		unsigned char cType; // 1,整数,2 浮点 3,字符 4 类指针
		unsigned char cGlobal;// 1 表示全局变量
		unsigned short wExtend; // 大于1表示是数组下标,不再使用
		unsigned int dwPos;//位置ID

	};
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
	class CBaseICode;
	class CFunICode;
	class CScriptCodeLoader
	{
	public:
		CScriptCodeLoader(void);
		~CScriptCodeLoader(void);
	public:
		static CScriptCodeLoader* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptCodeLoader s_Instance;

	public:
		void RegisterCallBack_LoadFun(std::function<bool(const char*, std::vector<char>&)>);
		bool LoadFile(const char* filename);

	private:
		static bool DefaultLoadFile(const char*, std::vector<char>&);
		typedef std::function< bool(const char*, std::vector<char>&)> LoadFun;
		std::vector<LoadFun> m_vLoadFun;
	private:
		//***************字典*****************//
		//map<string,unsigned short> m_mapDicSentenceToEnum;
		//map<string,unsigned short> m_mapDicSignToEnum;
		std::map<std::string, unsigned short> m_mapDicSingleSignToEnum;//单元素运算符
		std::map<std::string, unsigned short> m_mapDicSignToEnum;
		std::map<std::string, unsigned short> m_mapDicSignToPRI;//计算符的优先级，越高越优先

		std::map<std::string, unsigned short> m_mapDic2KeyWord;

		//*********************中间码的字典**************************//
		std::map<std::string, unsigned short> m_mapDicVarTypeToICode;
		std::map<std::string, unsigned short> m_mapDicSentenceToICode;

		std::map<std::string, unsigned short> m_mapDicFunToICode;

		void initDic();
	public:
		char GetVarType(std::string type, unsigned short& classtype);
		unsigned short GetWordKey(std::string str)
		{
			auto it = m_mapDic2KeyWord.find(str);
			if (it != m_mapDic2KeyWord.end())
			{
				return it->second;
			}
			return 0;
		}
	public:
		//*****************代码*******************//

		struct tagCodeData
		{
			tagCodeData();
			int nType;

			std::vector<StackVarInfo> vNumVar;//临时变量
			std::vector<CodeStyle> vCodeData;
			std::vector<__int64> vInt64Const;//64位整形常量
			std::vector<double> vFloatConst;//浮点常量
			std::vector<std::string> vStrConst;//字符常量

			std::vector<std::string> vCallFunName;//会调用的函数的名称

			std::string filename;
			std::string funname;

			int nDefaultReturnType;//默认返回值类型
		};

		unsigned int GetCodeIndex(const char* pStr);
	private:
		//全局变量字典
		std::map<std::string, VarInfo> m_mapDicGlobalVar;
		//全局变量库
		std::vector<StackVarInfo> vGlobalNumVar;//变量

		//代码库
		std::vector<tagCodeData> m_vecCodeData;

		std::unordered_map<std::string, int> m_mapString2CodeIndex;
	public:
		VarInfo* GetGlobalVarInfo(std::string name);
		void GetGlobalVar(std::vector<StackVarInfo>& vOut);
		int GetCodeSize()
		{
			return (int)m_vecCodeData.size();
		}
		tagCodeData* GetCode(int index)
		{
			if (index >= 0 && index < (int)m_vecCodeData.size())
			{
				return &m_vecCodeData[index];
			}
			return nullptr;
		}
		tagCodeData* GetCode(const char* pName);
		void LoadXml(std::string filename);
		void clear();
		//***************************编译器*******************************
	private:
		void clearAllCompileData();
		//*******************词法分析机*****************
	private:
		enum
		{
			EReturn,
			EContinue,
			ESkipState,
			ESignState,
			EKeyState,
			EStringState,
			ESkipAnnotateState,

			ENeedLoadNewChar = 0x8000,
		};
		int LoadSkipState(std::string& strOut, char ch);
		//读取符号
		int LoadSignState(std::string& strOut, char ch);
		//读取关键字
		int LoadKeyState(std::string& strOut, char ch);
		//读取字符串
		int LoadStringState(std::string& strOut, char ch);
		//略过注释
		int LoadSkipAnnotateState(std::string& strOut, char ch);

		//返回true表示该字符处理完毕
		bool RunLexical(std::string& strOut, char ch, unsigned int nSourceIndex);
	private:

		std::stack<int> m_stackLexical;//词法分析机的状态堆栈
		SentenceSourceCode m_vCurSourceSentence;
		//vector<SentenceSourceCode> m_vLexicalData;//存放词法分析机的生成结果
	private:
		//*******************语法分析状态机********************

		//检查是否是操作符
		bool CheckOperator(std::string word);
		//检查变量名是否合法
		bool CheckVarName(std::string varName);
		//查询临时变量的index
		int QueryTempVar(std::string varName, CBaseICode *pICode);
		/////////////////////////////////////////////

		bool RunCompileState(SentenceSourceCode& vIn);

		int LoadDefineFunctionParameter(SentenceSourceCode& vIn, CBaseICode *pCode);
		int LoadDefineTempVar(SentenceSourceCode& vIn, CBaseICode* pCode);

		int LoadDefineFunState(SentenceSourceCode& vIn);
		//读取{}块的状态机
		int LoadBlockState(SentenceSourceCode& vIn, CBaseICode* pCode, int nType);
		//读取if语句
		int LoadIfSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType);
		//读取for语句
		int LoadForSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType);
		//读取while语句
		int LoadWhileSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType);
		//读取switch语句
		int LoadSwitchSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType);
		//读取switch语句
		int LoadReturnSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType);

		//读取一条语句
		//int LoadOneSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType, std::string endFlag=";");
		int LoadOperatiorState(SentenceSourceCode& vIn, CBaseICode* pCode, int nType);
		////读取函数调用
		//int LoadCallFunState(SentenceSourceCode& vIn, CBaseICode* pCode, int nType);
		////读取类函数调用
		//int LoadCallClassFun(SentenceSourceCode& vIn, CBaseICode* pCode, int nType);
		//////读取算式的状态机
		//int LoadFormulaSentence(SentenceSourceCode& vIn, CBaseICode* pCode, int nType);
		////生成一个向堆栈压数值的代码
		//int LoadAndPushNumVar(SentenceSourceCode& vIn, CBaseICode* pCode, int nType);
		////读取括号内
		//int LoadBracket(SentenceSourceCode& vIn, CBaseICode* pCode, int nType);

		CBaseICode* LoadOperator(SentenceSourceCode& vIn, std::string endFlag = "");
		CBaseICode* LoadCallFun(SentenceSourceCode& vIn);
		CBaseICode* LoadOperand(tagSourceWord &word);

		bool CheckOperatorTree(CBaseICode**pNode);
	private:
		//编译时的中间指令
		enum EICodeType
		{
			EICode_NONE = 0,
			EICode_Define_Int,
			EICode_Define_Double,
			EICode_Define_String,

			EICode_Global_Define,//全局定义
		};
		enum
		{
			ECompile_ERROR,
			ECompile_Return,
			ECompile_Continue,
			ECompile_DefineState,
			ECompile_DefineFunState,
			ECompile_BlockState,
			ECompile_BracketState,
			ECompile_FormulaState,

			ECompile_Next = 0x8000,
		};

		std::stack<int> m_stackCompile;//语法分析机的状态堆栈

		typedef std::vector<CodeStyle> tagCodeSection;

		std::unordered_map<std::string, CFunICode*> m_mapString2Code;

	public:
		//将中间代码树转化成执行代码
		int MakeICode2Code(int nMode);
		//清理中间代码
		void ClearICode();

		void PrintAllCode(const char *pFilename);
		std::string PrintOneCode(CodeStyle code);

	protected:
		std::string GetSignPosTypeName(char Idx);
		std::string GetRegisterName(char regIdx);
	protected:
		//typedef std::map<std::string, int> tagVarName2Pos;
		//std::list<tagVarName2Pos> m_ListTempVarInfo;;

		//std::stack<char> m_stackBlockLayer;

		//tagCodeData m_vTempCodeData;
	public:
		void SaveToBin();
		void LoadFormBin();
		//临时变量库
	//*************************************************************//

	private:
		

		//****************编译以及调试用信息*****************//
		bool bCompileStop;//编译终止
		std::string strCurFileName;
		std::string strCurFunName;

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
		void AddErrorInfo(unsigned int pos, std::string error)
		{
			m_vError.push_back(tagErrorInfo(pos, error));
		}
	public:
		struct tagSourceLineInfo
		{
			tagSourceLineInfo()
			{
				nLineNum = 0;
			}
			std::string strCurFileName;
			unsigned int nLineNum;
			std::string strLineWords;
		};
	private:
#if _SCRIPT_DEBUG
		std::vector<unsigned int> m_vCharIndex2LineIndex;

		std::vector<tagSourceLineInfo> m_vScoureLines;

		void PartitionSourceWords(std::vector<char> &vSource);
		unsigned int GetSourceWordsIndex(unsigned int nIndex);
#endif
	public:
		
		const tagSourceLineInfo& GetSourceWords(unsigned int nIndex);
		//struct tagDebugInfo
		//{
		//	std::string strFileName;
		//	std::string strFunName;
		//	std::string strWords;
		//};
		//std::vector<tagDebugInfo> m_vDebugInfo;

	private:


		friend class CScriptVirtualMachine;
	};
}