
#include "ScriptClassMgr.h"
#include "ScriptSuperPointer.h"

namespace zlscript
{
	CBaseScriptVarMgr::CBaseScriptVarMgr()
	{

	}
	CBaseScriptVarMgr::~CBaseScriptVarMgr()
	{

	}
	void CBaseScriptVarMgr::Release(CBaseVar* pPoint)
	{
		if (pPoint)
		{
			delete pPoint;
		}
	}

}