#include "CBaseICodeMgr.h"
#include "ScriptIntermediateCode.h"
namespace zlscript
{

	std::list<CBaseICode*> CBaseICodeMgr::m_listICode;

	bool CBaseICodeMgr::Release(CBaseICode* pCode)
	{
		if (pCode)
		{
			for (auto it = m_listICode.begin(); it != m_listICode.end(); it++)
			{
				if (*it == pCode)
				{
					m_listICode.erase(it);
					break;
				}
			}
			delete pCode;
			return true;
		}
		return false;
	}

	void CBaseICodeMgr::Clear()
	{
		for (auto it = m_listICode.begin(); it != m_listICode.end(); it++)
		{
			CBaseICode* pCode = *it;
			if (pCode)
			{
				delete pCode;
			}
		}
		m_listICode.clear();
	}
}