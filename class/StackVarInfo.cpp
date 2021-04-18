#include "EScriptVariableType.h"
#include "ScriptSuperPointer.h"
#include "StackVarInfo.h"

namespace zlscript
{
	zlscript::CStringPool StackVarInfo::s_strPool;
	zlscript::CBinaryPool StackVarInfo::s_binPool;
	StackVarInfo::StackVarInfo()
	{
		cType = EScriptVal_None;
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
		//case EScriptVal_ClassPointIndex:
		//	Int64 = cls.Int64;
		//	CScriptSuperPointerMgr::GetInstance()->ScriptUsePointer(Int64);
		//	break;
		case EScriptVal_ClassPoint:
			pPoint = cls.pPoint;
			CScriptSuperPointerMgr::GetInstance()->PickupPointer(pPoint);
			break;
		case EScriptVal_Binary:
		{
			Int64 = cls.Int64;
			s_binPool.UseBinary(Int64);
		}
		break;
		}

	}
	StackVarInfo::StackVarInfo(__int64 val)
	{
		Clear();
		cType = EScriptVal_Int;
		Int64 = val;
	}
	StackVarInfo::StackVarInfo(double val)
	{
		Clear();
		cType = EScriptVal_Double;
		Double = val;
	}
	StackVarInfo::StackVarInfo(const char *pStr)
	{
		Clear();
		cType = EScriptVal_String;
		Int64 = StackVarInfo::s_strPool.NewString(pStr);
	}
	StackVarInfo::StackVarInfo(CScriptBasePointer* pVal)
	{
		Clear();
		cType = EScriptVal_ClassPoint;
		pPoint = pVal;
		CScriptSuperPointerMgr::GetInstance()->PickupPointer(pPoint);
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
		//else if (cType == EScriptVal_ClassPointIndex)
		//{
		//	CScriptSuperPointerMgr::GetInstance()->ScriptReleasePointer(Int64);
		//}
		else if (cType == EScriptVal_ClassPoint)
		{
			CScriptSuperPointerMgr::GetInstance()->ReturnPointer(pPoint);
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
		//case EScriptVal_ClassPointIndex:
		//	Int64 = cls.Int64;
		//	CScriptSuperPointerMgr::GetInstance()->ScriptUsePointer(Int64);
		//	break;
		case EScriptVal_ClassPoint:
			pPoint = cls.pPoint;
			CScriptSuperPointerMgr::GetInstance()->PickupPointer(pPoint);
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
	PointVarInfo::PointVarInfo()
	{
		pPoint = nullptr;
	}
	PointVarInfo::PointVarInfo(const PointVarInfo& info)
	{
		this->pPoint = info.pPoint;
		CScriptSuperPointerMgr::GetInstance()->PickupPointer(info.pPoint);
	}
	PointVarInfo::PointVarInfo(__int64 nPointIndex)
	{
		pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(nPointIndex);
	}
	PointVarInfo::PointVarInfo(CScriptBasePointer* pPoint)
	{
		this->pPoint = pPoint;
		CScriptSuperPointerMgr::GetInstance()->PickupPointer(pPoint);
	}
	PointVarInfo::~PointVarInfo()
	{
		Clear();
	}
	void PointVarInfo::Clear()
	{
		CScriptSuperPointerMgr::GetInstance()->ReturnPointer(pPoint);
		pPoint = nullptr;
	}
	PointVarInfo& PointVarInfo::operator=(__int64 nPointIndex)
	{
		Clear();
		pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(nPointIndex);
		return *this;
	}
	PointVarInfo& PointVarInfo::operator=(CScriptBasePointer* pPoint)
	{
		Clear();
		this->pPoint = pPoint;
		CScriptSuperPointerMgr::GetInstance()->PickupPointer(pPoint);
		return *this;
	}
	PointVarInfo& PointVarInfo::operator=(const PointVarInfo& info)
	{
		this->pPoint = info.pPoint;
		CScriptSuperPointerMgr::GetInstance()->PickupPointer(info.pPoint);
		return *this;
	}
	bool PointVarInfo::operator==(const PointVarInfo& cls) const
	{
		return this->pPoint == cls.pPoint;
	}
}