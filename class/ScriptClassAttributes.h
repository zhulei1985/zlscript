#pragma once
#include <vector>
#include <string>
#include "scriptcommon.h"
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
		virtual ~CBaseScriptClassAttribute()
		{

		}
		enum
		{
			E_FLAG_SYNC = 1,
			E_FLAG_DB = 2,
			E_FLAG_DB_PRIMARY = 4,
			E_FLAG_DB_UNIQUE = 8,
		};
		void init(unsigned short flag, unsigned short index, CScriptPointInterface* master);
		virtual void AddData2Bytes(std::vector<char>& vBuff) = 0;
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len) = 0;
		virtual std::string ToType() = 0;
		virtual std::string ToString() = 0;
		virtual bool SetVal(std::string str) =0;
		unsigned short m_flag;
		unsigned short m_index;
		CScriptPointInterface* m_master;
	
	};
	struct CScriptCharAttribute : public CBaseScriptClassAttribute
	{
		char* pPoint;
		operator char();
		char operator =(char& val);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptShortAttribute : public CBaseScriptClassAttribute
	{
		short* pPoint;
		operator short();
		short operator =(short& val);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptIntAttribute : public CBaseScriptClassAttribute
	{
		int m_val;
		operator int();
		int operator =(int& val);
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptInt64Attribute : public CBaseScriptClassAttribute
	{
		__int64 m_val;
		operator __int64();
		__int64 operator =(__int64& val);
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptFloatAttribute : public CBaseScriptClassAttribute
	{
		float m_val;
		operator float();
		float operator =(float& val);
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptDoubleAttribute : public CBaseScriptClassAttribute
	{
		double m_val;
		operator double();
		double operator =(double& val);
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void AddData2Bytes(std::vector<char>& vBuff);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	};
	struct CScriptStringAttribute : public CBaseScriptClassAttribute
	{
		static const int s_maxStrLen;
		std::string m_val;
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
	//class CSyncAttributeMake
	//{
	//public:
	//	CSyncAttributeMake();
	//	template<typename T>
	//	CBaseSyncAttribute* Create(T &val);
	//	void Remove(CBaseSyncAttribute* pAttr);
	//public:
	//	static CSyncAttributeMake* GetInstance()
	//	{
	//		return &s_Instance;
	//	}
	//private:
	//	static CSyncAttributeMake s_Instance;
	//};
}