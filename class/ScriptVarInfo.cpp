#include "EScriptVariableType.h"
#include "ScriptPointInterface.h"
#include "ScriptVarInfo.h"
#pragma warning (disable: 4996)

namespace zlscript
{
	__int64 CIntVar::ToInt()
	{
		return nValue;
	}

	double CIntVar::ToFloat()
	{
		return (double)nValue;
	}

	std::string CIntVar::ToString()
	{
		char strbuff[64] = { 0 };
		sprintf(strbuff, "%lld", nValue);
		return strbuff;
	}

	tagByteArray CIntVar::ToBinary()
	{
		tagByteArray vec;
		vec.push_back((char)GetType());
		AddInt642Bytes(vec, nValue);
		return std::move(vec);
	}

	bool CIntVar::Set(CBaseVar* pVar)
	{
		CIntVar* pInt = dynamic_cast<CIntVar*>(pVar);
		if (pInt)
		{
			nValue = pInt->nValue;
			return true;
		}
		return false;
	}

	bool CIntVar::Set(__int64 val)
	{
		nValue = val;
		return true;
	}

	bool CIntVar::Set(double val)
	{
		nValue = (__int64)val;
		return true;
	}

	bool CIntVar::Set(std::string val)
	{
		nValue = _atoi64(val.c_str());
		return true;
	}

	bool CIntVar::Set(tagByteArray data)
	{
		int pos = 0;
		char cType = DecodeBytes2Char(&data[0], pos, data.size());
		if (cType != GetType())
			return false;
		nValue = DecodeBytes2Int64(&data[0], pos, data.size());
		return true;
	}

	bool CIntVar::Set(CScriptPointInterface* pVal)
	{
		return false;
	}


	__int64 CFloatVar::ToInt()
	{
		return (__int64)fValue;
	}

	double CFloatVar::ToFloat()
	{
		return fValue;
	}

	std::string CFloatVar::ToString()
	{
		char strbuff[64] = { 0 };
		sprintf(strbuff, "%f", fValue);
		return strbuff;
	}

	tagByteArray CFloatVar::ToBinary()
	{
		tagByteArray vec;
		vec.push_back((char)GetType());
		AddDouble2Bytes(vec, fValue);
		return std::move(vec);
	}

	bool CFloatVar::Set(CBaseVar* pVar)
	{
		CFloatVar* pInt = dynamic_cast<CFloatVar*>(pVar);
		if (pInt)
		{
			fValue = pInt->fValue;
			return true;
		}
		return false;
	}

	bool CFloatVar::Set(__int64 val)
	{
		fValue = (double)val;
		return true;
	}

	bool CFloatVar::Set(double val)
	{
		fValue = val;
		return true;
	}

	bool CFloatVar::Set(std::string val)
	{
		fValue = atof(val.c_str());
		return true;
	}

	bool CFloatVar::Set(tagByteArray data)
	{
		int pos = 0;
		char cType = DecodeBytes2Char(&data[0], pos, data.size());
		if (cType != GetType())
			return false;
		fValue = DecodeBytes2Float(&data[0], pos, data.size());
		return true;
	}

	bool CFloatVar::Set(CScriptPointInterface* pVal)
	{
		return false;
	}

	zlscript::CStringPool CStringVar::s_strPool;
	CStringVar::~CStringVar()
	{
		clear();
	}
	__int64 CStringVar::ToInt()
	{
		std::string& str = s_strPool.GetString(nStringIndex);

		return _atoi64(str.c_str());
	}

	double CStringVar::ToFloat()
	{
		std::string& str = s_strPool.GetString(nStringIndex);

		return atof(str.c_str());
	}

	std::string& CStringVar::ToString()
	{
		std::string& str = s_strPool.GetString(nStringIndex);

		return str;
	}

	tagByteArray CStringVar::ToBinary()
	{
		std::string& str = s_strPool.GetString(nStringIndex);

		tagByteArray vec;
		vec.push_back((char)GetType());
		AddString2Bytes(vec, str.c_str());
		return std::move(vec);

	}

	bool CStringVar::Set(CBaseVar* pVar)
	{
		CStringVar* pStrVar = dynamic_cast<CStringVar*>(pVar);
		if (pStrVar)
		{
			clear();
			nStringIndex = pStrVar->nStringIndex;
			s_strPool.UseString(nStringIndex);
			return true;
		}
		return false;
	}

	bool CStringVar::Set(__int64 val)
	{
		char strbuff[64] = { 0 };
		sprintf(strbuff, "%lld", val);
		clear();
		nStringIndex = s_strPool.NewString(strbuff);
		return true;
	}

	bool CStringVar::Set(double val)
	{
		char strbuff[64] = { 0 };
		sprintf(strbuff, "%f", val);
		clear();
		nStringIndex = s_strPool.NewString(strbuff);
		return true;
	}

	bool CStringVar::Set(std::string val)
	{
		clear();
		nStringIndex = s_strPool.NewString(val.c_str());
		return true;
	}

