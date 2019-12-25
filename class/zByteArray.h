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

#include "stdlib.h"

#include <vector>
#include "scriptcommon.h"

namespace zlscript
{
	//#include "utf8.h"

	typedef std::vector<char> tagByteArray;

#define IFNOT_FALSE(x) if(!(x)) {return false;}
#define IFNOT_RETURN(x) if(!(x)) {return ;}

	inline __int64 GetInt64(int nVal1, int nVal2)
	{
		static union
		{
			__int64 m_nInt64;
			struct
			{
				int m_nInt32_1;
				int m_nInt32_2;
			};
		} data;
		data.m_nInt32_1 = nVal1;
		data.m_nInt32_2 = nVal2;

		return data.m_nInt64;
	}
	inline void GetInt(__int64 nIn, int& nVal1, int& nVal2)
	{
		static union
		{
			__int64 m_nInt64;
			struct
			{
				int m_nInt32_1;
				int m_nInt32_2;
			};
		} data;
		data.m_nInt64 = nIn;
		nVal1 = data.m_nInt32_1;
		nVal2 = data.m_nInt32_2;
	}

	//inline void SafeStrcpy(char *pDest, const char *pSrc, int nSize)
	//{
	//	if (pDest == NULL)
	//	{
	//		return;
	//	}
	//	if (pSrc == NULL)
	//	{
	//		pDest[0] = 0;
	//		return;
	//	}
	//	int nlen = strlen(pSrc);
	//	if (nlen < nSize)
	//	{
	//		strcpy_s(pDest,strlen(pDest),pSrc);
	//	}
	//	else
	//	{
	//		memcpy(pDest,pSrc,nSize-2);
	//		pDest[nSize-1] = 0;
	//	}
	//}

	inline bool AddChar2Bytes(char* pBuff, char Val, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return false;
		}
		if (pos >= len)
		{
			return false;
		}


