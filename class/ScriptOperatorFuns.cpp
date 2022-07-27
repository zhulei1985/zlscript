#include "ScriptVarOperatorMgr.h"
#include "ScriptOperatorFuns.h"
#include "EMicroCodeType.h"
#include "ScriptClassInfo.h"
namespace zlscript
{
	bool addIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CIntVar* res = (CIntVar*)result;
		res->Set(pVar1->ToInt() + pVar2->ToInt());
		return true;
	}

	bool addStrInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CStringVar* pVar1 = (const CStringVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);

		CStringVar *res = (CStringVar*)result;
		res->Set(pVar1->ToString() + pVar2->ToString());
		//STACK_PUSH_COPY(stack, (&result));
		return true;
	}
	bool addStrStr(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CStringVar* pVar1 = (const CStringVar*)(var1);
		const CStringVar* pVar2 = (const CStringVar*)(var2);

		CStringVar* res = (CStringVar*)result;
		res->Set(pVar1->ToString() + pVar2->ToString());
		return true;
	}

	bool sumIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		res->Set(pVar1->ToInt() - pVar2->ToInt());
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}

	bool mulIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		res->Set(pVar1->ToInt() * pVar2->ToInt());
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}

	bool lessIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToInt() < pVar2->ToInt())
		{
			res->Set((__int64)1);
		}
		else
		{
			res->Set((__int64)0);
		}
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}
	bool lessEqualIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToInt() <= pVar2->ToInt())
		{
			res->Set((__int64)1);
		}
		else
		{
			res->Set((__int64)0);
		}
		//STACK_PUSH_MOVE(stack, result);
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