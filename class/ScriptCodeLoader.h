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
 //#include "scriptcommon.h"
namespace zlscript
{
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
#if _SCRIPT_DEBUG
		struct CodeStyle
		{
			union
			{
				__int64 qwCode;
				struct
				{
					unsigned char cSign;//标志
					unsigned char cExtend;//扩展标志
					unsigned short wInstruct;//指令ID
					unsigned int dwPos;//变量地址
				};
			};
			unsigned int nSourseWordIndex;
		};
#else
		union CodeStyle
		{
			__int64 qwCode;
			struct
			{
				unsigned char cSign;//标志
				unsigned char cExtend;//扩展标志
				unsigned short wInstruct;//指令ID
				unsigned int dwPos;//变量地址
			};
		};
#endif
		union VarInfo
		{
			__int64 nVarInfo;
			struct
			{
				unsigned char cType; // 1,整数,2 浮点 3,字符 4 类指针
				unsigned char cGlobal;// 1 表示全局变量
				unsigned short wSize; // 大于1表示是数组下标
				unsigned int dwPos;//位置ID
			};
		};
		//全局变量字典
		std::map<std::string, __int64> m_mapDicGlobalVar;



		////脚本定义的函数索引
		//map<string,WORD> m_mapDicScriptFun;
		////脚本定义的函数总数
		//WORD wScriptFunAmount;

		////临时变量字典
		//map<string,VarInfo> m_mapDicTempVar;
		////临时变量总数
		//map<unsigned char,unsigned int> wTempVarAmonut;
	public:
		//*****************代码*******************//

		//typedef	vector<CodeStyle> tagCodeData;

		struct VarPoint
		{
			unsigned char cType;
			unsigned short unArraySize;
			union
			{
				double* pDouble;
				__int64* pInt64;
				char* pStr;
				unsigned int nClassPointIndex;
			};
		};

		struct tagCodeData
		{
			tagCodeData();
			int nType;
			std::vector<std::string> vStrConst;//字符常量
			std::vector<VarPoint> vNumVar;//临时变量
			std::vector<CodeStyle> vCodeData;
			std::vector<double> vFloatConst;//浮点常量

			std::string filename;
			std::string funname;
		};

	private:
		//全局变量库
		std::vector<VarPoint> vGlobalNumVar;//变量

		//代码库
		std::vector<tagCodeData> m_vecCodeData;


		std::map<std::string, int> m_mapString2CodeIndex;
	public:
		void GetGlobalVar(std::vector<VarPoint>& vOut);
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
		typedef std::vector<tagSourceWord> SentenceSourceCode;
		std::stack<int> m_stackLexical;//词法分析机的状态堆栈
		SentenceSourceCode m_vCurSourceSentence;
		//vector<SentenceSourceCode> m_vLexicalData;//存放词法分析机的生成结果
	private:
		//*******************语法分析状态机********************


		//检查变量名是否合法
		bool CheckVarName(std::string varName);
		////定义临时变量
		int DefineTempVar(SentenceSourceCode& vIn, unsigned int& pos/*,vector<CodeStyle>  &vOut*/);
		//查询临时变量的index
		int QueryTempVar(std::string varName);
		//生成一个向堆栈压数值的代码
		int LoadAndPushVar(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);
		int LoadAndPushNumVar(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);
		int LoadAndPushClassPoint(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);
		int LoadCallClassFunn(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);
		int LoadAndPushStrVar(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);

		//给全局变量设置默认值
		int SetVarDefaultValue(SentenceSourceCode& vIn, unsigned int& pos, __int64 info);
		//读取函数调用
		int LoadCallFunState(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);
		//读取if语句
		int LoadIfSentence(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);
		//读取for语句
		int LoadForSentence(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);
		//读取while语句
		int LoadWhileSentence(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);
		//读取switch语句
		int LoadSwitchSentence(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);

		////读取算式的状态机
		int LoadFormulaSentence(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);
		///////////////分解算式函数//////////////////
		//TODO 优先级缺陷，修正待测试
		int LoadFormulaRecursion(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut, int nVarType, int nLevel = 1);//读取数值运算

		int LoadReturnFormulaRecursion(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);//读取数值运算

		//int LoadAddAndSum(vector<string> &vIn,unsigned int &pos,vector<CodeStyle> &vOut);//读取加减
		//int LoadMulAndDivAndMod(vector<string> &vIn,unsigned int &pos,vector<CodeStyle> &vOut);//读取乘除求余
		int LoadBracket(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut);//读取括号内
		/////////////////////////////////////////////

		////定义函数的状态
		int LoadDefineFunState(SentenceSourceCode& vIn, unsigned int& pos);
		////读取{}块的状态机
		int LoadBlockState(SentenceSourceCode& vIn, unsigned int& pos, std::vector<CodeStyle>& vOut, std::vector<CodeStyle> vBackCode);

		bool RunCompileState(SentenceSourceCode& vIn, unsigned int& pos);

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
		//std::vector<std::string> vDebugStringLog;
		std::string strError;

		std::stack<int> m_stackCompile;//语法分析机的状态堆栈

		typedef std::vector<CodeStyle> tagCodeSection;

		int m_nCurFunVarType;
		tagCodeData m_vTempCodeData;
		//vector<CodeStyle> m_vICodeData;


		//临时变量管理
	protected:
		void ClearStackTempVarInfo();
		void NewTempVarLayer();
		void RemoveTempVarLayer();
		bool CheckDefTempVar(const char* pStr);
		unsigned int GetTempVarIndex(const char* pStr);
		bool NewTempVarIndex(const char* pStr, unsigned int nIndex);
	protected:
		typedef std::map<std::string, int> tagVarName2Pos;
		std::list<tagVarName2Pos> m_ListTempVarInfo;;
		const unsigned int m_nTempVarIndexError = -1;

		std::stack<char> m_stackBlockLayer;
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
		unsigned int nErrorWordPos;
#if _SCRIPT_DEBUG

		struct tagSourceWordInfo
		{
			unsigned int nEndIndex;
			unsigned int nSourceLineIndex;
		};
		std::vector<tagSourceWordInfo> m_vSourceWords;
		struct tagSourceLineInfo
		{
			std::string strCurFileName;
			unsigned int nLineNum;
			std::string strLineWords;
		};
		std::vector<tagSourceLineInfo> m_vScoureLines;

		void PartitionSourceWords(std::vector<char> &vSource);
		unsigned int GetSourceWordsIndex(unsigned int nIndex);
		unsigned int GetSourceLineIndex(SentenceSourceCode& vIn, unsigned int pos);
#endif
	public:
		std::string GetSourceWords(unsigned int nIndex);
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