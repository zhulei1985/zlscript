#pragma once
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
namespace zlscript
{
	enum EScriptSentenceType
	{
		SCRIPTSENTENCE_INCLUDE, //本文件包含其他文件

		SCRIPTSENTENCE_IF,//if
		SCRIPTSENTENCE_WHILE,//while
		SCRIPTSENTENCE_ELSE,//else
		SCRIPTSENTENCE_SWITCH,//switch

		//符号
		SCRIPTSENTENCE_EQUAL,//==
		SCRIPTSENTENCE_NOTEQUAL,//!=
		SCRIPTSENTENCE_BIG,//>
		SCRIPTSENTENCE_BIGANDEQUAL,//>=
		SCRIPTSENTENCE_LESS,//<
		SCRIPTSENTENCE_LESSANDEQUAL,//<=

		SCRIPTSENTENCE_EVALUATE,// 赋值=

		SCRIPTSENTENCE_ADD, //加+
		SCRIPTSENTENCE_SUM, //减-
		SCRIPTSENTENCE_MUL, //乘*
		SCRIPTSENTENCE_DIV, //除/
		SCRIPTSENTENCE_MOD, //求余%

		//标识
		SCRIPTSENTENCE_FLAG_GOTOPOS,//跳转位置标志
	};

	enum EScriptFunType
	{
		EICODE_FUN_NO_CODE,
		EICODE_FUN_DEFAULT,
		EICODE_FUN_CAN_BREAK,//当其他脚本函数要插入执行时，退出本脚本函数
	};
}