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
	enum
	{
		EScriptVal_None = -1,

		EScriptVal_Var = 0,

		//EScriptVal_Class = 1000,
	};

	enum
	{
		E_VAR_SCOPE_CONST,//常量
		E_VAR_SCOPE_GLOBAL,//全局变量 
		E_VAR_SCOPE_LOACL,//临时变量
		E_VAR_SCOPE_REGISTER_STACK,//寄存器堆栈
		E_VAR_SCOPE_REGISTER,
	};
}