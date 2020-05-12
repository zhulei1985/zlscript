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


#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include <time.h>
#include <string.h>

namespace zlscript
{
	typedef	unsigned long	PROCESS_ID;

	extern short g_Process_ID;
#ifdef _WIN32

#else

#define __int64 long long

#endif


#if _SCRIPT_DEBUG
#define SCRIPT_PRINT
#else
#define SCRIPT_PRINT 
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{ if(p) { delete (p); (p)=nullptr; }}
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p); (p)=nullptr; }}
#endif

//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#ifndef _WIN32
#include <string>
	inline double stod(std::string str)
	{
		double result = 0;
		bool bState = true;//true小数点前 false小数点后
		double temp = 1;//小数点后
		for (unsigned int i = 0; i < str.length(); i++)
		{
			switch (str[i])
			{
			case '0':
				if (bState)
				{
					result = result * 10;
				}
				else
				{
					temp = temp * 0.1f;
				}
				break;
			case '1':
				if (bState)
				{
					result = result * 10 + 1;
				}
				else
				{
					temp = temp * 0.1f;
					result = result + temp * 1;
				}
				break;
			case '2':
				if (bState)
				{
					result = result * 10 + 2;
				}
				else
				{
					temp = temp * 0.1f;
					result = result + temp * 2;
				}
				break;
			case '3':
				if (bState)
				{
					result = result * 10 + 3;
				}
				else
				{
					temp = temp * 0.1f;
					result = result + temp * 3;
				}
				break;
			case '4':
				if (bState)
				{
					result = result * 10 + 4;
				}
				else
				{
					temp = temp * 0.1f;
					result = result + temp * 4;
				}
				break;
			case '5':
				if (bState)
				{
					result = result * 10 + 5;
				}
				else
				{
					temp = temp * 0.1f;
					result = result + temp * 5;
				}
				break;
			case '6':
				if (bState)
				{
					result = result * 10 + 6;
				}
				else
				{
					temp = temp * 0.1f;
					result = result + temp * 6;
				}
				break;
			case '7':
				if (bState)
				{
					result = result * 10 + 7;
				}
				else
				{
					temp = temp * 0.1f;
					result = result + temp * 7;
				}
				break;
			case '8':
				if (bState)
				{
					result = result * 10 + 8;
				}
				else
				{
					temp = temp * 0.1f;
					result = result + temp * 8;
				}
				break;
			case '9':
				if (bState)
				{
					result = result * 10 + 9;
				}
				else
				{
					temp = temp * 0.1f;
					result = result + temp * 9;
				}
				break;
			case '.':
				bState = false;
				break;
			}
		}
		return result;
	}
#endif
}