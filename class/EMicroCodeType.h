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
	//脚本汇编的基本指令
	enum EMicroCodeType
	{
		ECODE_NONE = 0, //空

		/************计算符***********/
		ECODE_ADD = 1, //加
		ECODE_SUM = 2, //减
		ECODE_MUL = 3, //乘
		ECODE_DIV = 4, //除
		ECODE_MOD = 5, //求余
		ECODE_MINUS, //	取负数

		ECODE_CMP_EQUAL,//比较
		ECODE_CMP_NOTEQUAL,
		ECODE_CMP_BIG,
		ECODE_CMP_BIGANDEQUAL,
		ECODE_CMP_LESS,
		ECODE_CMP_LESSANDEQUAL,

		//位运算
		ECODE_BIT_AND,
		ECODE_BIT_OR,
		ECODE_BIT_XOR,
		/*************功能符************/
		ECODE_PUSH = 20,//压入变量数值到堆栈cSign:0，值 1全局变量地址,2，临时变量,3,字符串常量 cExtend:数组索引 dwPos:表示值或地址
		//ECODE_POP = 21,//从堆栈弹出数值到变量.cSign:0，值 1全局变量地址,2，临时变量,3,字符串常量 cExtend:数组索引  dwPos:或地址
		ECODE_STATEMENT_END = 22, //语句结束


		//ECODE_JMP,//无条件跳转
		//ECODE_JMP_JUDGE,//m_JudgeRegister为真跳转
		ECODE_EVALUATE,//从堆栈中取一个值放入指定地址cSign::0，全局变量，1，临时变量 cExtend:数组索引 dwPos:表示地址

		ECODE_BEGIN_CALL,//开始计算本次函数调用实际压入多少个参数

		ECODE_CALL,		//调用函数 cSign:0,回调函数, 1,cExtend:参数数量,脚本函数 dwPos:函数索引

		ECODE_BRANCH_IF,	// if分支,从堆栈中取一个值，如果非0则执行接下来的块
		ECODE_BRANCH_ELSE,	// else分支，如果之前的if没有执行，则执行
		ECODE_CYC_IF,		//专门用于循环条件的判断
		ECODE_BLOCK,	//块开始标志 cSign: cExtend: dwPos:表示块大小
		ECODE_BLOCK_CYC,//放在块尾，执行到此返回块头
		ECODE_BREAK,	//退出此块
		ECODE_RETURN,	//退出函数
		ECODE_CLEAR_PARAM,//清空堆栈里的参数

		ECODE_CALL_CLASS_FUN,//调用类函数 cSign:类函数索引 cExtend:参数数量,脚本函数 dwPos:类指针索引

		ECODE_NEW_CLASS, //新建一个类实例
		ECODE_RELEASE_CLASS,//释放一个类实例
		/*************标识符************/
		ECODE_INT = 200,//接下来的变量取成INT
		ECODE_DOUDLE = 201,//接下来的变量取成double
		ECODE_STRING = 202,//接下来的变量取成string
		ECODE_CLASSPOINT = 203,//类指针

		ECODE_Define_INT = 210,//定义INT临时变量 dwPos:为数组元素数量，如果为0或1表示不是数组
		ECODE_Define_DOUDLE = 211,//定义double临时变量 dwPos:为数组元素数量，如果为0或1表示不是数组
		ECODE_Define_STRING = 212,//定义string临时变量 dwPos:为数组元素数量，如果为0或1表示不是数组
	};
}