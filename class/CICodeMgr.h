#pragma once
#include "CBaseICodeMgr.h"
#include "ScriptBaseICode.h"
namespace zlscript
{

	template<class T>
	class CICodeMgr : public CBaseICodeMgr
	{
	public:
		CICodeMgr();
		~CICodeMgr();

		CBaseICode* New(CScriptCompiler* pLoader, unsigned int index);

	};

	template<class T>
	inline CICodeMgr<T>::CICodeMgr()
	{
	}

	template<class T>
	inline CICodeMgr<T>::~CICodeMgr()
	{
	}

	template<class T>
	inline CBaseICode* CICodeMgr<T>::New(CScriptCompiler* pCompiler, unsigned int index)
	{
		T* pResult = new T;
		CBaseICode* pCode = dynamic_cast<CBaseICode*>(pResult);
		if (pCode)
		{
			pCode->SetCompiler(pCompiler);
			pCode->m_unBeginSoureIndex = index;
			m_listICode.push_front(pCode);
		}
		else if (pResult)
		{
			delete pResult;
			pResult = nullptr;
			return nullptr;
		}
		return pCode;
	}

}