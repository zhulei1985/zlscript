#pragma once
#include "scriptcommon.h"
#include "zStringBuffer.h"
#include "zBinaryBuffer.h"

namespace zlscript
{
	class CScriptBasePointer;
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

		//赋值重载
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