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
	enum EErrorType
	{
		ESIGN_ILLEGAL_END,
		ERROR_NO_PRI_SIGN,//没有优先级配置
		ERROR_NO_OPER_SIGN,
		ERROR_FORMAT_NOT,
		ERROR_VAR_TYPE_NONE, //变量类型无定义
		ERROR_VAR_NAME_ILLEGAL, //变量名非法
		ERROR_VAR_NAME_EXISTS, //变量名已存在
		ERROR_GLOBAL_VAR_CANNOT_ADD,//全局变量注册失败
		ERROR_GLOBAL_VAR_CANNOT_SET,//全局变量设置失败
		ERROR_TEMP_VAR_CANNOT_ADD,//局部变量注册失败
		ERROR_TEMP_VAR_CANNOT_SET,//局部变量设置失败
		ERROR_FUN_PARAM,
		ERROR_FUN_FORMAT_PARAM,
		ERROR_FUN_NAME_ILLEGAL,
		ERROR_FUN_NAME_EXISTS,
		ERROR_FUN_FORMAT_NO_BRACKET,
	};
}