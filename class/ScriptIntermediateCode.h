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
#include "ScriptVarOperatorMgr.h"
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
	class CScriptExecBlock;
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

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);
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

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);
	};
	class CFunICode : public CBaseICode
	{
	public:
		CFunICode()
		{
			//pBodyCode = nullptr;
			m_nRunStateIndex = 0;
		}
		int GetType()
		{
			return E_I_CODE_FUN;
		}
	public:
		virtual bool DefineTempVar(int type, std::string VarName, CScriptCompiler* pCompiler);

		virtual VarInfo* GetTempVarInfo(const char* pVarName);

		bool LoadDefineFunctionParameter(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);
		virtual bool Compile(SentenceSourceCode& vIn,CScriptCompiler* pCompiler);

		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool MakeExeCode(CExeCodeData& vOut);
	public:
		virtual void SetMaxRunState(int val);
	public:
		std::string funname;
		std::string filename;

		std::string strReturnType;
		int nReturnType;
		int nPramSize{ 0 };
		std::vector<CBaseICode*> vBodyCode;

		int MaxStateIndex{0};
	protected:
		std::map<std::string, VarInfo> m_mapTempVarInfo;

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
		virtual bool DefineTempVar(int type, std::string VarName, CScriptCompiler* pCompiler);
		bool CheckMyTempVar(std::string varName);
		virtual VarInfo* GetTempVarInfo(const char* pVarName);

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool MakeExeCode(CExeCodeData& vOut);
	public:
		//std::map<std::string, std::string> m_mapVarNameAndType;
	protected:
		std::map<std::string, VarInfo> m_mapTempVarInfo;

		std::vector< CBaseICode*> m_vICode;

	public:
		//直接基于中间代码来执行
		//virtual int Run(CScriptExecBlock* pBlock);
	};

	class CLoadVarICode : public CBaseICode
	{
	public:
		CLoadVarICode()
		{
			//cSource = 0;
			//nPos = 0;
		}
		~CLoadVarICode();
		int GetType()
		{
			return E_I_CODE_LOADVAR;
		}


		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);

		virtual int GetResultVarType() { return nResultVarType; }
	public:
		int nResultVarType{ -1 };

		int nLoadType;

		CBaseVar* m_pConst{nullptr};

		int VarIndex{-1};
	public:
		////直接基于中间代码来执行
		//virtual int Run(CScriptExecBlock* pBlock);
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

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool MakeExeCode(CExeCodeData& vOut);
	public:

		int nSaveType;
		CBaseICode* pRightOperand;//右操作数

		int VarIndex{ -1 };

	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
	};

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

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);
		virtual bool MakeExeCode(CExeCodeData& vOut);

		virtual int GetResultVarType() { return nResultVarType; }
	public:
		int nResultVarType{ -1 };

		std::string strClassVarName;//类对象名
		std::string strParamName;//类成员变量名

		bool isGlobal{false};
		int varIndex{ -1 };

		int nParamIndex{-1};
	public:
		////直接基于中间代码来执行
		//virtual int Run(CScriptExecBlock* pBlock);
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
		//virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);
		virtual bool MakeExeCode(CExeCodeData& vOut);
		std::string strClassVarName;//类对象名
		std::string strParamName;//类成员变量名

		bool isGlobal{ false };
		int varIndex{ -1 };

		int nParamIndex{ -1 };

		CBaseICode* pRightOperand;//右操作数

	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
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
		//virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);

		virtual int GetResultVarType() { return nResultVarType; }

	public:
		int nResultVarType{ -1 };
		CBaseICode* pRightOperand;//右操作数

	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
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
		//virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
		virtual CBaseICode* GetICode(int nType, int index);
		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);

		virtual int GetResultVarType() { return nResultVarType; }
	public:
		int nResultVarType{-1};//结果的数据类型，与左操作符相等
		std::string strOperator;//操作符
		int nPriorityLv;//优先级
		//int nOperatorFlag;
		CBaseICode* pLeftOperand;//左操作数
		CBaseICode* pRightOperand;//右操作数
		std::vector<CBaseICode*> m_OtherOperand;

		int nOperatorCode;//操作符，暂时用EMicroCodeType里的ECODE_ADD到ECODE_BIT_XOR
		BinaryOperGroup* pOperGroup{nullptr};
	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
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
		//virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);
	public:
		int nCallBack{ -1 };
		int nFunIndex{ -1 };
		std::string strFunName;
		std::vector<CBaseICode*> vParams;//参数
	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
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
		//virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);

		//int RunClassFun(CScriptExecBlock* pBlock,CScriptPointInterface* pPoint, int funIndex);
	public:
		std::string strClassVarName;
		std::string strFunName;
		std::vector<CBaseICode*> vParams;//参数

		bool isGlobal{ false };
		int varIndex{ -1 };

		int nFunIndex{ -1 };
	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
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

		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);
	protected:
		std::vector<CBaseICode*> vData;
	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
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
		//virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);

		bool CheckOperatorTree(CBaseICode** pNode, CScriptCompiler* pCompiler);
		enum
		{
			E_STATE_OPERATOR,
			E_STATE_OPERAND,
			E_STATE_SIZE,
		};
	public:
		COperatorICode* m_pRoot;

		CBaseICode* pOperandCode;

	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
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
		//virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);

		//bool CheckCondVar(CBaseVar* pVar);
	protected:
		CBaseICode* pCondCode;
		CBaseICode* pTureCode;
		CBaseICode* pFalseCode;
	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
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
		//virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);

		//bool CheckCondVar(CBaseVar* pVar);
	protected:
		CBaseICode* pCondCode;
		CBaseICode* pBodyCode;

	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
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
		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);

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
		//virtual bool MakeExeCode(tagCodeData& vOut);
		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);
	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
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
		//virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);
	protected:
		CBaseICode* pBodyCode;
		//int nVarType;//返回值类型
	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
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
		//virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);
	public:
		std::string strClassType;

		int nClassType{-1};
	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
	};

	//class CDeleteICode : public CBaseICode
	//{
	//public:
	//	CDeleteICode()
	//	{
	//		//cSource = 0;
	//		//nPos = 0;
	//	}
	//	int GetType()
	//	{
	//		return E_I_CODE_DELETE;
	//	}
	//	virtual bool MakeExeCode(tagCodeData& vOut);
	//	virtual void AddICode(int nType, CBaseICode* pCode);

	//	virtual bool Compile(SentenceSourceCode& vIn);
	//public:
	//	std::string m_VarName;

	//	//char cSource;//ESignType
	//	//int nPos;
	//};
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
		//virtual bool MakeExeCode(tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		virtual bool Compile(SentenceSourceCode& vIn, CScriptCompiler* pCompiler);

		virtual bool MakeExeCode(CExeCodeData& vOut);
	public:
		CBaseICode* m_pBody;
	//public:
	//	//直接基于中间代码来执行
	//	virtual int Run(CScriptExecBlock* pBlock);
	};
	//class CTestSignICode : public CBaseICode
	//{
	//public:
	//	CTestSignICode()
	//	{
	//		nNum = 0;
	//	}
	//	int GetType()
	//	{
	//		return E_I_CODE_NONE;
	//	}
	//	virtual bool MakeExeCode(tagCodeData& vOut);
	//	virtual void AddICode(int nType, CBaseICode* pCode);

	//	virtual bool Compile(SentenceSourceCode& vIn);
	//public:
	//	int nNum;
	//};


}