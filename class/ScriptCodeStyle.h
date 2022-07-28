#pragma once
///****************************************************************************
//	Copyright (c) 2019 ZhuLei
//	Email:zhulei1985@foxmail.com
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//	http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.
// ****************************************************************************/
//#include <list>
//#include <string>
#include "ScriptVarOperatorMgr.h"
namespace zlscript
{
	class CScriptExecBlock;
	struct CExeParamInfo
	{
		int nType{ 0 };
		int dwPos{ 0 };

		bool isNeedRelease{false};
		const CBaseVar* pVar{nullptr};
	};
	class CBaseExeCode
	{
	public:
		CBaseExeCode()
		{
			m_pNext = nullptr;
			nSoureWordIndex = 0;
			nCodeIndex = 0;
		}

		virtual int Run(CScriptExecBlock *pBlock, CBaseExeCode **pNextPoint) = 0;
		virtual std::string GetCodeString() { return ""; }

		inline bool MakeParamInfo(CScriptExecBlock* pBlock, CExeParamInfo& info);
		inline void ReleaseParamInfo(CScriptExecBlock* pBlock, CExeParamInfo& info);
	public:
		unsigned int registerIndex{ 0xffffffff };
		CBaseExeCode* m_pNext;

		unsigned int nSoureWordIndex;
		unsigned int nCodeIndex;
	};
	//操作符执行(一元),结果压入堆栈
	class CUnaryOperExeCode : public CBaseExeCode
	{
	public:
		CUnaryOperExeCode()
		{
			//operGroup = nullptr;
		}
		virtual int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
		virtual std::string GetCodeString();

		void Clear(CScriptExecBlock* pBlock);
	public:
		//unsigned int registerIndex{ 0xffffffff };

		UnaryOperFun oper;

		CExeParamInfo param;
	};
	//操作符执行(一元),结果压入堆栈
	class CUnaryOperGroupExeCode : public CBaseExeCode
	{
	public:
		CUnaryOperGroupExeCode()
		{
			operGroup = nullptr;
		}
		virtual int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
		virtual std::string GetCodeString();

		void Clear(CScriptExecBlock* pBlock);
	public:
		//unsigned int registerIndex{0xffffffff};
		UnaryOperGroup* operGroup;

		CExeParamInfo param;
	};

	//操作符执行(二元),结果压入堆栈
	class CBinaryOperExeCode : public CBaseExeCode
	{
	public:
		CBinaryOperExeCode()
		{

		}
		virtual int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
		virtual std::string GetCodeString();

		void Clear(CScriptExecBlock* pBlock);
	public:


		BinaryOperFun oper;

		CExeParamInfo leftParam;
		CExeParamInfo rightParam;
	};

	//操作符执行(二元),结果压入堆栈
	class CBinaryOperGroupExeCode : public CBaseExeCode
	{
	public:
		CBinaryOperGroupExeCode()
		{
			operGroup = nullptr;
		}
		virtual int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
		virtual std::string GetCodeString();

		void Clear(CScriptExecBlock* pBlock);
	public:
		//unsigned int registerIndex{ 0xffffffff };

		BinaryOperGroup* operGroup;

		CExeParamInfo leftParam;
		CExeParamInfo rightParam;
	};

//	//	ECODE_PUSH = 20,向堆栈压入数据,经过仔细思考，这个指令应该用不到了
//	class CPushExeCode : public CBaseExeCode
//	{
//	public:
//		CPushExeCode()
//		{
//			//cResultRegister = 0;
//			cType = 0;
//			dwPos = 0;
//		}
//		int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
//		virtual std::string GetCodeString();
//	public:
//		//char cResultRegister;
//		char cType;
//		unsigned int dwPos;
//	};

//	这个指令，用于将指定位置的数据赋值给指定变量
	class CVoluationExeCode : public CBaseExeCode
	{
	public:
		CVoluationExeCode()
		{
			//cResultRegister = 0;
			cType = 0;
			dwPos = 0;
		}
		int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
		virtual std::string GetCodeString();
	public:
		//char cResultRegister;
		char cType;
		unsigned int dwPos;

		CExeParamInfo param;
	};

	//	ECODE_GET_CLASS_PARAM,读取类成员变量到堆栈
	class CGetClassParamExeCode : public CBaseExeCode
	{
	public:
		CGetClassParamExeCode()
		{
			dwPos = 0;
		}
		int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
		virtual std::string GetCodeString();
	public:
		unsigned int dwPos;

		CExeParamInfo param;//类对象所在位置
	};
	//	ECODE_SET_CLASS_PARAM,从堆栈取一值赋值给类成员变量
	class CSetClassParamExeCode : public CBaseExeCode
	{
	public:
		CSetClassParamExeCode()
		{

			dwPos = 0;
		}
		int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
		virtual std::string GetCodeString();