		pBuff[pos++] = Val;
		return true;
	}
	inline bool AddUChar2Bytes(char* pBuff, unsigned char Val, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return false;
		}
		if (pos >= len)
		{
			return false;
		}


		pBuff[pos++] = Val;
		return true;
	}
	inline bool AddShort2Bytes(char* pBuff, short Val, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return false;
		}
		if (pos + 1 >= len)
		{
			return false;
		}
		union
		{
			short nShort;
			struct
			{
				char c1;
				char c2;
			};
		} result;

		result.nShort = Val;
		pBuff[pos++] = result.c2;
		pBuff[pos++] = result.c1;
		return true;
	}
	inline bool AddUShort2Bytes(char* pBuff, unsigned short Val, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return false;
		}

		if (pos + 1 >= len)
		{
			return false;
		}

		union
		{
			unsigned short unShort;
			struct
			{
				char c1;
				char c2;
			};
		} result;

		result.unShort = Val;
		pBuff[pos++] = result.c2;
		pBuff[pos++] = result.c1;

		//pBuff[pos++] = (char)(Val & 0x00ff);
		//pBuff[pos++] = (char)(Val >> 8);
		return true;
	}
	inline bool AddUInt2Bytes(char* pBuff, unsigned int Val, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return false;
		}

		if (pos + 3 >= len)
		{
			return false;
		}

		union
		{
			unsigned int unInt;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
			};
		} result;
		result.unInt = Val;
		pBuff[pos++] = result.c4;
		pBuff[pos++] = result.c3;
		pBuff[pos++] = result.c2;
		pBuff[pos++] = result.c1;

		//pBuff[pos++] = (char)(Val & 0x000000ff);
		//pBuff[pos++] = (char)(Val >> 8 & 0x000000ff);
		//pBuff[pos++] = (char)(Val >> 16 & 0x000000ff);
		//pBuff[pos++] = (char)(Val >> 24 & 0x000000ff);
		return true;
	}
	inline bool AddInt2Bytes(char* pBuff, int Val, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return false;
		}

		if (pos + 3 >= len)
		{
			return false;
		}

		union
		{
			int nInt;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
			};
		} result;
		result.nInt = Val;
		pBuff[pos++] = result.c4;
		pBuff[pos++] = result.c3;
		pBuff[pos++] = result.c2;
		pBuff[pos++] = result.c1;

		return true;
	}
	inline bool AddInt642Bytes(char* pBuff, __int64 Val, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return false;
		}

		if (pos + 7 >= len)
		{
			return false;
		}

		union
		{
			__int64 nInt;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
				char c5;
				char c6;
				char c7;
				char c8;
			};
		} result;
		result.nInt = Val;
		pBuff[pos++] = result.c8;
		pBuff[pos++] = result.c7;
		pBuff[pos++] = result.c6;
		pBuff[pos++] = result.c5;
		pBuff[pos++] = result.c4;
		pBuff[pos++] = result.c3;
		pBuff[pos++] = result.c2;
		pBuff[pos++] = result.c1;

		return true;
	}
	inline bool AddFloat2Bytes(char* pBuff, float Val, int& pos, int  len)
	{
		if (pBuff == nullptr)
		{
			return false;
		}
		if (pos + 3 >= len)
		{
			return false;
		}

		//memcpy(pBuff,&Val,sizeof(float));
		//pos += sizeof(float);

		union
		{
			float nfloat;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
			};
		} result;
		result.nfloat = Val;
		pBuff[pos++] = result.c4;
		pBuff[pos++] = result.c3;
		pBuff[pos++] = result.c2;
		pBuff[pos++] = result.c1;

		return true;
	}
	inline bool AddData2Bytes(char* pBuff, char* pVal, int charlen, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return false;
		}
		if (pVal == nullptr)
		{
			return false;
		}
		if (pos + charlen >= len)
		{
			return false;
		}

		AddInt2Bytes(pBuff, charlen, pos, len);
		memcpy(&pBuff[pos], pVal, charlen);
		pos += charlen;
		return true;
	}
	/////////////////以下是使用vector自动变长的数据编码///////////////////////
	inline bool AddChar2Bytes(tagByteArray& vBuff, char Val)
	{
		if (vBuff.size() >= vBuff.max_size())
		{
			return false;
		}
		vBuff.push_back(Val);
		return true;
	}
	inline bool AddUChar2Bytes(tagByteArray& vBuff, unsigned char Val)
	{
		if (vBuff.size() >= vBuff.max_size())
		{
			return false;
		}

		vBuff.push_back(Val);
		return true;
	}
	inline bool AddShort2Bytes(tagByteArray& vBuff, short Val)
	{
		if (vBuff.size() + 2 >= vBuff.max_size())
		{
			return false;
		}
		union
		{
			short nShort;
			struct
			{
				char c1;
				char c2;
			};
		} result;

		result.nShort = Val;
		vBuff.push_back(result.c2);
		vBuff.push_back(result.c1);
		return true;
	}
	inline bool AddUShort2Bytes(tagByteArray& vBuff, unsigned short Val)
	{
		if (vBuff.size() + 2 >= vBuff.max_size())
		{
			return false;
		}

		union
		{
			unsigned short unShort;
			struct
			{
				char c1;
				char c2;
			};
		} result;

		result.unShort = Val;
		vBuff.push_back(result.c2);
		vBuff.push_back(result.c1);

		//pBuff[pos++] = (char)(Val & 0x00ff);
		//pBuff[pos++] = (char)(Val >> 8);
		return true;
	}
	inline bool AddUInt2Bytes(tagByteArray& vBuff, unsigned int Val)
	{
		if (vBuff.size() + 4 >= vBuff.max_size())
		{
			return false;
		}

		union
		{
			unsigned int unInt;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
			};
		} result;
		result.unInt = Val;
		//vBuff.resize(vBuff.size()+4)
		vBuff.push_back(result.c4);
		vBuff.push_back(result.c3);
		vBuff.push_back(result.c2);
		vBuff.push_back(result.c1);

		//pBuff[pos++] = (char)(Val & 0x000000ff);
		//pBuff[pos++] = (char)(Val >> 8 & 0x000000ff);
		//pBuff[pos++] = (char)(Val >> 16 & 0x000000ff);
		//pBuff[pos++] = (char)(Val >> 24 & 0x000000ff);
		return true;
	}
	inline bool AddInt2Bytes(tagByteArray& vBuff, int Val)
	{
		if (vBuff.size() + 4 >= vBuff.max_size())
		{
			return false;
		}

		union
		{
			int nInt;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
			};
		} result;
		result.nInt = Val;
		vBuff.push_back(result.c4);
		vBuff.push_back(result.c3);
		vBuff.push_back(result.c2);
		vBuff.push_back(result.c1);

		return true;
	}
	inline bool AddInt642Bytes(tagByteArray& vBuff, __int64 Val)
	{
		if (vBuff.size() + 8 >= vBuff.max_size())
		{
			return false;
		}

		union
		{
			__int64 nInt;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
				char c5;
				char c6;
				char c7;
				char c8;
			};
		} result;
		result.nInt = Val;
		vBuff.push_back(result.c8);
		vBuff.push_back(result.c7);
		vBuff.push_back(result.c6);
		vBuff.push_back(result.c5);
		vBuff.push_back(result.c4);
		vBuff.push_back(result.c3);
		vBuff.push_back(result.c2);
		vBuff.push_back(result.c1);
		return true;
	}
	inline bool AddFloat2Bytes(tagByteArray& vBuff, float Val)
	{
		if (vBuff.size() + 4 >= vBuff.max_size())
		{
			return false;
		}

		//memcpy(pBuff,&Val,sizeof(float));
		//pos += sizeof(float);

		union
		{
			float nfloat;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
			};
		} result;
		result.nfloat = Val;
		vBuff.push_back(result.c4);
		vBuff.push_back(result.c3);
		vBuff.push_back(result.c2);
		vBuff.push_back(result.c1);
		return true;
	}
	inline bool AddDouble2Bytes(tagByteArray& vBuff, double Val)
	{
		if (vBuff.size() + 8 >= vBuff.max_size())
		{
			return false;
		}

		//memcpy(pBuff,&Val,sizeof(float));
		//pos += sizeof(float);

		union
		{
			double nfloat;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
				char c5;
				char c6;
				char c7;
				char c8;
			};
		} result;
		result.nfloat = Val;
		vBuff.push_back(result.c8);
		vBuff.push_back(result.c7);
		vBuff.push_back(result.c6);
		vBuff.push_back(result.c5);
		vBuff.push_back(result.c4);
		vBuff.push_back(result.c3);
		vBuff.push_back(result.c2);
		vBuff.push_back(result.c1);
		return true;
	}
	inline bool AddData2Bytes(tagByteArray& vBuff, char* pVal, int charlen)
	{
		if (vBuff.size() + charlen >= vBuff.max_size())
		{
			return false;
		}

		AddInt2Bytes(vBuff, charlen);
		if (charlen > 0)
		{
			int pos = vBuff.size();
			vBuff.resize(vBuff.size() + charlen);
			memcpy(&vBuff[pos], pVal, charlen);
		}

		return true;
	}
	inline bool AddData2Bytes(tagByteArray& vBuff, std::vector<unsigned char>& vIn)
	{
		if (vBuff.size() + vIn.size() >= vBuff.max_size())
		{
			return false;
		}

		AddInt2Bytes(vBuff, vIn.size());
		if (vIn.size() > 0)
		{
			int pos = vBuff.size();
			vBuff.resize(vBuff.size() + vIn.size());
			memcpy(&vBuff[pos], &vIn[0], vIn.size());
		}

		return true;
	}
	inline bool AddString2Bytes(tagByteArray& vBuff, char* pStr)
	{
		if (pStr == nullptr)
		{
			return false;
		}

		int nStrLen = strlen(pStr);
		return AddData2Bytes(vBuff, pStr, nStrLen);
		//if (len-pos < nStrLen)
		//{
		//	return false;
		//}
		//memcpy(&pBuff[pos],pStr,nStrLen);
		//pos+=nStrLen;
	}
	//inline bool AddString2Udf8Bytes(tagByteArray &vBuff, char* pStr)
	//{
	//	if (pStr == NULL)
	//	{
	//		return false;
	//	}
	//
	//	int nStrLen = strlen(pStr);
	//	char *pBuff = new char[nStrLen*2+1];
	//	int nUtfLen = ansi_to_utf8(pStr,nStrLen,pBuff,nStrLen*2+1);
	//	if (nUtfLen == 0)
	//	{
	//		AddInt2Bytes(vBuff,0);
	//		return false;
	//	}
	//	
	//	if (AddData2Bytes(vBuff,pBuff,nUtfLen) == false)
	//	{
	//		delete [] pBuff;
	//		return false;
	//	}
	//	delete [] pBuff;
	//	return true;
	//}
	inline char DecodeBytes2Char(char* pBuff, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return 0;
		}
		if (pos >= len)
		{
			return 0;
		}
		return pBuff[pos++];
	}
	inline short DecodeBytes2Short(char* pBuff, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return 0;
		}
		//short temp = 0;
		//memcpy(&temp,&pBuff[pos],2);
		//short result = pBuff[pos++];
		//result = result << 8;
		//result = pBuff[pos++];
		if (pos + 1 >= len)
		{
			return 0;
		}
		union
		{
			short ShortOut;
			struct
			{
				char c1;
				char c2;
			};
		} result;
		result.c2 = pBuff[pos++];
		result.c1 = pBuff[pos++];
		return result.ShortOut;
	}
	inline int DecodeBytes2Int(char* pBuff, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return 0;
		}
		//int nTemp = 0;
		//memcpy(&nTemp,&pBuff[pos],4);
		if (pos + 3 >= len)
		{
			return 0;
		}
		union
		{
			int IntOut;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
			};
		} result;
		result.c4 = pBuff[pos++];
		result.c3 = pBuff[pos++];
		result.c2 = pBuff[pos++];
		result.c1 = pBuff[pos++];

		return result.IntOut;
	}

	inline float DecodeBytes2Float(char* pBuff, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return 0;
		}
		//float result = (float)pBuff[pos];
		//pos += 4;
		//return result;
		if (pos + 3 >= len)
		{
			return 0;
		}
		union
		{
			float FloatOut;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
			};
		} result;
		result.c4 = pBuff[pos++];
		result.c3 = pBuff[pos++];
		result.c2 = pBuff[pos++];
		result.c1 = pBuff[pos++];

		return result.FloatOut;
	}
	inline __int64 DecodeBytes2Int64(char* pBuff, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return 0;
		}
		//int nTemp = 0;
		//memcpy(&nTemp,&pBuff[pos],4);
		if (pos + 7 >= len)
		{
			return 0;
		}

		union
		{
			__int64 IntOut;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
				char c5;
				char c6;
				char c7;
				char c8;
			};
		} result;
		result.c8 = pBuff[pos++];
		result.c7 = pBuff[pos++];
		result.c6 = pBuff[pos++];
		result.c5 = pBuff[pos++];
		result.c4 = pBuff[pos++];
		result.c3 = pBuff[pos++];
		result.c2 = pBuff[pos++];
		result.c1 = pBuff[pos++];

		return result.IntOut;
	}
	inline double DecodeBytes2Double(char* pBuff, int& pos, int len)
	{
		if (pBuff == nullptr)
		{
			return 0;
		}
		//int nTemp = 0;
		//memcpy(&nTemp,&pBuff[pos],4);
		if (pos + 7 >= len)
		{
			return 0;
		}

		union
		{
			double IntOut;
			struct
			{
				char c1;
				char c2;
				char c3;
				char c4;
				char c5;
				char c6;
				char c7;
				char c8;
			};
		} result;
		result.c8 = pBuff[pos++];
		result.c7 = pBuff[pos++];
		result.c6 = pBuff[pos++];
		result.c5 = pBuff[pos++];
		result.c4 = pBuff[pos++];
		result.c3 = pBuff[pos++];
		result.c2 = pBuff[pos++];
		result.c1 = pBuff[pos++];

		return result.IntOut;
	}
	inline bool DecodeBytes2String(char* pBuff, int& pos, int len, char* pOut, int OutMaxlen)
	{
		if (pBuff == nullptr || pOut == nullptr)
		{
			return false;
		}

		int nStrLen = DecodeBytes2Int(pBuff, pos, len);
		if (nStrLen == 0)
		{
			pOut[nStrLen] = 0;
			return true;
		}
		if (OutMaxlen < nStrLen + 1)
		{
			return false;
		}

		memcpy(pOut, &pBuff[pos], nStrLen);

		pos += nStrLen;
		pOut[nStrLen] = '\0';

		return true;
	}
	//inline bool DecodeBytes2UTF8String(char *pBuff, int &pos, int len, char *pOut,int OutMaxlen)
	//{
	//	if (pBuff == NULL || pOut == NULL)
	//	{
	//		return false;
	//	}
	//	int nStrLen = DecodeBytes2Int(pBuff,pos,len);
	//	if (nStrLen == 0)
	//	{
	//		return false;
	//	}
	//	if (OutMaxlen < nStrLen+1)
	//	{
	//		return false;
	//	}
	//
	//	utf8_to_ansi(&pBuff[pos],nStrLen,pOut,OutMaxlen);
	//
	//	pos += nStrLen;
	//
	//	return true;
	//}

