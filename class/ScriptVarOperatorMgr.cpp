#include "ScriptVarOperatorMgr.h"
namespace zlscript
{
	CScriptVarOperatorMgr CScriptVarOperatorMgr::s_Instance;
	void CScriptVarOperatorMgr::RegisterFun(int OperType, int type1, int type2, OperFun pFun)
	{
		OperGroup &group = m_mapOperGroup[OperType];
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

		group[trans.index] = pFun;
	}

	bool CScriptVarOperatorMgr::Operator(int OperType, CBaseVar* pDes, CBaseVar* pSrc)
	{
		OperGroup& group = m_mapOperGroup[OperType];
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

		auto it = group.find(trans.index);
		if (it != group.end())
		{
			OperFun& fun = it->second;
			return fun(pDes, pSrc);
		}

		return false;
	}
	OperGroup* CScriptVarOperatorMgr::GetOperGroup(int OperType)
	{
		OperGroup& group = m_mapOperGroup[OperType];
		return &group;
	}
	bool CScriptVarOperatorMgr::Operator(OperGroup* group, CBaseVar* pDes, CBaseVar* pSrc)
	{
		if (group == nullptr)
		{
			return false;
		}
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

		auto it = group->find(trans.index);
		if (it != group->end())
		{
			OperFun& fun = it->second;
			return fun(pDes, pSrc);
		}
		return false;
	}
}