#pragma once
#include "scriptcommon.h"
#include "zStringBuffer.h"
#include "zBinaryBuffer.h"

namespace zlscript
{
	struct StackVarInfo
	{
		unsigned char cType;
		unsigned char cExtend;
		union
		{
			double Double;
			__int64 Int64;
			//const char *pStr;
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
}