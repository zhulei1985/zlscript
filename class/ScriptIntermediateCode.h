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
#include <list>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "ScriptCodeStyle.h"
#include "ScriptCompiler.h"
#include "EMicroCodeType.h"
#include "CBaseICodeMgr.h"
#include "ScriptBaseICode.h"
 //编译时的中间代码
namespace zlscript
{


	//class CScriptCodeLoader;
	//struct stVarDefine
	//{
	//	std::string strType;
	//	std::string strName;
	//	std::string strInitValue;
	//};

	enum E_I_CODE_TYPE
	{
		E_I_CODE_NONE = 0,
		E_I_CODE_DEF_GLOBAL_VAL,
		E_I_CODE_DEF_TEMP_VAL,
		E_I_CODE_FUN,
		E_I_CODE_BLOCK,
		E_I_CODE_LOADVAR,
		E_I_CODE_SAVEVAR,
		E_I_CODE_GET_CLASS_PARAM,
		E_I_CODE_SET_CLASS_PARAM,
		E_I_CODE_MINUS,
		E_I_CODE_OPERATOR,
		E_I_CODE_CALL,
		E_I_CODE_CALL_CLASS_FUN,
		E_I_CODE_SENTENCE,
		E_I_CODE_EXPRESSION,
		E_I_CODE_IF,
		E_I_CODE_WHILE,
		E_I_CODE_CONTINUE,
		E_I_CODE_BREAK,
		E_I_CODE_RETURN,
		E_I_CODE_NEW,
		E_I_CODE_DELETE,
		E_I_CODE_BRACKETS,
		//E_I_CODE_OPERAND,
	};
	
	class CBlockICode;
	class CDefGlobalVarICode : public CBaseICode
	{
	public:
		CDefGlobalVarICode()
		{

		}
		int GetType()
		{
			return E_I_CODE_DEF_GLOBAL_VAL;
		}
		virtual bool MakeExeCode(tagCodeData& vOut)
		{
			return true;
		}
		virtual bool Compile(SentenceSourceCode& vIn);
	};
	class CDefTempVarICode : public CBaseICode
	{
	public:
		CDefTempVarICode()
		{

		}
		int GetType()
		{
			return E_I_CODE_DEF_TEMP_VAL;
		}
		virtual bool MakeExeCode(tagCodeData& vOut)
		{
			return true;
		}
		virtual bool Compile(SentenceSourceCode& vIn);
	};
	class CFunICode : public CBaseICode
	{
	public:
		CFunICode()
		{
			//pBodyCode = nullptr;
		}
		int GetType()
		{
			return E_I_CODE_FUN;
		}
	public:
		virtual bool DefineTempVar(std::string VarType, std::string VarName);
		//virtual void SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex);
		virtual unsigned int GetTempVarIndex(const char* pVarName);
		virtual VarInfo* GetTempVarInfo(const char* pVarName);
		virtual bool MakeExeCode(tagCodeData& vOut);

		bool LoadAttribute(SentenceSourceCode& vIn);
		bool LoadDefineFunctionParameter(SentenceSourceCode& vIn);
		virtual bool Compile(SentenceSourceCode& vIn);

		virtual void AddICode(int nType, CBaseICode* pCode);
	public:
		std::string funname;
		std::string filename;
		//char cReturnRegisterIndex;
		//std::vector<stVarDefine> m_vTempVar;
		std::map<std::string, std::string> m_mapVarNameAndType;

		std::string strReturnType;

		//CBlockICode* pBodyCode;
		std::vector<CBaseICode*> vBodyCode;
		//函数属性
		std::map<std::string, StackVarInfo> m_mapFunAttribute;
	protected:
		std::map<std::string, VarInfo> m_mapTempVarIndex;
		std::vector<std::string> m_vecTempVarOrder;

	};

	class CBlockICode : public CBaseICode
	{
	public:
		CBlockICode()
		{

		}
		int GetType()
		{
			return E_I_CODE_BLOCK;
		}
	public:
		virtual bool DefineTempVar(std::string VarType, std::string VarName);
		virtual bool CheckTempVar(const char* pVarName);
		//virtual void SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex);
		virtual unsigned int GetTempVarIndex(const char* pVarName);
		virtual VarInfo* GetTempVarInfo(const char* pVarName);
		virtual bool MakeExeCode(tagCodeData& vOut);

		virtual bool Compile(SentenceSourceCode& vIn);

		virtual void AddICode(int nType, CBaseICode* pCode);
	public:
		std::map<std::string, std::string> m_mapVarNameAndType;
	protected:
		std::map<std::string, VarInfo> m_mapTempVarIndex;

