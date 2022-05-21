#include "ScriptVarOperatorMgr.h"
#include "ScriptOperatorFuns.h"
#include "EMicroCodeType.h"
#include "ScriptClassInfo.h"
namespace zlscript
{
	bool addIntInt(CBaseVar* var1, CBaseVar* var2,tagScriptVarStack& stack)
	{
		CIntVar* pVar1 = dynamic_cast<CIntVar*>(var1);
		CIntVar* pVar2 = dynamic_cast<CIntVar*>(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CIntVar result;
		result.Set(pVar1->ToInt() + pVar2->ToInt());
		STACK_PUSH_COPY(stack, (&result));
		return true;
	}

	bool addStrInt(CBaseVar* var1, CBaseVar* var2, tagScriptVarStack& stack)
	{
		CStringVar* pVar1 = dynamic_cast<CStringVar*>(var1);
		CIntVar* pVar2 = dynamic_cast<CIntVar*>(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CStringVar result;
		result.Set(pVar1->ToString() + pVar2->ToString());
		STACK_PUSH_COPY(stack, (&result));
		return true;
	}
	bool addStrStr(CBaseVar* var1, CBaseVar* var2, tagScriptVarStack& stack)
	{
		CStringVar* pVar1 = dynamic_cast<CStringVar*>(var1);
		CStringVar* pVar2 = dynamic_cast<CStringVar*>(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CStringVar result;
		result.Set(pVar1->ToString() + pVar2->ToString());
		STACK_PUSH_COPY(stack, (&result));
		return true;
	}

	bool sumIntInt(CBaseVar* var1, CBaseVar* var2, tagScriptVarStack& stack)
	{
		CIntVar* pVar1 = dynamic_cast<CIntVar*>(var1);
		CIntVar* pVar2 = dynamic_cast<CIntVar*>(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CIntVar result;
		result.Set(pVar1->ToInt() - pVar2->ToInt());
		STACK_PUSH_COPY(stack, (&result));
		return true;
	}

	bool mulIntInt(CBaseVar* var1, CBaseVar* var2, tagScriptVarStack& stack)
	{
		CIntVar* pVar1 = dynamic_cast<CIntVar*>(var1);
		CIntVar* pVar2 = dynamic_cast<CIntVar*>(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CIntVar result;
		result.Set(pVar1->ToInt() * pVar2->ToInt());
		STACK_PUSH_COPY(stack, (&result));
		return true;
	}

	bool lessIntInt(CBaseVar* var1, CBaseVar* var2, tagScriptVarStack& stack)
	{
		CIntVar* pVar1 = dynamic_cast<CIntVar*>(var1);
		CIntVar* pVar2 = dynamic_cast<CIntVar*>(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CIntVar result;
		if (pVar1->ToInt() < pVar2->ToInt())
		{
			result.Set((__int64)1);
		}
		else
		{
			result.Set((__int64)0);
		}
		STACK_PUSH_COPY(stack, (&result));
		return true;
	}
	bool lessEqualIntInt(CBaseVar* var1, CBaseVar* var2, tagScriptVarStack& stack)
	{
		CIntVar* pVar1 = dynamic_cast<CIntVar*>(var1);
		CIntVar* pVar2 = dynamic_cast<CIntVar*>(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CIntVar result;
		if (pVar1->ToInt() <= pVar2->ToInt())
		{
			result.Set((__int64)1);
		}
		else
		{
			result.Set((__int64)0);
		}
		STACK_PUSH_COPY(stack, (&result));
		return true;
	}
	void InitOperFun()
	{
		int intType = CScriptClassInfo<CIntVar>::GetInstance().nClassType;
		int strType = CScriptClassInfo<CStringVar>::GetInstance().nClassType;
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_ADD, intType, intType, addIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_ADD, strType, intType, addStrInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_ADD, strType, strType, addStrStr);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_SUM, intType, intType, sumIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_MUL, intType, intType, mulIntInt);

		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_LESS, intType, intType, lessIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_LESSANDEQUAL, intType, intType, lessEqualIntInt);
	}
}