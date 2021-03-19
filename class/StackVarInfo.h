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
		~StackVarInfo();

		void Clear();

		//赋值重载
		StackVarInfo& operator=(const StackVarInfo& cls);
		bool operator==(const StackVarInfo& cls) const;

		static zlscript::CStringPool s_strPool;
		static zlscript::CBinaryPool s_binPool;
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
		CScriptBasePointer* pPoint;
	};
}