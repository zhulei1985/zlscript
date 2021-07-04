#include "ZLScript.h"
#include "zByteArray.h"
#include "ScriptClassFunion.h"
#include "ScriptSuperPointer.h"
#include "EScriptVariableType.h"
#include "ScriptPointInterface.h"
namespace zlscript
{
	CBaseScriptClassFun::~CBaseScriptClassFun()
	{
	}
	void CBaseScriptClassFun::init(std::string name, std::function< int(CScriptCallState*)> pFun, IClassFunObserver* pMaster)
	{
		m_fun = pFun;
		m_name = name;
		if (pMaster)
		{
			pMaster->RegisterScriptFun(this);
		}
	}
	int CBaseScriptClassFun::RunFun(CScriptCallState* pState)
	{
		return m_fun(pState);
	}
}