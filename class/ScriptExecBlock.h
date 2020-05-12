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
#include <stack>
#include <vector>
#include "ScriptCodeLoader.h"
#include "ScriptStack.h"
//////////////////////////////////////////////////////////////////////////////
//                              脚本执行块                                  //
//////////////////////////////////////////////////////////////////////////////
namespace zlscript
{
	class CScriptRunState;
	class CScriptVirtualMachine;

	class CScriptExecBlock
	{
	public:
		CScriptExecBlock(CScriptCodeLoader::tagCodeData* pData, CScriptRunState* pMaster);
		~CScriptExecBlock(void);

		int GetDefaultReturnType();
		int GetFunType();
	private:

		CScriptRunState* m_pMaster;
		//指向的代码块
		CScriptCodeLoader::tagCodeData* m_pCodeData;
		//执行位置
		unsigned int m_nCodePoint;

	private:
		std::stack<int> m_sCurStackSizeWithoutFunParam;
		std::stack<unsigned int> m_sCycBlockEnd;
		unsigned int m_nCycBlockEnd;

		//堆栈
		CScriptStack m_varRegister;

	public:
		enum ERESULT_TYPE
		{
			ERESULT_END,
			ERESULT_CONTINUE,
			ERESULT_ERROR,
			ERESULT_RETURN,
			ERESULT_WAITING,
			ERESULT_CALLSCRIPTFUN,
			ERESULT_NEXTCONTINUE
		};

		unsigned int ExecBlock(CScriptVirtualMachine* pMachine);

		void SetCallFunParamNum(int nVal)
		{
			CurCallFunParamNum = nVal;
			CurStackSizeWithoutFunParam = m_varRegister.size() - CurCallFunParamNum;
			if (CurStackSizeWithoutFunParam < 0)
			{
				CurStackSizeWithoutFunParam = 0;
			}
		}
		void PushVar(StackVarInfo& var);
		StackVarInfo PopVar();
		StackVarInfo* GetVar(unsigned int index);
		unsigned int GetVarSize();

		int GetParamNum()
		{
			return CurCallFunParamNum;
		}
		void ClearFunParam();
		void ClearStack();

		std::string GetCurSourceWords();
	private:
		std::vector<StackVarInfo> vNumVar;//临时变量

		//当前变量类型
		unsigned char m_cVarType;
		bool bIfSign;
		std::vector<bool> m_vbIfSign;

		int CurCallFunParamNum;//当前调用函数的参数数量
		int CurStackSizeWithoutFunParam;//除了函数参数，堆栈的大小
	public:
		friend class CScriptVirtualMachine;
	};
}