		void Clear(CScriptExecBlock* pBlock);
	public:

		unsigned int dwPos;

		CExeParamInfo param;//类对象所在位置

		CExeParamInfo valInfo;
	};
	//	ECODE_CALL_CALLBACK,
	class CCallBackExeCode : public CBaseExeCode
	{
	public:
		CCallBackExeCode()
		{
			unFunIndex = 0;
		}
		int Run(CScriptExecBlock * pBlock, CBaseExeCode** pNextPoint);
		virtual std::string GetCodeString();

		void Clear(CScriptExecBlock* pBlock);
	public:
		std::vector<CExeParamInfo> params;

		unsigned int unFunIndex;
	};
	//	ECODE_CALL_SCRIPT,
	class CCallScriptExeCode : public CBaseExeCode
	{
	public:
		CCallScriptExeCode()
		{
			unFunIndex = 0;
		}
		int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
		virtual std::string GetCodeString();
		void Clear(CScriptExecBlock* pBlock);
	public:
		std::vector<CExeParamInfo> params;
		unsigned int unFunIndex;

	protected:
		std::vector<const CBaseVar*> paramsCache;
	};
	//	ECODE_JUMP,
	class CJumpExeCode : public CBaseExeCode
	{
	public:
		CJumpExeCode()
		{
			pJumpCode = nullptr;
		}
		int Run(CScriptExecBlock* pBlock, CBaseExeCode **pNextPoint);
		virtual std::string GetCodeString();

		bool CheckCond(const CBaseVar* pVar);
	public:
		CBaseExeCode* pJumpCode;
	};
	//	ECODE_JUMP_TRUE,
	class CJumpTrueExeCode : public CJumpExeCode
	{
	public:
		CJumpTrueExeCode()
		{
			pJumpCode = nullptr;
		}
		int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
		virtual std::string GetCodeString();
	public:
		CExeParamInfo condParam;
	};
	//	ECODE_JUMP_FALSE,
	class CJumpFalseExeCode : public CJumpExeCode
	{
	public:
		CJumpFalseExeCode()
		{
			pJumpCode = nullptr;
		}
		int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
		virtual std::string GetCodeString();
	public:
		CExeParamInfo condParam;

	};
	//	ECODE_RETURN,	//退出函数
	class CReturnExeCode : public CBaseExeCode
	{
	public:
		CReturnExeCode()
		{
			bNeedReturnVar = false;
		}
		int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
		virtual std::string GetCodeString();
	public:
		CExeParamInfo returnParam;
		bool bNeedReturnVar;
	};

	//	ECODE_CALL_CLASS_FUN,
	class CCallClassFunExeCode : public CBaseExeCode
	{
	public:
		CCallClassFunExeCode()
		{
			funIndex = 0;
		}
		int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
		virtual std::string GetCodeString();
		void Clear(CScriptExecBlock* pBlock);
	public:
		CExeParamInfo object;//类对象所在位置
		std::vector<CExeParamInfo> params;

		unsigned int funIndex;
	};
	//	ECODE_NEW_CLASS, //新建一个类实例
	class CNewClassExeCode : public CBaseExeCode
	{
	public:
		CNewClassExeCode()
		{
			//cResultRegister = 0;
			dwClassIndex = 0;
		}
		int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
		virtual std::string GetCodeString();
	public:
		//char cResultRegister;
		unsigned int dwClassIndex;
	};
	////	ECODE_RELEASE_CLASS,//释放一个类实例
	//class CReleaseClassExeCode : public CBaseExeCode
	//{
	//public:
	//	CReleaseClassExeCode()
	//	{
	//		cVarRegister = 0;
	//	}
	//	int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
	//	virtual std::string GetCodeString();
	//public:
	//	char cVarRegister;
	//};
	//	ECODE_BREAK,
	class CBreakExeCode : public CBaseExeCode
	{
	public:
		CBreakExeCode()
		{
			pJumpCode = nullptr;
		}
		int Run(CScriptExecBlock * pBlock, CBaseExeCode * *pNextPoint);
		virtual std::string GetCodeString();
	public:
		CBaseExeCode* pJumpCode;
	};
	//	ECODE_CONTINUE,
	class CContinueExeCode : public CBaseExeCode
	{
	public:
		CContinueExeCode()
		{
			pJumpCode = nullptr;
		}
		int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
		virtual std::string GetCodeString();
	public:
		CBaseExeCode* pJumpCode;
	};

	class CSignExeCode : public CBaseExeCode
	{
	public:
		CSignExeCode()
		{

		}
		int Run(CScriptExecBlock* pBlock, CBaseExeCode** pNextPoint);
		virtual std::string GetCodeString();
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