
#include "ScriptClassMgr.h"
#include "ScriptSuperPointer.h"

namespace zlscript
{
	CBaseScriptClassMgr::CBaseScriptClassMgr()
	{

	}
	CBaseScriptClassMgr::~CBaseScriptClassMgr()
	{

	}
	void CBaseScriptClassMgr::Release(CScriptPointInterface* pPoint)
	{
		if (pPoint)
		{
			if (pPoint->IsInitScriptPointIndex())
			{
				CScriptSuperPointerMgr::GetInstance()->RemoveClassPoint(pPoint->GetScriptPointIndex());
				pPoint->ClearScriptPointIndex();
			}
		}
	}
	void CBaseScriptClassMgr::Release(CScriptBasePointer* pPoint)
	{
		if (pPoint)
		{
			Release(pPoint->GetPoint());
			//pPoint->SetPointer(nullptr);
		}
	}
}