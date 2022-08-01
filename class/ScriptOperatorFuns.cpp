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

	bool addIntFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CIntVar* res = (CIntVar*)result;
		res->Set(pVar1->ToInt() + pVar2->ToInt());
		return true;
	}

	bool addIntStr(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CStringVar* pVar2 = (const CStringVar*)(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CIntVar* res = (CIntVar*)result;
		res->Set(pVar1->ToInt() + pVar2->ToInt());
		return true;
	}

	bool addFloatInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CFloatVar* res = (CFloatVar*)result;
		res->Set(pVar1->ToInt() + pVar2->ToInt());
		return true;
	}

	bool addFloatFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CFloatVar* res = (CFloatVar*)result;
		res->Set(pVar1->ToInt() + pVar2->ToInt());
		return true;
	}

	bool addFloatStr(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CStringVar* pVar2 = (const CStringVar*)(var2);
		if (pVar1 == nullptr || pVar2 == nullptr)
		{
			return false;
		}
		CFloatVar* res = (CFloatVar*)result;
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
	bool addStrFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CStringVar* pVar1 = (const CStringVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);

		CStringVar* res = (CStringVar*)result;
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
	bool sumIntFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		res->Set(pVar1->ToInt() - pVar2->ToInt());
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}
	bool sumFloatInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		CFloatVar* res = (CFloatVar*)result;
		res->Set(pVar1->ToInt() - pVar2->ToInt());
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}
	bool sumFloatFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CFloatVar* res = (CFloatVar*)result;
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
	bool mulIntFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		res->Set(pVar1->ToInt() * pVar2->ToInt());
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}
	bool mulFloatInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		CFloatVar* res = (CFloatVar*)result;
		//CIntVar result;
		res->Set(pVar1->ToInt() * pVar2->ToInt());
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}
	bool mulFloatFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CFloatVar* res = (CFloatVar*)result;
		//CIntVar result;
		res->Set(pVar1->ToInt() * pVar2->ToInt());
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}

	bool divIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
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
		if (pVar2->ToInt() == 0)
		{
			return false;
		}
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		res->Set(pVar1->ToInt() / pVar2->ToInt());
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}
	bool divIntFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		if (abs(pVar2->ToFloat()) <= 0.000001f)
		{
			return false;
		}
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		res->Set(pVar1->ToInt() / pVar2->ToFloat());
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}
	bool divFloatInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		if (pVar2->ToInt() == 0)
		{
			return false;
		}
		CFloatVar* res = (CFloatVar*)result;
		//CIntVar result;
		res->Set(pVar1->ToInt() / pVar2->ToInt());
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}
	bool divFloatFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		if (abs(pVar2->ToFloat()) <= 0.000001f)
		{
			return false;
		}
		CFloatVar* res = (CFloatVar*)result;
		//CIntVar result;
		res->Set(pVar1->ToInt() / pVar2->ToInt());
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}

	bool modIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
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
		if (pVar2->ToInt() == 0)
		{
			return false;
		}
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		res->Set(pVar1->ToInt() % pVar2->ToInt());
		//STACK_PUSH_MOVE(stack, result);
		return true;
	}

	bool equalIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
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
		if (pVar1->ToInt() == pVar2->ToInt())
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

	bool equalIntFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() == pVar2->ToFloat())
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
	bool equalFloatInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() == pVar2->ToFloat())
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
	bool equalFloatFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() == pVar2->ToFloat())
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

	bool equalStrStr(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CStringVar* pVar1 = (const CStringVar*)(var1);
		const CStringVar* pVar2 = (const CStringVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToString() == pVar2->ToString())
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

	bool notEqualIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
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
		if (pVar1->ToInt() != pVar2->ToInt())
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

	bool notEqualIntFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() != pVar2->ToFloat())
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
	bool notEqualFloatInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() != pVar2->ToFloat())
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
	bool notEqualFloatFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() != pVar2->ToFloat())
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

	bool notEqualStrStr(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CStringVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CStringVar* pVar1 = (const CStringVar*)(var1);
		const CStringVar* pVar2 = (const CStringVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToString() != pVar2->ToString())
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

	bool bigIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
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
		if (pVar1->ToInt() > pVar2->ToInt())
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

	bool bigIntFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() > pVar2->ToFloat())
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
	bool bigFloatInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() > pVar2->ToFloat())
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
	bool bigFloatFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() > pVar2->ToFloat())
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

	bool bigEqualIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
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
		if (pVar1->ToInt() >= pVar2->ToInt())
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

	bool bigEqualIntFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() >= pVar2->ToFloat())
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
	bool bigEqualFloatInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() >= pVar2->ToFloat())
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
	bool bigEqualFloatFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() >= pVar2->ToFloat())
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

	bool lessIntFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() < pVar2->ToFloat())
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
	bool lessFloatInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() < pVar2->ToFloat())
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
	bool lessFloatFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() < pVar2->ToFloat())
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
	bool lessEqualIntFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CIntVar* pVar1 = (const CIntVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() <= pVar2->ToFloat())
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
	bool lessEqualFloatInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CIntVar* pVar2 = (const CIntVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() <= pVar2->ToFloat())
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
	bool lessEqualFloatFloat(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
	{
		if (var1 == nullptr || var2 == nullptr || result == nullptr)
		{
			return false;
		}
		if (var1->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			var2->GetType() != CScriptClassInfo<CFloatVar>::GetInstance().nClassType ||
			result->GetType() != CScriptClassInfo<CIntVar>::GetInstance().nClassType)
		{
			return false;
		}
		const CFloatVar* pVar1 = (const CFloatVar*)(var1);
		const CFloatVar* pVar2 = (const CFloatVar*)(var2);
		CIntVar* res = (CIntVar*)result;
		//CIntVar result;
		if (pVar1->ToFloat() <= pVar2->ToFloat())
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

	bool bitAndIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
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
		res->Set(pVar1->ToInt() & pVar2->ToInt());
		return true;
	}
	bool bitOrIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
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
		res->Set(pVar1->ToInt() | pVar2->ToInt());
		return true;
	}
	bool bitXorIntInt(const CBaseVar* var1, const CBaseVar* var2, CBaseVar* result)
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
		res->Set(pVar1->ToInt() ^ pVar2->ToInt());
		return true;
	}
	void InitOperFun()
	{
		int intType = CScriptClassInfo<CIntVar>::GetInstance().nClassType;
		int floatType = CScriptClassInfo<CFloatVar>::GetInstance().nClassType;
		int strType = CScriptClassInfo<CStringVar>::GetInstance().nClassType;
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_ADD, intType, intType, intType, addIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_ADD, intType, floatType, intType, addIntFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_ADD, intType, strType, intType, addIntStr);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_ADD, floatType, intType, floatType, addFloatInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_ADD, floatType, floatType, floatType, addFloatFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_ADD, floatType, strType, floatType,addFloatStr);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_ADD, strType, intType, strType, addStrInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_ADD, strType, floatType, strType, addStrFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_ADD, strType, strType, strType, addStrStr);

		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_SUM, intType, intType, intType,sumIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_SUM, intType, floatType, intType,sumIntFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_SUM, floatType, intType, floatType,sumFloatInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_SUM, floatType, floatType, floatType,sumFloatFloat);

		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_MUL, intType, intType, intType, mulIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_MUL, intType, floatType, intType, mulIntFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_MUL, floatType, intType, floatType, mulFloatInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_MUL, floatType, floatType, floatType,mulFloatFloat);

		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_DIV, intType, intType, intType,divIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_DIV, intType, floatType, intType,divIntFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_DIV, floatType, intType, floatType,divFloatInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_DIV, floatType, floatType, floatType,divFloatFloat);

		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_MOD, intType, intType, intType, modIntInt);

		//ECODE_CMP_EQUAL,//比较
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_EQUAL, intType, intType, intType, equalIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_EQUAL, intType, floatType, intType, equalIntFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_EQUAL, floatType, intType, intType, equalFloatInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_EQUAL, floatType, floatType, intType, equalFloatFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_EQUAL, strType, strType, intType, equalStrStr);
		//	ECODE_CMP_NOTEQUAL,
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_NOTEQUAL, intType, intType, intType, notEqualIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_NOTEQUAL, intType, floatType, intType, notEqualIntFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_NOTEQUAL, floatType, intType, intType, notEqualFloatInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_NOTEQUAL, floatType, floatType, intType, notEqualFloatFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_NOTEQUAL, strType, strType, intType, notEqualStrStr);
		//	ECODE_CMP_BIG,
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_BIG, intType, intType, intType, bigIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_BIG, intType, floatType, intType, bigIntFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_BIG, floatType, intType, intType, bigFloatInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_BIG, floatType, floatType, intType, bigFloatFloat);
		//	ECODE_CMP_BIGANDEQUAL,
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_BIGANDEQUAL, intType, intType, intType, bigEqualIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_BIGANDEQUAL, intType, floatType, intType, bigEqualIntFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_BIGANDEQUAL, floatType, intType, intType, bigEqualFloatInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_BIGANDEQUAL, floatType, floatType, intType, bigEqualFloatFloat);

		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_LESS, intType, intType, intType, lessIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_LESS, intType, floatType, intType, lessIntFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_LESS, floatType, intType, intType, lessFloatInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_LESS, floatType, floatType, intType, lessFloatFloat);

		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_LESSANDEQUAL, intType, intType, intType, lessEqualIntInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_LESSANDEQUAL, intType, floatType, intType, lessEqualIntFloat);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_LESSANDEQUAL, floatType, intType, intType, lessEqualFloatInt);
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_CMP_LESSANDEQUAL, floatType, floatType, intType, lessEqualFloatFloat);

		//ECODE_BIT_AND,
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_BIT_AND, intType, intType, intType, bitAndIntInt);
		//	ECODE_BIT_OR,
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_BIT_OR, intType, intType, intType, bitOrIntInt);
		//	ECODE_BIT_XOR,
		CScriptVarOperatorMgr::GetInstance()->RegisterFun(EMicroCodeType::ECODE_BIT_XOR, intType, intType, intType, bitXorIntInt);
	}
}