		std::vector< CBaseICode*> m_vICode;
	};

	class CLoadVarICode : public CBaseICode
	{
	public:
		CLoadVarICode()
		{
			//cSource = 0;
			//nPos = 0;
		}
		int GetType()
		{
			return E_I_CODE_LOADVAR;
		}

		char AnalysisVar(tagCodeData& vOut, unsigned int& pos);
		virtual bool MakeExeCode(tagCodeData& vOut);

		virtual bool Compile(SentenceSourceCode& vIn);

		//char cSource;//ESignType
		//int nPos;

		tagSourceWord m_word;
	};
	//此类并不直接参与编译
	class CSaveVarICode : public CBaseICode
	{
	public:
		CSaveVarICode()
		{
			pRightOperand = nullptr;
		}
		int GetType()
		{
			return E_I_CODE_SAVEVAR;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);

		virtual bool Compile(SentenceSourceCode& vIn);

		virtual void AddICode(int nType, CBaseICode* pCode);
		CBaseICode* pRightOperand;//右操作数
		tagSourceWord m_word;
	};
	//class CPush2StackICode : public CBaseICode
	//{
	//public:
	//	CPush2StackICode()
	//	{
	//		cSource = 0;
	//		nPos = 0;
	//	}
	//	virtual bool MakeExeCode(stCodeData& vOut);
	//	char cSource;//ESignType
	//	int nPos;
	//};
	//class CPop4StackICode : public CBaseICode
	//{
	//public:
	//	CPop4StackICode()
	//	{
	//		cRegisterIndex = 0;
	//	}
	//	virtual bool MakeExeCode(stCodeData& vOut);
	//	char cRegisterIndex;
	//	char cDestination;//ESignType
	//	int nPos;
	//};
	class CGetClassParamICode : public CBaseICode
	{
	public:
		CGetClassParamICode()
		{
		}
		int GetType()
		{
			return E_I_CODE_GET_CLASS_PARAM;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual bool Compile(SentenceSourceCode& vIn);
		std::string strClassVarName;//类对象名
		std::string strParamName;//类成员变量名
	};
	//此类并不直接参与编译
	class CSetClassParamICode : public CBaseICode
	{
	public:
		CSetClassParamICode()
		{
			pRightOperand = nullptr;
		}
		int GetType()
		{
			return E_I_CODE_SET_CLASS_PARAM;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
		virtual bool Compile(SentenceSourceCode& vIn);
		std::string strClassVarName;//类对象名
		std::string strParamName;//类成员变量名

		CBaseICode* pRightOperand;//右操作数
	};
	//取反操作
	class CMinusICode : public CBaseICode
	{
	public:
		CMinusICode()
		{
			pRightOperand = nullptr;
		}
		int GetType()
		{
			return E_I_CODE_MINUS;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn);
		CBaseICode* pRightOperand;//右操作数
	};
	class COperatorICode : public CBaseICode
	{
	public:
		COperatorICode()
		{
			nPriorityLv = 0;
			pLeftOperand = nullptr;
			pRightOperand = nullptr;
			//nOperatorFlag = 0;

			nOperatorCode = 0;
		}
		int GetType()
		{
			return E_I_CODE_OPERATOR;
		}
		enum
		{
			E_LEFT_OPERAND,
			E_RIGHT_OPERAND,
			E_OTHER_OPERAND,
		};
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
		virtual CBaseICode* GetICode(int nType, int index);
		virtual bool Compile(SentenceSourceCode& vIn);
	public:
		std::string strOperator;//操作符
		int nPriorityLv;//优先级
		//int nOperatorFlag;
		CBaseICode* pLeftOperand;//左操作数
		CBaseICode* pRightOperand;//右操作数
		std::vector<CBaseICode*> m_OtherOperand;

		int nOperatorCode;//操作符，暂时用EMicroCodeType里的ECODE_ADD到ECODE_BIT_XOR
	};
	//class CCallBackFunICode : public CBaseICode
	//{
	//public:
	//	CCallBackFunICode()
	//	{
	//		nFunIndex = 0;
	//		cRegisterIndex = 0;
	//	}
	//	enum
	//	{
	//		E_PARAM,
	//	};
	//	virtual bool MakeExeCode(stCodeData& vOut);
	//	virtual void AddICode(int nType, CBaseICode* pCode);
	//public:
	//	int nFunIndex;
	//	char cRegisterIndex;//结果存放的寄存器
	//	std::vector<CBaseICode*> vParams;//参数
	//};
	class CCallFunICode : public CBaseICode
	{
	public:
		CCallFunICode()
		{
			//nFunIndex = 0;
		}
		int GetType()
		{
			return E_I_CODE_CALL;
		}
		enum
		{
			E_PARAM,
		};
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
		virtual bool Compile(SentenceSourceCode& vIn);
	public:
		std::string strFunName;
		std::vector<CBaseICode*> vParams;//参数

	};
	class CCallClassFunICode : public CBaseICode
	{
	public:
		CCallClassFunICode()
		{

		}
		int GetType()
		{
			return E_I_CODE_CALL_CLASS_FUN;
		}
		enum
		{
			E_POINT,
			E_PARAM,
		};
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn);
	public:
		std::string strClassVarName;
		std::string strFunName;
		std::vector<CBaseICode*> vParams;//参数
	};

