#include "scriptcommon.h"
#include "TempScriptRunState.h"

namespace zlscript
{
	CTempScriptRunState::CTempScriptRunState()
	{
	}
	CTempScriptRunState::~CTempScriptRunState()
	{
	}
	bool CTempScriptRunState::PushEmptyVarToStack()
	{
		StackVarInfo var;
		var.cType = EScriptVal_None;
		var.Int64 = 0;
		m_varRegister.push(var);
		return true;
	}
	bool CTempScriptRunState::PushVarToStack(int nVal)
	{
		StackVarInfo var;
		var.cType = EScriptVal_Int;
		var.Int64 = nVal;
		m_varRegister.push(var);
		return true;
	}
	bool CTempScriptRunState::PushVarToStack(__int64 nVal)
	{
		StackVarInfo var;
		var.cType = EScriptVal_Int;
		var.Int64 = nVal;
		m_varRegister.push(var);
		return true;
	}
	bool CTempScriptRunState::PushVarToStack(double Double)
	{
		StackVarInfo var;
		var.cType = EScriptVal_Double;
		var.Double = Double;
		m_varRegister.push(var);
		return true;
	}
	bool CTempScriptRunState::PushVarToStack(const char* pstr)
	{
		StackVarInfo var;
		var.cType = EScriptVal_String;
		var.Int64 = StackVarInfo::s_strPool.NewString(pstr);
		m_varRegister.push(var);
		return true;
	}
	bool CTempScriptRunState::PushClassPointToStack(__int64 nIndex)
	{
		StackVarInfo var;
		var.cType = EScriptVal_ClassPoint;
		var.pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(nIndex);
		m_varRegister.push(var);
		return true;
	}

	bool CTempScriptRunState::PushVarToStack(StackVarInfo& Val)
	{
		m_varRegister.push(Val);

		return true;
	}

	__int64 CTempScriptRunState::PopIntVarFormStack()
	{
		StackVarInfo var;

		var = m_varRegister.top();
		m_varRegister.pop();
		__int64 nReturn = 0;
		switch (var.cType)
		{
		case EScriptVal_None:
			nReturn = 0;
			break;
		case EScriptVal_Int:
			nReturn = var.Int64;
			break;
		case EScriptVal_Double:
			nReturn = (__int64)(var.Double + 0.5f);
			break;
		case EScriptVal_String:
		{
			const char* pStr = StackVarInfo::s_strPool.GetString(var.Int64);
			if (pStr)
			{
				nReturn = _atoi64(pStr);

			}
			else
			{
				nReturn = 0;
			}
		}

		break;
		}
		return nReturn;
	}
	double CTempScriptRunState::PopDoubleVarFormStack()
	{
		StackVarInfo var;
		var = m_varRegister.top();
		m_varRegister.pop();
		double qReturn = 0;
		switch (var.cType)
		{
		case EScriptVal_None:
			qReturn = 0;
			break;
		case EScriptVal_Int:
			qReturn = (double)var.Int64;
			break;
		case EScriptVal_Double:
			qReturn = var.Double;
			break;
		case EScriptVal_String:
			const char* pStr = StackVarInfo::s_strPool.GetString(var.Int64);
			if (pStr)
			{
				qReturn = atof(pStr);

			}
			else
			{
				qReturn = 0;
			}
			break;
		}

		return qReturn;
	}
	char* CTempScriptRunState::PopCharVarFormStack()
	{
		memset(strbuff, 0, sizeof(strbuff));

		StackVarInfo var;
		var = m_varRegister.top();
		m_varRegister.pop();
		switch (var.cType)
		{
		case EScriptVal_None:
			break;
		case EScriptVal_Int:
			//ltoa(var.Int64,strbuff,10);
			sprintf(strbuff, "%lld", var.Int64);
			break;
		case EScriptVal_Double:
			sprintf(strbuff, "%f", var.Double);
			//gcvt(var.Double,8,strbuff);
			break;
		case EScriptVal_String:
		{
			const char* pStr = StackVarInfo::s_strPool.GetString(var.Int64);
			if (pStr)
				strcpy(strbuff, pStr);
		}


		break;
		}

		return strbuff;
	}
	PointVarInfo CTempScriptRunState::PopClassPointFormStack()
	{
		StackVarInfo var;
		var = m_varRegister.top();
		m_varRegister.pop();

		__int64 nReturn = 0;
		switch (var.cType)
		{
		//case EScriptVal_ClassPointIndex:
		//	return PointVarInfo(var.Int64);
		case EScriptVal_ClassPoint:
			return PointVarInfo(var.pPoint);
		}

		return PointVarInfo();
	}
	StackVarInfo CTempScriptRunState::PopVarFormStack()
	{
		StackVarInfo var;
		var = m_varRegister.top();
		m_varRegister.pop();
		return var;
	}

	int CTempScriptRunState::GetParamNum()
	{
		return m_varRegister.size();
	}

	void CTempScriptRunState::CopyToStack(CScriptStack* pStack, int nNum)
	{
		if (pStack == nullptr)
		{
			return;
		}

		for (unsigned int i = m_varRegister.size() - nNum; i < m_varRegister.size(); i++)
		{
			auto pVar = m_varRegister.GetVal(i);
			if (pVar)
				pStack->push(*pVar);
		}

	}
	void CTempScriptRunState::CopyFromStack(CScriptStack* pStack)
	{
		if (pStack == nullptr)
		{
			return;
		}
		//std::vector<StackVarInfo> vVars;
		//while (!pStack->empty())
		//{
		//	vVars.push_back(pStack->top());
		//	pStack->pop();
		//}

		for (unsigned int i = 0; i < pStack->size(); i++)
		{
			m_varRegister.push(*pStack->GetVal(i));
		}

		while (!pStack->empty())
		{
			pStack->pop();
		}
	}

	//获取函数变量
	void CTempScriptRunState::ClearFunParam()
	{
		while (m_varRegister.size())
		{
			m_varRegister.pop();
		}
	}
}