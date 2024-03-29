#include "ScriptVarOperatorMgr.h"
namespace zlscript
{
	CScriptVarOperatorMgr CScriptVarOperatorMgr::s_Instance;
	void CScriptVarOperatorMgr::RegisterFun(int OperType, int type1, int type2, int resultType, BinaryOperFun pFun)
	{
		BinaryOperGroup &group = m_mapBinaryOperGroup[OperType];
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

		group[trans.index].resultVarType = resultType;
		group[trans.index].fun = pFun;
	}

	bool CScriptVarOperatorMgr::Operator(int OperType, const CBaseVar* pLeft, const CBaseVar* pRight,  CBaseVar* result)
	{
		if (pLeft == nullptr || pRight == nullptr)
		{
			return false;
		}
		BinaryOperGroup& group = m_mapBinaryOperGroup[OperType];
		union {
			struct
			{
				int type1;
				int type2;
			};
			__int64 index;
		} trans;
		trans.type1 = pLeft->GetType();
		trans.type2 = pRight->GetType();

		auto it = group.find(trans.index);
		if (it != group.end())
		{
			BinaryOperFun& fun = it->second.fun;
			return fun(pLeft, pRight, result);
		}

		return false;
	}
	BinaryOperGroup* CScriptVarOperatorMgr::GetBinaryOperGroup(int OperType)
	{
		BinaryOperGroup& group = m_mapBinaryOperGroup[OperType];
		return &group;
	}
	bool CScriptVarOperatorMgr::Operator(BinaryOperGroup* group, const CBaseVar* pLeft, const CBaseVar* pRight, CBaseVar* result)
	{
		if (group == nullptr)
		{
			return false;
		}
		if (pLeft == nullptr || pRight == nullptr)
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
		trans.type1 = pLeft->GetType();
		trans.type2 = pRight->GetType();

		auto it = group->find(trans.index);
		if (it != group->end())
		{
			BinaryOperFun& fun = it->second.fun;
			return fun(pLeft, pRight, result);
		}
		return false;
	}
	void CScriptVarOperatorMgr::RegisterFun(int OperType, int type, int resultType, UnaryOperFun pFun)
	{
		UnaryOperGroup& group = m_mapUnaryOperGroup[OperType];

		group[type].resultVarType = resultType;
		group[type].fun = pFun;
	}
	bool CScriptVarOperatorMgr::Operator(int OperType, const CBaseVar* pVar, CBaseVar* result)
	{
		if (pVar == nullptr)
		{
			return false;
		}
		UnaryOperGroup& group = m_mapUnaryOperGroup[OperType];
		auto it = group.find(pVar->GetType());
		if (it != group.end())
		{
			UnaryOperFun& fun = it->second.fun;
			return fun(pVar, result);
		}
		return false;
	}
	UnaryOperGroup* CScriptVarOperatorMgr::GetUnaryOperGroup(int OperType)
	{
		auto it = m_mapUnaryOperGroup.find(OperType);
		if (it != m_mapUnaryOperGroup.end())
		{
			return &it->second;
		}

		return nullptr;
	}
	bool CScriptVarOperatorMgr::Operator(UnaryOperGroup* group, const CBaseVar* pVar, CBaseVar* result)
	{
		if (group == nullptr || pVar == nullptr)
		{
			return false;
		}
		auto it = group->find(pVar->GetType());
		if (it != group->end())
		{
			UnaryOperFun& fun = it->second.fun;
			return fun(pVar, result);
		}
		return false;
	}
}