	class CSentenceICode : public CBaseICode
	{
	public:
		CSentenceICode()
		{
		}
		int GetType()
		{
			return E_I_CODE_SENTENCE;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn);

	protected:
		std::vector<CBaseICode*> vData;
	};
	class CExpressionICode : public CBaseICode
	{
	public:
		CExpressionICode()
		{
			m_pRoot = nullptr;
			//m_pCurICode = nullptr;
			pOperandCode = nullptr;
		}
		int GetType()
		{
			return E_I_CODE_EXPRESSION;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn);
		bool CheckOperatorTree(CBaseICode** pNode);
		enum
		{
			E_STATE_OPERATOR,
			E_STATE_OPERAND,
			E_STATE_SIZE,
		};
	protected:
		COperatorICode* m_pRoot;

		CBaseICode* pOperandCode;
	};
	class CIfICode : public CBaseICode
	{
	public:
		CIfICode()
		{
			m_unElseSoureIndex = 0;
			pCondCode = nullptr;
			pTureCode = nullptr;
			pFalseCode = nullptr;
		}
		int GetType()
		{
			return E_I_CODE_IF;
		}
		unsigned int m_unElseSoureIndex;

		enum
		{
			E_COND,
			E_TRUE,
			E_FALSE,
		};
	public:
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn);
	protected:
		CBaseICode* pCondCode;
		CBaseICode* pTureCode;
		CBaseICode* pFalseCode;
	};


	class CWhileICode : public CBaseICode
	{
	public:
		CWhileICode()
		{
			pCondCode = nullptr;
			pBodyCode = nullptr;
		}
		int GetType()
		{
			return E_I_CODE_WHILE;
		}
		enum
		{
			E_COND,
			E_BLOCK,
		};
	public:
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn);
	protected:
		CBaseICode* pCondCode;
		CBaseICode* pBodyCode;
	};
	class CContinueICode : public CBaseICode
	{
	public:
		CContinueICode()
		{

		}
		int GetType()
		{
			return E_I_CODE_CONTINUE;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual bool Compile(SentenceSourceCode& vIn);
	};
	class CBreakICode : public CBaseICode
	{
	public:
		CBreakICode()
		{

		}
		int GetType()
		{
			return E_I_CODE_BREAK;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual bool Compile(SentenceSourceCode& vIn);
	};

	class CForICode : public CBaseICode
	{

	};

	class CReturnICode : public CBaseICode
	{
	public:
		CReturnICode()
		{
			pBodyCode = nullptr;
			//nVarType = 0;
		}
		int GetType()
		{
			return E_I_CODE_RETURN;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn);
	protected:
		CBaseICode* pBodyCode;
		//int nVarType;//返回值类型
	};

	class CNewICode : public CBaseICode
	{
	public:
		CNewICode()
		{
			//nClassType = 0;
		}
		int GetType()
		{
			return E_I_CODE_NEW;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn);
	public:
		std::string strClassType;
	};

	class CDeleteICode : public CBaseICode
	{
	public:
		CDeleteICode()
		{
			//cSource = 0;
			//nPos = 0;
		}
		int GetType()
		{
			return E_I_CODE_DELETE;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn);
	public:
		std::string m_VarName;

		//char cSource;//ESignType
		//int nPos;
	};
	class CBracketsICode : public CBaseICode
	{
	public:
		CBracketsICode()
		{
			m_pBody = nullptr;
			//cSource = 0;
			//nPos = 0;
		}
		int GetType()
		{
			return E_I_CODE_BRACKETS;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn);
	public:
		CBaseICode* m_pBody;
	};
	class CTestSignICode : public CBaseICode
	{
	public:
		CTestSignICode()
		{
			nNum = 0;
		}
		int GetType()
		{
			return E_I_CODE_NONE;
		}
		virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn);
	public:
		int nNum;
	};


}