#pragma once
#include "scriptcommon.h"
#include "zStringBuffer.h"
#include "zBinaryBuffer.h"

namespace zlscript
{
	class CScriptBasePointer;
	class CScriptPointInterface;
	struct PointVarInfo;
	struct StackVarInfo
	{
		unsigned char cType;
		unsigned char cExtend;
		union
		{
			double Double;
			__int64 Int64;
			//const char *pStr;
			CScriptBasePointer* pPoint;
		};

		StackVarInfo();
		StackVarInfo(const StackVarInfo& cls);
		StackVarInfo(__int64 val);
		StackVarInfo(double val);
		StackVarInfo(const char* pStr);
		StackVarInfo(CScriptBasePointer* pPoint);
		~StackVarInfo();

		void Clear();

		//赋值重载 操作符重载消耗过大，开始逐步替代
		StackVarInfo& operator=(__int64 val);
		StackVarInfo& operator=(double val);
		StackVarInfo& operator=(const char *pStr);
		StackVarInfo& operator=(CScriptBasePointer* pPoint);
		StackVarInfo& operator=(CScriptPointInterface* pPoint);
		StackVarInfo& operator=(const PointVarInfo& info);
		StackVarInfo& operator=(const StackVarInfo& cls);
		bool operator==(const StackVarInfo& cls) const;

		static zlscript::CStringPool s_strPool;
		static zlscript::CBinaryPool s_binPool;
	};
	struct hash_SV
	{
		size_t operator() (const zlscript::StackVarInfo& cls) const
		{
			std::size_t h1 = std::hash<char>()(cls.cType);
			std::size_t h2 = std::hash<char>()(cls.cExtend);
			std::size_t h3 = std::hash<__int64>()(cls.Int64);
			return h1 ^ h2 ^ h3;
		}
	};
#define SCRIPTVAR_CLEAR_NO_SAFE(var) {\
		if (var.cType == EScriptVal_String)\
		{\
			StackVarInfo::s_strPool.ReleaseString(var.Int64);\
		}\
		else if (var.cType == EScriptVal_Binary)\
		{\
			StackVarInfo::s_binPool.ReleaseBinary(var.Int64);\
		}\
		else if (var.cType == EScriptVal_ClassPoint)\
		{\
			CScriptSuperPointerMgr::GetInstance()->ReturnPointer(var.pPoint);\
		}\
	}
#define SCRIPTVAR_CLEAR(var) {\
		SCRIPTVAR_CLEAR_NO_SAFE(var)\
		var.cType = EScriptVal_None;\
		var.Int64 = 0;\
	}
#define SCRIPTVAR_SET_INT(var, intvar) { \
		SCRIPTVAR_CLEAR_NO_SAFE(var)\
		var.cType = EScriptVal_Int;\
		var.Int64 = (intvar);\
	}
#define SCRIPTVAR_SET_FLOAT(var, floatvar) { \
		SCRIPTVAR_CLEAR_NO_SAFE(var)\
		var.cType = EScriptVal_Double;\
		var.Double = (floatvar);\
	}
#define SCRIPTVAR_SET_STRING(var, stringvar) { \
		SCRIPTVAR_CLEAR_NO_SAFE(var)\
		var.cType = EScriptVal_String;\
		var.Int64 = StackVarInfo::s_strPool.NewString(stringvar.c_str());\
	}
#define SCRIPTVAR_SET_STRING_POINT(var, pStr) { \
		SCRIPTVAR_CLEAR_NO_SAFE(var)\
		var.cType = EScriptVal_String;\
		var.Int64 = StackVarInfo::s_strPool.NewString(pStr);\
	}
#define SCRIPTVAR_SET_SUPER_POINT(var, point) { \
		SCRIPTVAR_CLEAR_NO_SAFE(var)\
		var.cType = EScriptVal_ClassPoint;\
		var.pPoint = point;\
		CScriptSuperPointerMgr::GetInstance()->PickupPointer(point); \
	}
#define SCRIPTVAR_SET_INTERFACE_POINT(var, point) { \
		SCRIPTVAR_CLEAR_NO_SAFE(var)\
		if (point){\
			var.cType = EScriptVal_ClassPoint;\
			var.pPoint = CScriptSuperPointerMgr::GetInstance()->PickupPointer(point->GetScriptPointIndex());\
		}\
	}
