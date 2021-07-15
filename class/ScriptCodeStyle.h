#pragma once
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
#include <list>
#include <string>
#include "EMicroCodeType.h"
namespace zlscript
{

#ifdef  _SCRIPT_DEBUG
	struct CodeStyle
	{
		CodeStyle(unsigned int index)
		{
			nSoureWordIndex = index;
		}
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
		unsigned int nSoureWordIndex;
	};
#else
	struct CodeStyle
	{
		CodeStyle(unsigned int index)
		{
		}
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
	};

#endif
	class CScriptExecBlock;
	class CBaseExeCode
	{
	public:
		CBaseExeCode()
		{
			unInstruct = 0;
			m_pNext = nullptr;
			nSoureWordIndex = 0;
			nCodeIndex = 0;
		}

		//virtual int Run(CScriptExecBlock *pBlock, CBaseExeCode **pNextPoint) = 0;
		virtual std::string GetCodeString() { return ""; }
	public:
		unsigned int unInstruct;
		CBaseExeCode* m_pNext;

		unsigned int nSoureWordIndex;
		unsigned int nCodeIndex;
	};
	//ECODE_ADD = 1, //加
	class CAddExeCode : public CBaseExeCode
	{
	public:
		CAddExeCode()
		{
			unInstruct = ECODE_ADD;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock * pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_SUM = 2, //减
	class CSumExeCode : public CBaseExeCode
	{
	public:
		CSumExeCode()
		{
			unInstruct = ECODE_SUM;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock * pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_MUL = 3, //乘
	class CMulExeCode : public CBaseExeCode
	{
	public:
		CMulExeCode()
		{
			unInstruct = ECODE_MUL;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock * pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_DIV = 4, //除
	class CDivExeCode : public CBaseExeCode
	{
	public:
		CDivExeCode()
		{
			unInstruct = ECODE_DIV;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock * pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_MOD = 5, //求余
	class CModExeCode : public CBaseExeCode
	{
	public:
		CModExeCode()
		{
			unInstruct = ECODE_MOD;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_MINUS, //	取负数
	class CMinusExeCode : public CBaseExeCode
	{
	public:
		CMinusExeCode()
		{
			unInstruct = ECODE_MINUS;
			cResultRegister = 0;
			//cType = 0;
			//dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		//char cType;
		//unsigned int dwPos;
	};
	//	ECODE_CMP_EQUAL,//比较
	class CCmpEqualExeCode : public CBaseExeCode
	{
	public:
		CCmpEqualExeCode()
		{
			unInstruct = ECODE_CMP_EQUAL;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_CMP_NOTEQUAL,
	class CCmpNotEqualExeCode : public CBaseExeCode
	{
	public:
		CCmpNotEqualExeCode()
		{
			unInstruct = ECODE_CMP_NOTEQUAL;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_CMP_BIG,
	class CCmpBigExeCode : public CBaseExeCode
	{
	public:
		CCmpBigExeCode()
		{
			unInstruct = ECODE_CMP_BIG;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock * pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_CMP_BIGANDEQUAL,
	class CCmpBigAndEqualExeCode : public CBaseExeCode
	{
	public:
		CCmpBigAndEqualExeCode()
		{
			unInstruct = ECODE_CMP_BIGANDEQUAL;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_CMP_LESS,
	class CCmpLessExeCode : public CBaseExeCode
	{
	public:
		CCmpLessExeCode()
		{
			unInstruct = ECODE_CMP_LESS;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_CMP_LESSANDEQUAL,
	class CCmpLessAndEqualExeCode : public CBaseExeCode
	{
	public:
		CCmpLessAndEqualExeCode()
		{
			unInstruct = ECODE_CMP_LESSANDEQUAL;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_BIT_AND,
	class CBitAndExeCode : public CBaseExeCode
	{
	public:
		CBitAndExeCode()
		{
			unInstruct = ECODE_BIT_AND;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_BIT_OR,
	class CBitOrExeCode : public CBaseExeCode
	{
	public:
		CBitOrExeCode()
		{
			unInstruct = ECODE_BIT_OR;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock * pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_BIT_XOR,
	class CBitXorExeCode : public CBaseExeCode
	{
	public:
		CBitXorExeCode()
		{
			unInstruct = ECODE_BIT_XOR;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock * pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_PUSH = 20,
	class CPushExeCode : public CBaseExeCode
	{
	public:
		CPushExeCode()
		{
			unInstruct = ECODE_PUSH;
			//cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
	public:
		//char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_POP = 21,
	class CPopExeCode : public CBaseExeCode
	{
	public:
		CPopExeCode()
		{
			unInstruct = ECODE_POP;
			//cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
	public:
		//char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_STATEMENT_END = 22, //语句结束
	//	ECODE_LOAD,
	class CLoadExeCode : public CBaseExeCode
	{
	public:
		CLoadExeCode()
		{
			unInstruct = ECODE_LOAD;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_MOVE,
	class CMoveExeCode : public CBaseExeCode
	{
	public:
		CMoveExeCode()
		{
			unInstruct = ECODE_MOVE;
			cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
	public:
		char cResultRegister;
		char cType;
		unsigned int dwPos;
	};
	//	ECODE_GET_CLASS_PARAM,
	class CGetClassParamExeCode : public CBaseExeCode
	{
	public:
		CGetClassParamExeCode()
		{
			unInstruct = ECODE_GET_CLASS_PARAM;
			cResultRegister = 0;
			cClassRegIndex = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
	public:
		char cResultRegister;
		char cClassRegIndex;
		unsigned int dwPos;
	};
	//	ECODE_SET_CLASS_PARAM,
	class CSetClassParamExeCode : public CBaseExeCode
	{
	public:
		CSetClassParamExeCode()
		{
			unInstruct = ECODE_SET_CLASS_PARAM;
			cVarRegister = 0;
			cClassRegIndex = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
	public:
		char cVarRegister;
		char cClassRegIndex;
		unsigned int dwPos;
	};
	//	ECODE_CALL_CALLBACK,
	class CCallBackExeCode : public CBaseExeCode
	{
	public:
		CCallBackExeCode()
		{
			unInstruct = ECODE_CALL_CALLBACK;
			cResultRegister = 0;

			cParmSize = 0;
			unFunIndex = 0;
		}
		//int Run(CScriptExecBlock * pBlock, CBaseExeCode** pNextPoint);
	public:
		unsigned char cParmSize;
		char cResultRegister;
		unsigned int unFunIndex;
	};
	//	ECODE_CALL_SCRIPT,
	class CCallScriptExeCode : public CBaseExeCode
	{
	public:
		CCallScriptExeCode()
		{
			unInstruct = ECODE_CALL_SCRIPT;
			cResultRegister = 0;

			cParmSize = 0;
			unFunIndex = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
	public:
		unsigned char cParmSize;
		char cResultRegister;
		unsigned int unFunIndex;
	};
	//	ECODE_JUMP,
	class CJumpExeCode : public CBaseExeCode
	{
	public:
		CJumpExeCode()
		{
			unInstruct = ECODE_JUMP;
			pJumpCode = nullptr;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
	public:
		CBaseExeCode* pJumpCode;
	};
	//	ECODE_JUMP_TRUE,
	class CJumpTrueExeCode : public CBaseExeCode
	{
	public:
		CJumpTrueExeCode()
		{
			unInstruct = ECODE_JUMP_TRUE;
			cVarRegister = 0;
			pJumpCode = nullptr;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cVarRegister;
		CBaseExeCode* pJumpCode;
	};
	//	ECODE_JUMP_FALSE,
	class CJumpFalseExeCode : public CBaseExeCode
	{
	public:
		CJumpFalseExeCode()
		{
			unInstruct = ECODE_JUMP_FALSE;
			cVarRegister = 0;
			pJumpCode = nullptr;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cVarRegister;
		CBaseExeCode* pJumpCode;
	};
	//	ECODE_RETURN,	//退出函数
	class CReturnExeCode : public CBaseExeCode
	{
	public:
		CReturnExeCode()
		{
			unInstruct = ECODE_RETURN;
			cVarRegister = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cVarRegister;
	};
	//	ECODE_CLEAR_PARAM,//清空堆栈里的参数
	//	ECODE_CALL_CLASS_FUN,
	class CCallClassFunExeCode : public CBaseExeCode
	{
	public:
		CCallClassFunExeCode()
		{
			unInstruct = ECODE_CALL_CLASS_FUN;
			cResultRegister = 0;
			cParmSize = 0;
			cClassRegIndex = 0;
			dwPos = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		unsigned char cParmSize;
		char cClassRegIndex;
		unsigned int dwPos;
	};
	//	ECODE_NEW_CLASS, //新建一个类实例
	class CNewClassExeCode : public CBaseExeCode
	{
	public:
		CNewClassExeCode()
		{
			unInstruct = ECODE_NEW_CLASS;
			cResultRegister = 0;
			dwClassIndex = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cResultRegister;
		unsigned int dwClassIndex;
	};
	//	ECODE_RELEASE_CLASS,//释放一个类实例
	class CReleaseClassExeCode : public CBaseExeCode
	{
	public:
		CReleaseClassExeCode()
		{
			unInstruct = ECODE_RELEASE_CLASS;
			cVarRegister = 0;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		char cVarRegister;
	};
	//	ECODE_BREAK,
	class CBreakExeCode : public CBaseExeCode
	{
	public:
		CBreakExeCode()
		{
			unInstruct = ECODE_BREAK;
			pJumpCode = nullptr;
		}
		//int Run(CScriptExecBlock * pBlock, CBaseExeCode * *pNextPoint);
	public:
		CBaseExeCode* pJumpCode;
	};
	//	ECODE_CONTINUE,
	class CContinueExeCode : public CBaseExeCode
	{
	public:
		CContinueExeCode()
		{
			unInstruct = ECODE_CONTINUE;
			pJumpCode = nullptr;
		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	public:
		CBaseExeCode* pJumpCode;
	};

	class CSignExeCode : public CBaseExeCode
	{
	public:
		CSignExeCode()
		{

		}
		//int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	};

	class CExeCodeMgr
	{
	public:
		CExeCodeMgr();
		~CExeCodeMgr();

		template<class T>
		T* New(unsigned int index);

		//void Release(CBaseICode* pPoint);

		void Clear();
	private:
		std::list<CBaseExeCode*> m_listExeCode;
	public:
		static CExeCodeMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CExeCodeMgr s_Instance;
	};

	template<class T>
	inline T* CExeCodeMgr::New(unsigned int index)
	{
		T* pResult = new T;
		CBaseExeCode* pCode = dynamic_cast<CBaseExeCode*>(pResult);
		if (pCode)
		{
			pCode->nSoureWordIndex = index;
			m_listExeCode.push_back(pCode);
		}
		else if (pResult)
		{
			delete pResult;
			pResult = nullptr;
		}
		return pResult;
	}
}