#pragma once
#include <vector>
#include <string>
#include "scriptcommon.h"
#include <atomic>
#include <mutex>
#include "StackVarInfo.h"
namespace zlscript
{
	class CScriptPointInterface;
	struct CBaseScriptClassAttribute
	{
		CBaseScriptClassAttribute()
		{
			m_flag = 0;
			m_index = 0;
			m_master = nullptr;
		}
		CBaseScriptClassAttribute(const char *pName,unsigned short flag, unsigned short index, CScriptPointInterface* master)
		{
			init(pName,flag, index, master);
		}
		virtual ~CBaseScriptClassAttribute()
		{

		}
		enum
		{
			E_FLAG_NONE = 0,
			E_FLAG_SYNC = 1,
			E_FLAG_DB = 2,
			E_FLAG_DB_PRIMARY = 4,
			E_FLAG_DB_UNIQUE = 8,
		};
		void init(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master);
		virtual void AddData2Bytes(std::vector<char>& vBuff) = 0;
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len) = 0;
		virtual void AddChangeData2Bytes(std::vector<char>& vBuff)
		{
			AddData2Bytes(vBuff);
		}
		//virtual bool DecodeChangeData4Bytes(char* pBuff, int& pos, unsigned int len)
		//{
		//	return DecodeData4Bytes(pBuff, pos, len);
		//}
		virtual std::string ToType() = 0;
		virtual std::string ToString() = 0;
		virtual bool SetVal(std::string str) =0;
		virtual void ClearChangeFlag(){}
		unsigned short m_flag;
		unsigned short m_index;
		std::string m_strAttrName;
		CScriptPointInterface* m_master;
	
	};
	struct CScriptCharAttribute : public CBaseScriptClassAttribute
	{
		char m_val;
		operator char();
		char operator =(char val);
		char operator =(char& val);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptShortAttribute : public CBaseScriptClassAttribute
	{
		short m_val;
		operator short();
		short operator =(short& val);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptIntAttribute : public CBaseScriptClassAttribute
	{
		CScriptIntAttribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master):
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			m_val = 0;
		}
		std::atomic_int m_val;
		operator int();
		int operator =(int val);
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptInt64Attribute : public CBaseScriptClassAttribute
	{
		CScriptInt64Attribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master) :
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			m_val = 0;
		}
		std::atomic_int64_t m_val;
		operator __int64();
		__int64 operator =(__int64 val);
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptFloatAttribute : public CBaseScriptClassAttribute
	{
		CScriptFloatAttribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master) :
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			m_val = 0;
		}
		float m_val;
		std::mutex m_lock;
		operator float();
		float operator =(float val);
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptDoubleAttribute : public CBaseScriptClassAttribute
	{
		CScriptDoubleAttribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master) :
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			m_val = 0;
		}
		double m_val;
		std::mutex m_lock;
		operator double();
		double operator =(double val);
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptStringAttribute : public CBaseScriptClassAttribute
	{
		CScriptStringAttribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master) :
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			//m_val = 0;
		}
		static const int s_maxStrLen;
		std::string m_val;
		std::mutex m_lock;
		operator std::string&();
		std::string& operator =(std::string& val);
		std::string& operator =(char* val);
		std::string& operator =(const char* val);
		virtual std::string ToType();
		virtual std::string ToString();
		const char* c_str();
		bool SetVal(std::string str);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};

	struct CScriptInt64ArrayAttribute : public CBaseScriptClassAttribute
	{
		CScriptInt64ArrayAttribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master) :
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			//m_val = 0;
		}
		std::vector<__int64> m_vecVal;
		std::set<unsigned int> m_setFlag;
		std::mutex m_lock;
		void SetSize(unsigned int size);
		unsigned int GetSize();
		bool SetVal(unsigned int index, __int64 nVal);
		__int64 GetVal(unsigned int index);
		void clear();
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void ClearChangeFlag();
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual void AddChangeData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptInt64toInt64MapAttribute : public CBaseScriptClassAttribute
	{
		CScriptInt64toInt64MapAttribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master) :
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			//m_val = 0;
		}
		std::map<__int64, __int64> m_mapVal;
		std::set<__int64> m_setFlag;
		std::mutex m_lock;
		unsigned int GetSize();
		bool SetVal(__int64 index, __int64 nVal);
		__int64 GetVal(__int64 index);
		bool Remove(__int64 index);

		void clear();
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void ClearChangeFlag();
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual void AddChangeData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};

	//指针不能用于存档，只能用于同步
	struct CScriptClassPointAttribute : public CBaseScriptClassAttribute
	{
		CScriptClassPointAttribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master) :
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			//m_val = 0;
			unsigned short noflag = ~(E_FLAG_DB | E_FLAG_DB_PRIMARY | E_FLAG_DB_UNIQUE);
			m_flag = m_flag & noflag;
		}
		PointVarInfo m_val;
		std::mutex m_lock;
		operator PointVarInfo&();
		PointVarInfo& operator =(CScriptPointInterface* pPoint);
		PointVarInfo& operator =(CScriptBasePointer* pPoint);
		PointVarInfo& operator =(__int64 val);

		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};

	struct CScriptClassPointArrayAttribute : public CBaseScriptClassAttribute
	{
		CScriptClassPointArrayAttribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master) :
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			//m_val = 0;
			//指针不能用于存档，只能用于同步
			unsigned short noflag = ~(E_FLAG_DB | E_FLAG_DB_PRIMARY | E_FLAG_DB_UNIQUE);
			m_flag = m_flag & noflag;
		}
		std::vector<PointVarInfo> m_vecVal;
		std::set<unsigned int> m_setFlag;
		std::mutex m_lock;
		void SetSize(unsigned int size);
		unsigned int GetSize();
		bool SetVal(unsigned int index, CScriptPointInterface* pPoint);
		bool SetVal(unsigned int index, CScriptBasePointer* pPoint);
		bool SetVal(unsigned int index, __int64 nVal);
		PointVarInfo GetVal(unsigned int index);

		void PushBack(CScriptPointInterface* pPoint);
		void PushBack(CScriptBasePointer* pPoint);
		void PushBack(__int64 nVal);

		bool Remove(CScriptPointInterface* pPoint);
		bool Remove(CScriptBasePointer* pPoint);
		bool Remove(unsigned int index);

		void clear();
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void ClearChangeFlag();
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual void AddChangeData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptInt64toClassPointMapAttribute : public CBaseScriptClassAttribute
	{
		CScriptInt64toClassPointMapAttribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master) :
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			//m_val = 0;
			//指针不能用于存档，只能用于同步
			unsigned short noflag = ~(E_FLAG_DB | E_FLAG_DB_PRIMARY | E_FLAG_DB_UNIQUE);
			m_flag = m_flag & noflag;
		}

		std::map<__int64, PointVarInfo> m_val;
		std::set<__int64> m_setFlag;
		std::mutex m_lock;
		unsigned int GetSize();
		bool SetVal(unsigned int index, CScriptPointInterface* pPoint);
		bool SetVal(unsigned int index, CScriptBasePointer* pPoint);
		bool SetVal(__int64 index, __int64 nVal);
		PointVarInfo GetVal(__int64 index);
		bool Remove(__int64 index);

		void clear();
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void ClearChangeFlag();
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual void AddChangeData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
}