	bool CStringVar::Set(tagByteArray data)
	{
		int pos = 0;
		char cType = DecodeBytes2Char(&data[0], pos, data.size());
		if (cType != GetType())
			return false;
		std::string str = DecodeBytes2String(&data[0], pos, data.size());
		clear();
		nStringIndex = s_strPool.NewString(str.c_str());
		return true;
	}

	bool CStringVar::Set(CScriptPointInterface* pVal)
	{
		return false;
	}

	void CStringVar::clear()
	{
		if (nStringIndex != 0)
		{
			s_strPool.ReleaseString(nStringIndex);
			nStringIndex = 0;
		}
	}


	zlscript::CBinaryPool CBinaryVar::s_binPool;
	CBinaryVar::~CBinaryVar()
	{
		clear();
	}
	tagByteArray CBinaryVar::ToBinary()
	{
		unsigned int len = 0;
		const char* pData = s_binPool.GetBinary(nBinaryIndex, len);
		if (pData == nullptr || len == 0)
		{
			return tagByteArray();
		}
		tagByteArray vec;
		AddData2Bytes(vec, pData, len);
		return vec;
	}

	bool CBinaryVar::Set(CBaseVar* pVar)
	{

		CBinaryVar* pStrVar = dynamic_cast<CBinaryVar*>(pVar);
		if (pStrVar)
		{
			clear();
			nBinaryIndex = pStrVar->nBinaryIndex;
			s_binPool.UseBinary(nBinaryIndex);
			return true;
		}

		return false;
	}

	bool CBinaryVar::Set(CScriptPointInterface* pVal)
	{
		return false;
	}

	void CBinaryVar::clear()
	{
		if (nBinaryIndex)
		{
			s_binPool.ReleaseBinary(nBinaryIndex);
			nBinaryIndex = 0;
		}
	}


	CPointVar::~CPointVar()
	{
		clear();
	}

	CScriptPointInterface* CPointVar::ToPoint()
	{
		return pPoint;
	}

	bool CPointVar::Set(CBaseVar* pVar)
	{
		CPointVar* pPointVar = dynamic_cast<CPointVar*>(pVar);
		if (pPointVar)
		{
			clear();
			pPoint = pPointVar->pPoint;
			pPoint->PickUp();
			return true;
		}

		return false;
	}

	bool CPointVar::Set(CScriptPointInterface* pVal)
	{
		clear();
		if (pVal)
		{
			pVal->PickUp();
			pPoint = pVal;
		}

		return true;
	}

	void CPointVar::clear()
	{
		if (pPoint)
		{
			pPoint->PickDown();
			pPoint = nullptr;
		}
	}


	//StackVarInfo::StackVarInfo()
	//{
	//	cType = EScriptVal_None;
	//	cExtend = 0;
	//	Int64 = 0;
	//}
	//StackVarInfo::StackVarInfo(const StackVarInfo& cls)
	//{
	//	cType = cls.cType;
	//	cExtend = cls.cExtend;

	//	switch (cls.cType)
	//	{
	//	case EScriptVal_Int:
	//		Int64 = cls.Int64;
	//		break;
	//	case EScriptVal_Double:
	//		Double = cls.Double;
	//		break;
	//	case EScriptVal_String:
	//	{
	//		Int64 = cls.Int64;
	//		s_strPool.UseString(Int64);
	//	}
	//	break;
	//	//case EScriptVal_ClassPointIndex:
	//	//	Int64 = cls.Int64;
	//	//	CScriptSuperPointerMgr::GetInstance()->ScriptUsePointer(Int64);
	//	//	break;
	//	case EScriptVal_ClassPoint:
	//		pPoint = cls.pPoint;
	//		CScriptSuperPointerMgr::GetInstance()->PickupPointer(pPoint);
	//		break;
	//	case EScriptVal_Binary:
	//	{
	//		Int64 = cls.Int64;
	//		s_binPool.UseBinary(Int64);
	//	}
	//	break;
	//	}

	//}
	//StackVarInfo::StackVarInfo(__int64 val)
	//{
	//	Clear();
	//	cType = EScriptVal_Int;
	//	Int64 = val;
	//}
	//StackVarInfo::StackVarInfo(double val)
	//{
	//	Clear();
	//	cType = EScriptVal_Double;
	//	Double = val;
	//}
	//StackVarInfo::StackVarInfo(const char *pStr)
	//{
	//	Clear();
	//	cType = EScriptVal_String;
	//	Int64 = StackVarInfo::s_strPool.NewString(pStr);
	//}
	//StackVarInfo::StackVarInfo(CScriptBasePointer* pVal)
	//{
	//	Clear();
	//	cType = EScriptVal_ClassPoint;
	//	pPoint = pVal;
	//	CScriptSuperPointerMgr::GetInstance()->PickupPointer(pPoint);
	//}
	//StackVarInfo::~StackVarInfo()
	//{
	//	Clear();
	//}

