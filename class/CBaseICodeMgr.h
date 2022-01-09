#pragma once
#include <list>

namespace zlscript
{
	class CBaseICode;
	class CScriptCompiler;

	class CBaseICodeMgr
	{
	public:
		virtual CBaseICode* New(CScriptCompiler* pLoader, unsigned int index) = 0;
		virtual bool Release(CBaseICode* pCode);

	public:
		static void Clear();
	protected:
		static std::list<CBaseICode*> m_listICode;
	};
}