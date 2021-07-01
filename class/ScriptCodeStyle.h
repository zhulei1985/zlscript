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

#ifdef  _SCRIPT_DEBUG
	struct CodeStyle
	{
		CodeStyle(unsigned int index)
		{
			nSoureWordIndex = index;
		}
		union
		{
			__int64 qwCode;
			struct
			{
				unsigned char cSign;//标志
				unsigned char cExtend;//扩展标志
				unsigned short wInstruct;//指令ID
				unsigned int dwPos;//变量地址
			};
		};
		unsigned int nSoureWordIndex;
	};
#else
	struct CodeStyle
	{
		CodeStyle(unsigned int index)
		{
		}
		union
		{
			__int64 qwCode;
			struct
			{
				unsigned char cSign;//标志
				unsigned char cExtend;//扩展标志
				unsigned short wInstruct;//指令ID
				unsigned int dwPos;//变量地址
			};
		};
	};

#endif
}