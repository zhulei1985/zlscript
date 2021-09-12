#pragma once
#include "ZLScript.h"

namespace zlscript
{
	class CTempScriptRunState : public CScriptRunState
	{
	public:
		CTempScriptRunState();
		~CTempScriptRunState();
	public:
		//virtual bool PushEmptyVarToStack();
		//virtual bool PushVarToStack(int nVal);
		//virtual bool PushVarToStack(__int64 nVal);
		//virtual bool PushVarToStack(double Double);
		//virtual bool PushVarToStack(const char* pstr);
		//virtual bool PushClassPointToStack(__int64 nIndex);

		//virtual bool PushVarToStack(StackVarInfo& Val);

		//virtual __int64 PopIntVarFormStack();
		//virtual double PopDoubleVarFormStack();
		//virtual char* PopCharVarFormStack();
		//virtual PointVarInfo PopClassPointFormStack();
		//virtual StackVarInfo PopVarFormStack();

		//virtual int GetParamNum();

		//virtual void CopyToStack(tagScriptVarStack& pStack, int nNum);
		//virtual void CopyFromStack(tagScriptVarStack& pStack);

		////获取函数变量
		//virtual void ClearFunParam();

	protected:
		//堆栈
		//tagScriptVarStack m_varRegister;
	};
}