	//void StackVarInfo::Clear()
	//{
	//	if (cType == EScriptVal_String)
	//	{
	//		s_strPool.ReleaseString(Int64);
	//	}
	//	else if (cType == EScriptVal_Binary)
	//	{
	//		s_binPool.ReleaseBinary(Int64);
	//	}
	//	//else if (cType == EScriptVal_ClassPointIndex)
	//	//{
	//	//	CScriptSuperPointerMgr::GetInstance()->ScriptReleasePointer(Int64);
	//	//}
	//	else if (cType == EScriptVal_ClassPoint)
	//	{
	//		CScriptSuperPointerMgr::GetInstance()->ReturnPointer(pPoint);
	//	}
	//	cType = EScriptVal_None;
	//	cExtend = 0;
	//	Int64 = 0;
	//}

	//StackVarInfo& StackVarInfo::operator=(__int64 val)
	//{
	//	Clear();
	//	cType = EScriptVal_Int;
	//	Int64 = val;
	//	return *this;
	//}

	//StackVarInfo& StackVarInfo::operator=(double val)
	//{
	//	Clear();
	//	cType = EScriptVal_Double;
	//	Double = val;
	//	return *this;
	//}

	//StackVarInfo& StackVarInfo::operator=(const char* pStr)
	//{
	//	Clear();
	//	cType = EScriptVal_String;
	//	Int64 = StackVarInfo::s_strPool.NewString(pStr);
	//	return *this;
	//}

	//StackVarInfo& StackVarInfo::operator=(CScriptBasePointer* pPoint)
	//{
	//	Clear();
	//	cType = EScriptVal_ClassPoint;
	//	this->pPoint = pPoint;
	//	CScriptSuperPointerMgr::GetInstance()->PickupPointer(pPoint);
	//	return *this;
	//}

	//StackVarInfo& StackVarInfo::operator=(CScriptPointInterface* pPoint)
	//{
	//	Clear();
	//	if (pPoint)
	//	{
	//		cType = EScriptVal_ClassPoint;
	//		this->pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(pPoint->GetScriptPointIndex());
	//	}

	//	return *this;
	//}

	//StackVarInfo& StackVarInfo::operator=(const PointVarInfo& info)
	//{
	//	Clear();
	//	cType = EScriptVal_ClassPoint;
	//	this->pPoint = info.pPoint;
	//	CScriptSuperPointerMgr::GetInstance()->PickupPointer(info.pPoint);
	//	return *this;
	//}

	//StackVarInfo& StackVarInfo::operator=(const StackVarInfo& cls)
	//{
	//	Clear();
	//	cType = cls.cType;
	//	cExtend = cls.cExtend;

	//	switch (cls.cType)
	//	{
	//	case EScriptVal_Int:
	//		Int64 = cls.Int64;
	//		break;
	//	case EScriptVal_Double:
	//		Double = cls.Double;
	//		break;
	//	case EScriptVal_String:
	//	{
	//		Int64 = cls.Int64;
	//		s_strPool.UseString(Int64);
	//	}
	//	break;
	//	case EScriptVal_ClassPoint:
	//		pPoint = cls.pPoint;
	//		CScriptSuperPointerMgr::GetInstance()->PickupPointer(pPoint);
	//		break;
	//	case EScriptVal_Binary:
	//	{
	//		Int64 = cls.Int64;
	//		s_binPool.UseBinary(Int64);
	//	}
	//	break;
	//	}

	//	return *this;
	//}

	//bool StackVarInfo::operator==(const StackVarInfo& cls) const
	//{
	//	return cType == cls.cType && cExtend == cls.cExtend && Int64 == cls.Int64;
	//}
	//PointVarInfo::PointVarInfo()
	//{
	//	pPoint = nullptr;
	//}
	//PointVarInfo::PointVarInfo(const PointVarInfo& info)
	//{
	//	this->pPoint = info.pPoint;
	//	CScriptSuperPointerMgr::GetInstance()->PickupPointer(info.pPoint);
	//}
	//PointVarInfo::PointVarInfo(__int64 nPointIndex)
	//{
	//	pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(nPointIndex);
	//}
	//PointVarInfo::PointVarInfo(CScriptBasePointer* pPoint)
	//{
	//	this->pPoint = pPoint;
	//	CScriptSuperPointerMgr::GetInstance()->PickupPointer(pPoint);
	//}
	//PointVarInfo::~PointVarInfo()
	//{
	//	Clear();
	//}
	//void PointVarInfo::Clear()
	//{
	//	CScriptSuperPointerMgr::GetInstance()->ReturnPointer(pPoint);
	//	pPoint = nullptr;
	//}
	//PointVarInfo& PointVarInfo::operator=(__int64 nPointIndex)
	//{
	//	Clear();
	//	pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(nPointIndex);
	//	return *this;
	//}
	//PointVarInfo& PointVarInfo::operator=(CScriptBasePointer* pPoint)
	//{
	//	Clear();
	//	this->pPoint = pPoint;
	//	CScriptSuperPointerMgr::GetInstance()->PickupPointer(pPoint);
	//	return *this;
	//}
	//PointVarInfo& PointVarInfo::operator=(const PointVarInfo& info)
	//{
	//	this->pPoint = info.pPoint;
	//	CScriptSuperPointerMgr::GetInstance()->PickupPointer(info.pPoint);
	//	return *this;
	//}
	//bool PointVarInfo::operator==(const PointVarInfo& cls) const
	//{
	//	return this->pPoint == cls.pPoint;
	//}



}