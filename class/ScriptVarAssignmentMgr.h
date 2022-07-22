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


#include <functional> 
#include "ScriptVarInfo.h"
//赋值管理
namespace zlscript
{
	typedef std::function<bool(CBaseVar*, const CBaseVar*)> AssignFun;
	class CScriptVarAssignmentMgr
	{
	public:
		static CScriptVarAssignmentMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		CScriptVarAssignmentMgr() {};
		static CScriptVarAssignmentMgr s_Instance;
	public:
		void RegisterFun(int type1, int type2, AssignFun pFun);
		bool Assign(CBaseVar* pDes, const CBaseVar* pSrc);
	protected:
		std::map<__int64, AssignFun> m_vPrintFunFun;
	};

#define RegisterAssign(T1,T2,fun) {\
	T1 *pPoint1 = new T1(); \
	T2 *pPoint2 = new T2(); \
	CScriptVarAssignmentMgr::GetInstance()->RegisterFun(pPoint1->GetType(),pPoint2->GetType2(),fun);\
	delete pPoint1;\
	delete pPoint2;}

#define AssignVar(var1,var2) CScriptVarAssignmentMgr::GetInstance()->Assign(var1,var2)

}