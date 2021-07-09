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
	enum ESignType
	{
		ESIGN_VALUE_INT, //32位整形值
		ESIGN_POS_GLOBAL_VAR,//全局变量地址
		ESIGN_POS_LOACL_VAR,//临时变量地址
		ESIGN_POS_CONST_STRING,//字符串常量地址
		ESIGN_POS_CONST_FLOAT,//浮点数常量地址
		ESIGN_POS_CONST_INT64,//64位整形常量地址
		ESIGN_REGISTER,//寄存器
	};
	enum ERegisterIndex
	{
		R_A,
		R_B,
		R_C,
		R_D,
		R_SIZE,
	};
	//脚本汇编的基本指令
	enum EMicroCodeType
	{
		ECODE_NONE = 0, //空

		/************计算符***********/
		// 所有的二元计算符,左值固定为R_A
		//	cSign:	使用ESignType的定义，右值来处
		//	cExtend:计算结果放入此寄存器
		//	dwPos:	右值根据cSign的值表示值或地址
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
		// 压入变量到堆栈。
		//	cSign:	使用ESignType的定义.变量来源
		//	dwPos:	根据cSign的值表示值或地址
		ECODE_PUSH = 20,
		// 提取堆栈中的变量。
		//	cSign:	使用ESignType的定义，变量去处
		//	cExtend:寄存器索引ERegisterIndex
		//	dwPos:	根据cSign的值表示值或地址
		ECODE_POP = 21,
		ECODE_STATEMENT_END = 22, //语句结束
		//读取变量到寄存器。
		//	cSign:	使用ESignType的定义
		//	cExtend:寄存器索引ERegisterIndex
		//	dwPos:	根据cSign的值表示值或地址
		ECODE_LOAD,
		// 移动寄存器的值
		//	cSign:	目的地类型
		//	cExtend:起点，寄存器索引ERegisterIndex
		//	dwPos:	终点，根据cSign的值表示值或地址
		ECODE_MOVE,
		//读取类成员变量到寄存器。
		//	cSign:	类对象所在寄存器 ERegisterIndex
		//	cExtend:结果放入的寄存器 ERegisterIndex
		//	dwPos:	类成员变量的索引
		ECODE_GET_CLASS_PARAM,
		// 设置寄存器内容到类成员变量。
		//	cSign:	类对象所在寄存器 ERegisterIndex
		//	cExtend:数值所在的寄存器 ERegisterIndex
		//	dwPos:	类成员变量的索引
		ECODE_SET_CLASS_PARAM,
		//ECODE_JMP,//无条件跳转
		//ECODE_JMP_JUDGE,//m_JudgeRegister为真跳转

		//ECODE_BEGIN_CALL,//开始计算本次函数调用实际压入多少个参数
		//调用回调函数
		//cSign:0,返回值存放的寄存器 ERegisterIndex
		//cExtend:参数数量
		//dwPos:函数索引
		ECODE_CALL_CALLBACK,
		//调用脚本函数 
		//cSign:0,返回值存放的寄存器 ERegisterIndex
		//cExtend:参数数量
		//dwPos:函数索引
		ECODE_CALL_SCRIPT,
		// 跳转
		//cSign:0,绝对地址跳转
		//		1,相对地址向后跳转
		//		2,相对地址向前跳转
		//dwPos:地址值
		ECODE_JUMP,
		// 寄存器值为真跳转
		//cSign:0,绝对地址跳转
		//		1,相对地址向后跳转
		//		2,相对地址向前跳转
		//cExtend:寄存器索引 ERegisterIndex
		//dwPos:地址值
		ECODE_JUMP_TRUE,
		// 寄存器值为假跳转
		//cSign:0,绝对地址跳转
		//		1,相对地址向后跳转
		//		2,相对地址向前跳转
		//cExtend:寄存器索引 ERegisterIndex
		//dwPos:地址值
		ECODE_JUMP_FALSE,
		// if分支,检查指定寄存器上的值，如果非0则执行接下来的块
		//cSign:0,判断值存放的寄存器 ERegisterIndex
		//dwPos:本分支下执行的代码块大小
		//ECODE_BRANCH_IF,
		//ECODE_BRANCH_JUMP,
		//ECODE_BRANCH_ELSE,	// else分支，如果之前的if没有执行，则执行
		//ECODE_CYC_IF,		//专门用于循环条件的判断
		//ECODE_BLOCK,	//块开始标志 cSign: cExtend: dwPos:表示块大小
		//ECODE_BLOCK_CYC,//放在块尾，执行到此返回块头
		//ECODE_BREAK,	//退出此块
		// 退出函数
		//	cExtend:返回值所在寄存器索引ERegisterIndex
		ECODE_RETURN,	//退出函数
		ECODE_CLEAR_PARAM,//清空堆栈里的参数

		//调用类函数 
		// cSign:	类对象所在寄存器，返回值也会写入在此
		// cExtend:	参数数量,
		// dwPos:	类函数索引
		ECODE_CALL_CLASS_FUN,

		// 新建一个类实例
		//cSign:结果放入寄存器索引
		//dwPos:类类型Index
		ECODE_NEW_CLASS, //新建一个类实例
		// 释放一个类实例
		//cSign:使用ESignType的定义.变量来源
		//dwPos:根据cSign的值获取类指针所在的地址
		ECODE_RELEASE_CLASS,//释放一个类实例

		//下面是中间代码
		ECODE_BREAK,
		ECODE_CONTINUE,
	};
}