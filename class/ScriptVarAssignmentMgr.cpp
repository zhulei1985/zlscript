#include "ScriptVarAssignmentMgr.h"
namespace zlscript
{
	CScriptVarAssignmentMgr CScriptVarAssignmentMgr::s_Instance;
	void CScriptVarAssignmentMgr::RegisterFun(int type1, int type2, AssignFun pFun)
	{
		union {
			struct
			{
				int type1;
				int type2;
			};
			__int64 index;
		} trans;
		trans.type1 = type1;
		trans.type2 = type2;

		m_vPrintFunFun[trans.index] = pFun;
	}

	bool CScriptVarAssignmentMgr::Assign(CBaseVar* pDes,const CBaseVar* pSrc)
	{
		union {
			struct
			{
				int type1;
				int type2;
			};
			__int64 index;
		} trans;
		trans.type1 = pDes->GetType();
		trans.type2 = pSrc->GetType();

		auto it = m_vPrintFunFun.find(trans.index);
		if (it != m_vPrintFunFun.end())
		{
			AssignFun& fun = it->second;
			return fun(pDes, pSrc);
		}
		//没有注册的赋值处理，尝试使用自带的赋值
		if (trans.type1 == trans.type2)
		{
			return pDes->Set(pSrc);
		}
		return false;
	}
}