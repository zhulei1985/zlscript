﻿#include "EScriptVariableType.h"
#include "ScriptSuperPointer.h"
#include "StackVarInfo.h"
namespace zlscript
{
	zlscript::CStringPool StackVarInfo::s_strPool;
	zlscript::CBinaryPool StackVarInfo::s_binPool;
	StackVarInfo::StackVarInfo()
	{
		cType = 0;
		cExtend = 0;
		Int64 = 0;
	}
	StackVarInfo::StackVarInfo(const StackVarInfo& cls)
	{
		cType = cls.cType;
		cExtend = cls.cExtend;

		switch (cls.cType)
		{
		case EScriptVal_Int:
			Int64 = cls.Int64;
			break;
		case EScriptVal_Double:
			Double = cls.Double;
			break;
		case EScriptVal_String:
		{
			Int64 = cls.Int64;
			s_strPool.UseString(Int64);
		}
		break;
		case EScriptVal_ClassPointIndex:
			Int64 = cls.Int64;
			CScriptSuperPointerMgr::GetInstance()->ScriptUsePointer(Int64);
			break;
		case EScriptVal_Binary:
		{
			Int64 = cls.Int64;
			s_binPool.UseBinary(Int64);
		}
		break;
		}

	}
	StackVarInfo::~StackVarInfo()
	{
		Clear();
	}

	void StackVarInfo::Clear()
	{
		if (cType == EScriptVal_String)
		{
			s_strPool.ReleaseString(Int64);
		}
		else if (cType == EScriptVal_Binary)
		{
			s_binPool.ReleaseBinary(Int64);
		}
		else if (cType == EScriptVal_ClassPointIndex)
		{
			CScriptSuperPointerMgr::GetInstance()->ScriptReleasePointer(Int64);
		}
		cType = EScriptVal_None;
		Int64 = 0;
	}

	StackVarInfo& StackVarInfo::operator=(const StackVarInfo& cls)
	{
		Clear();
		cType = cls.cType;
		cExtend = cls.cExtend;

		switch (cls.cType)
		{
		case EScriptVal_Int:
			Int64 = cls.Int64;
			break;
		case EScriptVal_Double:
			Double = cls.Double;
			break;
		case EScriptVal_String:
		{
			Int64 = cls.Int64;
			s_strPool.UseString(Int64);
		}
		break;
		case EScriptVal_ClassPointIndex:
			Int64 = cls.Int64;
			CScriptSuperPointerMgr::GetInstance()->ScriptUsePointer(Int64);
			break;
		case EScriptVal_Binary:
		{
			Int64 = cls.Int64;
			s_binPool.UseBinary(Int64);
		}
		break;
		}

		// TODO: 在此处插入 return 语句
		return *this;
	}

	bool StackVarInfo::operator==(const StackVarInfo& cls) const
	{
		return cType == cls.cType && cExtend == cls.cExtend && Int64 == cls.Int64;
	}
}