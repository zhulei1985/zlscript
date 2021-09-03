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
#include "ScriptCompileInfo.h"
 //编译时的中间代码
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

	const unsigned int g_nTempVarIndexError = -1;

	class CScriptCodeLoader;
	//struct stVarDefine
	//{
	//	std::string strType;
	//	std::string strName;
	//	std::string strInitValue;
	//};

	enum E_I_CODE_TYPE
	{
		E_I_CODE_NONE,
		E_I_CODE_OPERATOR,
		E_I_CODE_CALL,
		E_I_CODE_OPERAND,
	};
	//编译的中间状态
	class CBaseICode
	{
	public:
		CBaseICode() {
			m_pFather = nullptr;
			m_pLoader = nullptr;
			cRegisterIndex = R_A;
		}
		virtual int GetType()
		{
			return E_I_CODE_NONE;
		}
	public:
		virtual CBaseICode* GetFather();
		virtual void SetFather(CBaseICode* pCode);

		CScriptCodeLoader* GetLoader();
		void SetLoader(CScriptCodeLoader* pLoader);

		virtual bool DefineTempVar(std::string VarType, std::string VarName);
		virtual bool CheckTempVar(const char* pVarName);
		//virtual void SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex) {
		//	return;
		//}
		virtual unsigned int GetTempVarIndex(const char* pVarName);
		virtual VarInfo* GetTempVarInfo(const char* pVarName);

		virtual void SetRegisterIndex(char val) { cRegisterIndex = val; }
		virtual bool MakeExeCode(stCodeData& vOut) = 0;
		virtual bool Compile(SentenceSourceCode& vIn) = 0;

		virtual void AddICode(int nType, CBaseICode* pCode);
		virtual CBaseICode* GetICode(int nType, int index);
	private:
		CBaseICode* m_pFather;
	protected:
		CScriptCodeLoader* m_pLoader;
	public:
		unsigned int m_unBeginSoureIndex;

		unsigned char cRegisterIndex;
	};
	class CBlockICode;
	class CDefGlobalVarICode : public CBaseICode
	{
	public:
		CDefGlobalVarICode()
		{

		}
		virtual bool MakeExeCode(stCodeData& vOut)
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
			pBodyCode = nullptr;
		}
	public:
		virtual bool DefineTempVar(std::string VarType, std::string VarName);
		//virtual void SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex);
		virtual unsigned int GetTempVarIndex(const char* pVarName);
		virtual VarInfo* GetTempVarInfo(const char* pVarName);
		virtual bool MakeExeCode(stCodeData& vOut);

		virtual void AddICode(int nType, CBaseICode* pCode);
	public:
		std::string funname;
		std::string filename;
		//char cReturnRegisterIndex;
		//std::vector<stVarDefine> m_vTempVar;
		std::map<std::string, std::string> m_mapVarNameAndType;

		std::string strReturnType;

		CBlockICode* pBodyCode;
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
	public:
		virtual bool DefineTempVar(std::string VarType, std::string VarName);
		virtual bool CheckTempVar(const char* pVarName);
		//virtual void SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex);
		virtual unsigned int GetTempVarIndex(const char* pVarName);
		virtual VarInfo* GetTempVarInfo(const char* pVarName);
		virtual bool MakeExeCode(stCodeData& vOut);
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
			cSource = 0;
			nPos = 0;
		}
		int GetType()
		{
			return E_I_CODE_OPERAND;
		}

		char AnalysisVar(stCodeData& vOut, unsigned int& pos);
		virtual bool MakeExeCode(stCodeData& vOut);

		char cSource;//ESignType
		int nPos;

		tagSourceWord m_word;
	};
	class CSaveVarICode : public CBaseICode
	{
	public:
		CSaveVarICode()
		{
			pRightOperand == nullptr;
		}

		virtual bool MakeExeCode(stCodeData& vOut);
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
		virtual bool MakeExeCode(stCodeData& vOut);
		std::string strClassVarName;//类对象名
		std::string strParamName;//类成员变量名
	};
	class CSetClassParamICode : public CBaseICode
	{
	public:
		CSetClassParamICode()
		{
			pRightOperand = nullptr;
		}
		virtual bool MakeExeCode(stCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

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
		virtual bool MakeExeCode(stCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
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
			nOperatorFlag = 0;

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
		virtual bool MakeExeCode(stCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
		virtual CBaseICode* GetICode(int nType, int index);
	public:
		std::string strOperator;//操作符
		int nPriorityLv;//优先级
		int nOperatorFlag;
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
		virtual bool MakeExeCode(stCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
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
		enum
		{
			E_POINT,
			E_PARAM,
		};
		virtual bool MakeExeCode(stCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
	public:
		std::string strClassVarName;
		std::string strFunName;
		std::vector<CBaseICode*> vParams;//参数
	};

	//class CSentenceICode : public CBaseICode
	//{
	//public:
	//	CSentenceICode()
	//	{
	//		bClearParm = true;
	//	}
	//	virtual bool MakeExeCode(stCodeData& vOut);
	//	void AddExeCode(CBaseICode* code);

	//	void SetClear(bool val)
	//	{
	//		bClearParm = val;
	//	}
	//protected:
	//	bool bClearParm;
	//	std::vector<CBaseICode*> vData;
	//};
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
		virtual bool MakeExeCode(stCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
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
		enum
		{
			E_COND,
			E_BLOCK,
		};
	public:
		virtual bool MakeExeCode(stCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
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
		virtual bool MakeExeCode(stCodeData& vOut);
	};
	class CBreakICode : public CBaseICode
	{
	public:
		CBreakICode()
		{

		}
		virtual bool MakeExeCode(stCodeData& vOut);
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
		}
		virtual bool MakeExeCode(stCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

	protected:
		CBaseICode* pBodyCode;
		int nVarType;//返回值类型
	};

	class CNewICode : public CBaseICode
	{
	public:
		CNewICode()
		{
			//nClassType = 0;
		}
		virtual bool MakeExeCode(stCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);

	public:
		std::string strClassType;
	};

	class CDeleteICode : public CBaseICode
	{
	public:
		CDeleteICode()
		{
			cSource = 0;
			nPos = 0;
		}
		virtual bool MakeExeCode(stCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
	public:
		std::string m_VarName;

		char cSource;//ESignType
		int nPos;
	};

	class CBaseICodeMgr
	{
	public:
		virtual CBaseICode* New(CScriptCodeLoader* pLoader, unsigned int index) = 0;
		virtual bool Release(CBaseICode* pCode);

	public:
		static void Clear();
	protected:
		static std::list<CBaseICode*> m_listICode;
	};

	template<class T>
	class CICodeMgr : public CBaseICodeMgr
	{
	public:
		CICodeMgr();
		~CICodeMgr();

		CBaseICode* New(CScriptCodeLoader* pLoader, unsigned int index);

	};

	template<class T>
	inline CICodeMgr<T>::CICodeMgr()
	{
	}

	template<class T>
	inline CICodeMgr<T>::~CICodeMgr()
	{
	}

	template<class T>
	inline CBaseICode* CICodeMgr<T>::New(CScriptCodeLoader* pLoader,unsigned int index)
	{
		T* pResult = new T;
		CBaseICode* pCode = dynamic_cast<CBaseICode*>(pResult);
		if (pCode)
		{
			pCode->SetLoader(pLoader);
			pCode->m_unBeginSoureIndex = index;
			m_listICode.push_front(pCode);
		}
		else if (pResult)
		{
			delete pResult;
			pResult = nullptr;
			return nullptr;
		}
		return pCode;
	}

}