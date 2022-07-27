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
#include <map>
#include <vector>
#include <mutex>
namespace zlscript
{
	template<class T>
	class CAllocatorPool
	{
	public:
		CAllocatorPool(int size);
		~CAllocatorPool();

		T* New();
		bool Release(T* pVar);
	protected:
		struct CData
		{
			char* buff{nullptr};
			int* indexes{nullptr};
			int pos{ 0 };
		};
		CData* NewData()
		{
			CData* pData = new CData();
			if (pData)
			{
				pData->buff = new char[nDataSize];
				memset(pData->buff, 0, nDataSize);
				pData->indexes = new int[nNum];

				for (int i = 0; i < nNum; i++)
				{
					pData->indexes[i] = nNum - i - 1;
				}
				pData->pos = nNum;

				printf("add datablock (%d) \n", vData.size());
				vData.push_back(pData);
			}
			return pData;
		}
		void ReleaseData(CData* pData)
		{
			if (pData)
			{
				vData.erase(pData);
				delete[] pData->buff;
				delete[] pData->indexes;
				delete pData;
			}
		}
	protected:
		unsigned int nObjSize;//单个对象大小
		unsigned int nNum;
		unsigned int nDataSize;
		std::vector<CData*> vData;

		std::mutex m_Lock;
	};
	template<class T>
	inline CAllocatorPool<T>::CAllocatorPool(int num)
	{
		nObjSize = sizeof(T);
		nNum = num;
		nDataSize = nObjSize * nNum;
		NewData();
	}
	template<class T>
	inline CAllocatorPool<T>::~CAllocatorPool()
	{
	}
	template<class T>
	inline T* CAllocatorPool<T>::New()
	{
		std::lock_guard<std::mutex> Lock(m_Lock);
		for (unsigned int i = 0; i < vData.size(); i++)
		{
			CData* pData = vData[i];
			if (pData && pData->pos > 0)
			{
				pData->pos--;
				char* pBuf = pData->buff + pData->indexes[pData->pos]* nObjSize;
				T* pObj = new (pBuf) T;
				printf("new this(%d) data(%d) buf(%lld) remain size(%d)\n",this, i, pBuf, pData->pos);
				return pObj;
			}
		}
		CData* pData = NewData();
		if (pData && pData->pos > 0)
		{
			pData->pos--;
			char* pBuf = pData->buff + pData->indexes[pData->pos] * nObjSize;
			T* pObj = new (pBuf) T;
			printf("new this(%d) data(add) buf(%lld) remain size(%d)\n", this, pBuf, pData->pos);
			return (T*)pBuf;
		}
		return nullptr;
	}
	template<class T>
	inline bool CAllocatorPool<T>::Release(T* pVar)
	{
		std::lock_guard<std::mutex> Lock(m_Lock);
		for (unsigned int i = 0; i < vData.size(); i++)
		{
			CData* pData = vData[i];
			unsigned int val = (char*)pVar - pData->buff;
			if (val < nDataSize)
			{
				pVar->~T();
				pData->indexes[pData->pos] = val/ nObjSize;
				pData->pos++;
				printf("Release this(%d) data(%d) buf(%lld) index(%d) remain size(%d)\n",this, i, pVar, val / nObjSize, pData->pos);
				return true;
			}
		}
		return false;
	}

}
 