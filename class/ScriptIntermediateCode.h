﻿/****************************************************************************
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
//编译时的中间代码
namespace zlscript
{
	//一句源码
	enum E_SOURCE_WORD_FLAG
	{
		E_WORD_FLAG_NORMAL,
		E_WORD_FLAG_STRING,
	};
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
	const unsigned int g_nTempVarIndexError = -1;

	enum E_ICODE_TYPE
	{

	};
	//编译的中间状态
	class CBaseICode
	{
	public:
		CBaseICode() {
			m_pFather = nullptr;
		}
	public:
		virtual CBaseICode* GetFather();
		virtual void SetFather(CBaseICode* pCode);

		virtual bool CheckTempVar(const char* pVarName);
		virtual void SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex) {
			return;
		}
		virtual unsigned int GetTempVarIndex(const char* pVarName);
		virtual VarInfo GetTempVarInfo(const char* pVarName);
		virtual void MakeExeCode(std::vector<CodeStyle> &vOut)=0;

		virtual void AddICode(int nType, CBaseICode* pCode);
	private:
		CBaseICode* m_pFather;
	public:
		unsigned int m_unBeginSoureIndex;
	};
	class CBlockICode;
	class CFunICode : public CBaseICode
	{
	public:
		CFunICode()
		{
			pBodyCode = nullptr;
		}
	public:
		virtual void SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex);
		virtual unsigned int GetTempVarIndex(const char* pVarName);
		virtual VarInfo GetTempVarInfo(const char* pVarName);
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);

		virtual void AddICode(int nType, CBaseICode* pCode);
	public:
		//char cReturnRegisterIndex;
	protected:
		std::map<std::string, VarInfo> m_mapTempVarIndex;
		std::vector<std::string> m_vecTempVarOrder;

		CBlockICode* pBodyCode;
	};

	class CBlockICode : public CBaseICode
	{
	public:
		CBlockICode()
		{

		}
	public:
		virtual bool CheckTempVar(const char* pVarName);
		virtual void SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex);
		virtual unsigned int GetTempVarIndex(const char* pVarName);
		virtual VarInfo GetTempVarInfo(const char* pVarName);
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
	protected:
		std::map<std::string, VarInfo> m_mapTempVarIndex;

		std::vector< CBaseICode*> m_vICode;
	};

	class CLoadVarICode : public CBaseICode
	{
	public:
		CLoadVarICode()
		{
			cSource = 0;
			nPos = 0;
			cRegisterIndex = 0;
		}

		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);

		char cSource;//ESignType
		int nPos;

		char cRegisterIndex;
	};
	class CSaveVarICode : public CBaseICode
	{
	public:
		CSaveVarICode()
		{
			cDestination = 0;
			nPos = 0;
			cRegisterIndex = 0;
		}

		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);

		char cRegisterIndex;
		char cDestination;//ESignType
		int nPos;
	};
	class CPush2StackICode : public CBaseICode
	{
	public:
		CPush2StackICode()
		{
			cSource = 0;
			nPos = 0;
		}
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		char cSource;//ESignType
		int nPos;
	};
	class CPop4StackICode : public CBaseICode
	{
	public:
		CPop4StackICode()
		{
			cRegisterIndex = 0;
		}
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		char cRegisterIndex;
		char cDestination;//ESignType
		int nPos;
	};
	class CGetClassParamICode : public CBaseICode
	{
	public:
		CGetClassParamICode()
		{
			cDestRIndex = 0;
			cClassRIndex = 0;
			nParamPos = 0;
		}
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		char cDestRIndex;//存放结果的寄存器索引
		char cClassRIndex;//类对象所在寄存器
		int nParamPos;//类成员参数编号
	};
	class CSetClassParamICode : public CBaseICode
	{
	public:
		CSetClassParamICode()
		{
			cVarRIndex = 0;
			cClassRIndex = 0;
			nParamPos = 0;
		}
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		char cVarRIndex;//存放值的寄存器索引
		char cClassRIndex;//类对象所在寄存器
		int nParamPos;//类成员参数编号
	};
	class COperatorICode : public CBaseICode
	{
	public:
		COperatorICode()
		{
			nOperatorCode = 0;
			cRegisterIndex = 0;
		}
		enum
		{
			E_CODE,
		};
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

		int nOperatorCode;//操作符，暂时用EMicroCodeType里的ECODE_ADD到ECODE_BIT_XOR
		char cRegisterIndex;
		std::vector<CBaseICode*> m_vICode;
	};
	class CCallBackFunICode : public CBaseICode
	{
	public:
		CCallBackFunICode()
		{
			nFunIndex = 0;
			cRegisterIndex = 0;
		}
		enum
		{
			E_PARAM,
		};
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
	public:
		int nFunIndex;
		char cRegisterIndex;//结果存放的寄存器
		std::vector<CBaseICode*> vParams;//参数
	};
	class CCallScriptFunICode : public CBaseICode
	{
	public:
		CCallScriptFunICode()
		{
			nFunIndex = 0;
			cRegisterIndex = 0;//结果存放的寄存器
		}
		enum
		{
			E_PARAM,
		};
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
	public:
		int nFunIndex;
		char cRegisterIndex;//结果存放的寄存器
		std::vector<CBaseICode*> vParams;//参数
	};
	class CCallClassFunICode : public CBaseICode
	{
	public:
		CCallClassFunICode()
		{
			nFunIndex = 0;
			cRegisterIndex = 0;//结果存放的寄存器
		}
		enum
		{
			E_POINT,
			E_PARAM,
		};
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
	public:
		int nFunIndex;
		char cRegisterIndex;//结果存放的寄存器
		CBaseICode* m_pClassPointCode;
		std::vector<CBaseICode*> vParams;//参数
	};

	class CSentenceICode : public CBaseICode
	{
	public:
		CSentenceICode()
		{
			bClearParm = true;
		}
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		void AddExeCode(CBaseICode* code);

		void SetClear(bool val)
		{
			bClearParm = val;
		}
	protected:
		bool bClearParm;
		std::vector<CBaseICode*> vData;
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
		unsigned int m_unElseSoureIndex;

		enum
		{
			E_COND,
			E_TRUE,
			E_FALSE,
		};
	public:
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
	protected:
		CSentenceICode* pCondCode;
		CBlockICode* pTureCode;
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
		enum
		{
			E_COND,
			E_BLOCK,
		};
	public:
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
	protected:
		CSentenceICode* pCondCode;
		CBlockICode* pBodyCode;
	};
	class CContinueICode : public CBaseICode
	{
	public:
		CContinueICode()
		{

		}
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
	};
	class CBreakICode : public CBaseICode
	{
	public:
		CBreakICode()
		{

		}
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
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
			nVarType = 0;
			cRegisterIndex = 0;
		}
		virtual void MakeExeCode(std::vector<CodeStyle>& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

	protected:
		CBaseICode* pBodyCode;
		int nVarType;//返回值类型
		char cRegisterIndex;//结果存放的寄存器
	};


	class CICodeMgr
	{
	public:
		CICodeMgr();
		~CICodeMgr();

		template<class T>
		T* New(unsigned int index);

		void Release(CBaseICode* pPoint);

		void Clear();
	private:
		std::set<CBaseICode*> m_setICode;
	public:
		static CICodeMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CICodeMgr s_Instance;
	};

	template<class T>
	inline T* CICodeMgr::New(unsigned int index)
	{
		T* pResult = new T;
		CBaseICode* pCode = dynamic_cast<CBaseICode*>(pResult);
		if (pCode)
		{
			pCode->m_unBeginSoureIndex = index;
			m_setICode.insert(pCode);
		}
		else if (pResult)
		{
			delete pResult;
			pResult = nullptr;
		}
		return pResult;
	}
}