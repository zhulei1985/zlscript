/****************************************************************************
	Copyright (c) 2019 ZhuLei
	Email:zhulei1985@foxmail.com

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
 ****************************************************************************/

#pragma once
#include "scriptcommon.h"
#include <list>
#include <stack>
#include <string>
#include "ScriptVarInfo.h"
#include "EScriptVariableType.h"
#include "ScriptVarTypeMgr.h"

#pragma warning(disable : 4996) 

namespace zlscript
{

	struct tagScriptVarStack
	{
		tagScriptVarStack(int size = 16)
		{
			m_vData.resize(size);
			nSize = size;
			nIndex = 0;
		}
		~tagScriptVarStack();
		std::vector<CBaseVar*> m_vData;

		unsigned int nSize;
		unsigned int nIndex;
	};
#define STACK_CHECK_SIZE(stack) {\
		if (stack.nIndex >= stack.nSize)\
		{\
			stack.nSize += 16;\
			stack.m_vData.resize(stack.nSize);\
		}\
	}

#define STACK_PUSH_COPY(stack,var) {\
		STACK_CHECK_SIZE(stack)\
		SCRIPTVAR_COPY_VAR(stack.m_vData[stack.nIndex],var);\
		stack.nIndex++;\
	}
#define STACK_PUSH_MOVE(stack,var) {\
		STACK_CHECK_SIZE(stack)\
		SCRIPTVAR_MOVE_VAR(stack.m_vData[stack.nIndex],var);\
		stack.nIndex++;\
	}

//#define STACK_PUSH_VAR(stack,intvar) {\
//		StackVarInfo var(intvar);\
//		STACK_PUSH(stack, var);\
//	}
//#define STACK_PUSH_INTERFACE(stack,point) {\
//		StackVarInfo var;\
//		SCRIPTVAR_SET_INTERFACE_POINT(var,point);\
//		STACK_PUSH(stack, var);\
//	}
#define STACK_PUSH_FRONT(stack,var) {\
		STACK_CHECK_SIZE(stack)\
		memcpy(&stack.m_vData[1],&stack.m_vData[0],sizeof(CBaseVar*)*(stack.nIndex));\
		SCRIPTVAR_COPY_VAR(stack.m_vData[0],var);\
		stack.nIndex++;\
	}
#define STACK_PUSH_FRONT_2(stack,var,var2) {\
		if (stack.nIndex+1 >= stack.nSize)\
		{\
			stack.nSize += 16;\
			stack.m_vData.resize(stack.nSize);\
		}\
		memcpy(&stack.m_vData[2],&stack.m_vData[0],sizeof(CBaseVar*)*(stack.nIndex));\
		SCRIPTVAR_COPY_VAR(stack.m_vData[0],var);\
		SCRIPTVAR_COPY_VAR(stack.m_vData[1],var2);\
		stack.nIndex+=2;\
	}
#define STACK_POP(stack,pVar) {\
		if (stack.nIndex>0)\
		{\
			SCRIPTVAR_MOVE_VAR(pVar,stack.m_vData[stack.nIndex-1]);\
			stack.nIndex--;\
		}\
	}
#define STACK_POP_FRONT(stack,num){\
		unsigned int i = 0;\
		for (; i < num&&i<stack.nIndex; i++)\
			SCRIPTVAR_RELEASE(stack.m_vData[i]);\
		if (i < stack.nIndex){\
			memcpy(&stack.m_vData[0],&stack.m_vData[i],sizeof(CBaseVar*)*(stack.nIndex-i));\
			memset(&stack.m_vData[stack.nIndex-i],0,sizeof(CBaseVar*)*i);\
			stack.nIndex-=i;\
		}\
	}
#define STACK_GET(stack,pVar) {\
		if (stack.nIndex>0)\
		{\
			SCRIPTVAR_COPY_VAR(pVar,stack.m_vData[stack.nIndex-1])\
		}\
	}
#define STACK_GET_INDEX(stack,pVar,index) {\
		if (stack.nIndex>index)\
		{\
			SCRIPTVAR_COPY_VAR(pVar,stack.m_vData[index]);\
		}\
	}
#define STACK_COPY(stackDes,stackSrc){\
		for (unsigned int i = 0; i < stackSrc.nIndex; i++)\
		{\
			STACK_PUSH_COPY(stackDes,stackSrc.m_vData[i]);\
		}\
	}
#define STACK_COPY_BEGIN(stackDes,stackSrc,begin){\
		for (unsigned int i = begin; i < stackSrc.nIndex; i++)\
		{\
			STACK_PUSH_COPY(stackDes,stackSrc.m_vData[i]);\
		}\
	}
#define STACK_MOVE_BACK(stackDes,stackSrc,begin,size){\
		while (stackDes.nIndex+(size) >= stackDes.nSize)\
		{\
			stackDes.nSize += 16;\
			stackDes.m_vData.resize(stackDes.nSize);\
		}\
		if (stackSrc.nIndex - (begin) < (size))\
		{\
			memcpy(&stackDes.m_vData[stackDes.nIndex], &stackSrc.m_vData[(begin)], sizeof(CBaseVar*)* (stackSrc.nIndex - (begin))); \
			memset( &stackSrc.m_vData[(begin)],0,sizeof(CBaseVar*)* (stackSrc.nIndex - ((begin))));\
			stackDes.nIndex += stackSrc.nIndex - (begin);\
		}\
		else\
		{\
			memcpy(&stackDes.m_vData[stackDes.nIndex], &stackSrc.m_vData[begin], sizeof(CBaseVar*)* (size)); \
			memset(&stackSrc.m_vData[begin],0,sizeof(CBaseVar*)* (size));\
			stackDes.nIndex += (size);\
		}\
	}
#define STACK_MOVE_ALL_BACK(stackDes,stackSrc,begin){\
		while (stackDes.nIndex+stackSrc.nIndex -(begin) >= stackDes.nSize)\
		{\
			stackDes.nSize += 16;\
			stackDes.m_vData.resize(stackDes.nSize);\
		}\
		if ((begin) < stackSrc.nIndex){\
			memcpy(&stackDes.m_vData[stackDes.nIndex], &stackSrc.m_vData[(begin)], sizeof(CBaseVar*)* (stackSrc.nIndex - (begin))); \
			memset(&stackSrc.m_vData[(begin)],0,sizeof(CBaseVar*)* (stackSrc.nIndex - (begin)));\
			stackDes.nIndex += stackSrc.nIndex - (begin);\
			stackSrc.nIndex =(begin);\
		}\
	}
#define STACK_CLEAR(stack) {\
		while (stack.nIndex>0)\
		{\
			SCRIPTVAR_RELEASE(stack.m_vData[stack.nIndex-1]);\
			stack.nIndex--;\
		}\
	}

#define GET_VAR_4_STACK(varType, varName, stack, index) \
	varType *varName = nullptr; \
	if (stack.nIndex>index)\
	{\
		varName = dynamic_cast<varType*>(stack.m_vData[index]);\
	}

}