#define DECODEDATA_RESULT_ERROR -2
	inline int DecodeBytes2Data(char* pBuff, int& pos, int len, char* pOut, int OutMaxlen)
	{
		if (pBuff == nullptr || pOut == nullptr)
		{
			return 0;
		}
		int nLen = DecodeBytes2Int(pBuff, pos, len);
		if (nLen < 0)
		{
			return DECODEDATA_RESULT_ERROR;
		}
		if (OutMaxlen < nLen)
		{
			return DECODEDATA_RESULT_ERROR;
		}

		memcpy(pOut, &pBuff[pos], nLen);

		pos += nLen;

		return nLen;
	}
	inline int DecodeBytes2Data(char* pBuff, int& pos, int len, std::vector<unsigned char>& vOut)
	{
		if (pBuff == nullptr)
		{
			return 0;
		}
		int nLen = DecodeBytes2Int(pBuff, pos, len);
		if (nLen < 0)
		{
			return DECODEDATA_RESULT_ERROR;
		}
		vOut.resize(nLen);
		if (nLen > 0)
			memcpy(&vOut[0], &pBuff[pos], nLen);

		pos += nLen;

		return nLen;
	}
	//bool AddInt2Bytes(char *pBuff, &Val,int &len)
	//{
	//	if (pBuff == NULL)
	//	{
	//		return false;
	//	}
	//
	//	memcpy(pBuff,&Val,sizeof(float));
	//
	//	return true;
	//}

	//inline bool GetHeadTalkSting(char *pIn, char *pHead, char *pText)
	//{
	//	if (pIn && pHead && pText)
	//	{
	//		int len = strlen(pIn);
	//		if (len > 0)
	//		{
	//			if (pIn[0] == '/')
	//			{
	//				int i = 1;
	//				int r = 0;
	//				for (; i < len; i++)
	//				{
	//					if (pIn[i] == ' ')
	//					{
	//						pHead[r] = 0;
	//						break;
	//					}
	//					pHead[r] = pIn[i];
	//					r++;
	//				}
	//				while (pIn[i] == ' ' && i < len)
	//				{
	//					i++;
	//				}
	//				strcpy_s(pText,&pIn[i]);
	//				return true;
	//			}
	//		}
	//	}
	//	return false;
	//}
};