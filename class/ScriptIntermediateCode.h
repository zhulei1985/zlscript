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
#include "ScriptCodeLoader.h"
#include "ScriptCodeStyle.h"
//编译时的中间代码
namespace zlscript
{


	const unsigned int g_nTempVarIndexError = -1;

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
			cRegisterIndex = R_A;
		}
		virtual int GetType()
		{
			return E_I_CODE_NONE;
		}
	public:
		virtual CBaseICode* GetFather();
		virtual void SetFather(CBaseICode* pCode);

		virtual bool DefineTempVar(std::string VarType,std::string VarName);
		virtual bool CheckTempVar(const char* pVarName);
		//virtual void SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex) {
		//	return;
		//}
		virtual unsigned int GetTempVarIndex(const char* pVarName);
		virtual VarInfo *GetTempVarInfo(const char* pVarName);

		virtual void SetRegisterIndex(char val) { cRegisterIndex = val; }
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData &vOut)=0;

		virtual void AddICode(int nType, CBaseICode* pCode);
		virtual CBaseICode* GetICode(int nType, int index);
	private:
		CBaseICode* m_pFather;
	public:
		unsigned int m_unBeginSoureIndex;

		unsigned char cRegisterIndex;
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
		virtual bool DefineTempVar(std::string VarType, std::string VarName);
		//virtual void SetTempVarIndex(const char* pVarName, unsigned int nIndex, int nType, int ClassIndex);
		virtual unsigned int GetTempVarIndex(const char* pVarName);
		virtual VarInfo *GetTempVarInfo(const char* pVarName);
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);

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
		virtual VarInfo *GetTempVarInfo(const char* pVarName);
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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

		char AnalysisVar(CScriptCodeLoader::tagCodeData& vOut, unsigned int& pos);
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);

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

		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
	//	virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
	//	virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
	//	virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
	//	virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
	};
	class CBreakICode : public CBaseICode
	{
	public:
		CBreakICode()
		{

		}
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
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
		virtual bool MakeExeCode(CScriptCodeLoader::tagCodeData& vOut);
		virtual void AddICode(int nType, CBaseICode* pCode);
	public:
		std::string m_VarName;

		char cSource;//ESignType
		int nPos;
	};

	class CICodeMgr
	{
	public:
		CICodeMgr();
		~CICodeMgr();

		template<class T>
		T* New(unsigned int index);

		//void Release(CBaseICode* pPoint);

		void Clear();
	private:
		std::list<CBaseICode*> m_listICode;
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
			m_listICode.push_back(pCode);
		}
		else if (pResult)
		{
			delete pResult;
			pResult = nullptr;
		}
		return pResult;
	}
}