#define SCRIPTVAR_SET_BINARY(var, pBin) { \
		SCRIPTVAR_CLEAR_NO_SAFE(var)\
		var.cType = EScriptVal_Binary;\
		var.Int64 = StackVarInfo::s_binPool.NewBinary(pStr);\
	}
#define SCRIPTVAR_SET_VAR(var, var2) { \
		SCRIPTVAR_CLEAR_NO_SAFE(var)\
		var.cType = var2.cType;\
		var.cExtend = var2.cExtend;\
		switch (var.cType)\
		{\
		case EScriptVal_Int:\
			var.Int64 = var2.Int64;\
			break;\
		case EScriptVal_Double:\
			var.Double = var2.Double;\
			break;\
		case EScriptVal_String:\
		{\
			var.Int64 = var2.Int64;\
			StackVarInfo::s_strPool.UseString(var.Int64);\
		}\
		break;\
		case EScriptVal_ClassPoint:\
			var.pPoint = var2.pPoint;\
			CScriptSuperPointerMgr::GetInstance()->PickupPointer(var.pPoint);\
			break;\
		case EScriptVal_Binary:\
		{\
			var.Int64 = var2.Int64;\
			StackVarInfo::s_binPool.UseBinary(var.Int64);\
		}\
		break;\
		}\
	}

#define SCRIPTVAR_GET_INT(var, intvar) { \
		if (var.cType == EScriptVal_Int)\
		{\
			intvar = var.Int64;\
		}\
		else if (var.cType == EScriptVal_Double)\
		{\
			intvar = (__int64)(var.Double + 0.5f);\
		}\
		else if (var.cType == EScriptVal_String)\
		{\
			const char* pStr = StackVarInfo::s_strPool.GetString(var.Int64);\
			if (pStr)\
			{\
				intvar = _atoi64(pStr);\
			}\
			else\
			{\
				intvar = 0;\
			}\
		}\
	}
#define SCRIPTVAR_GET_FLOAT(var, floatvar) { \
		if (var.cType == EScriptVal_Int)\
		{\
			floatvar = (double)var.Int64;\
		}\
		else if (var.cType == EScriptVal_Double)\
		{\
			floatvar = var.Double;\
		}\
		else if (var.cType == EScriptVal_String)\
		{\
			const char* pStr = StackVarInfo::s_strPool.GetString(var.Int64);\
			if (pStr)\
			{\
				floatvar = atof(pStr);\
			}\
			else\
			{\
				floatvar = 0;\
			}\
		}\
	}
#define SCRIPTVAR_GET_STRING(var, strvar) { \
		if (var.cType == EScriptVal_Int)\
		{\
			char strbuff[64] = {0};\
			sprintf(strbuff, "%lld", var.Int64);\
			strvar=strbuff;\
		}\
		else if (var.cType == EScriptVal_Double)\
		{\
			char strbuff[64] = {0};\
			sprintf(strbuff, "%f", var.Double);\
			strvar=strbuff;\
		}\
		else if (var.cType == EScriptVal_String)\
		{\
			const char* pStr = StackVarInfo::s_strPool.GetString(var.Int64);\
			if (pStr)\
			{\
				strvar = pStr;\
			}\
		}\
	}
	struct PointVarInfo
	{
		PointVarInfo();
		PointVarInfo(const PointVarInfo& info);
		PointVarInfo(__int64 nPointIndex);
		PointVarInfo(CScriptBasePointer *pPoint);
		~PointVarInfo();

		void Clear();

		PointVarInfo& operator=(__int64 nPointIndex);
		PointVarInfo& operator=(CScriptBasePointer* pPoint);
		PointVarInfo& operator=(const PointVarInfo& info);
		bool operator==(const PointVarInfo& cls) const;
		CScriptBasePointer* pPoint;
	};
	struct hash_PV
	{
		size_t operator() (const zlscript::PointVarInfo& cls) const
		{
			std::size_t h1 = std::hash<CScriptBasePointer*>()(cls.pPoint);
			return h1;
		}
	};
}