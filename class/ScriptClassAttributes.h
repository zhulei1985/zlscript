#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include "scriptcommon.h"
#include <atomic>
#include <mutex>
#include "ScriptVarInfo.h"
namespace zlscript
{
	class CScriptPointInterface;
	struct CBaseScriptClassAttribute;
	class IClassAttributeObserver
	{
	public:
		virtual void ChangeScriptAttribute(CBaseScriptClassAttribute* pAttr, StackVarInfo& old) = 0;
		virtual void RegisterScriptAttribute(CBaseScriptClassAttribute* pAttr) = 0;
		virtual void RemoveScriptAttribute(CBaseScriptClassAttribute* pAttr) = 0;
	};
	struct CBaseScriptClassAttribute
	{
		CBaseScriptClassAttribute()
		{
			m_pMaster = nullptr;
			m_flag = 0;
			m_index = 0;
		}
		CBaseScriptClassAttribute(const char *pName,unsigned short flag, unsigned short index, CScriptPointInterface* pMaster)
		{
			init(pName,flag, index, pMaster);
		}
		virtual ~CBaseScriptClassAttribute();
		enum
		{
			E_FLAG_NONE = 0,
			E_FLAG_SYNC = 1,
			E_FLAG_DB = 2,
			E_FLAG_DB_PRIMARY = 4,
			E_FLAG_DB_UNIQUE = 8,
		};
		void init(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* pMaster);
		virtual void AddData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint) = 0;
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint) = 0;
		virtual void AddChangeData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint)
		{
			AddData2Bytes(vBuff, vOutClassPoint);
		}
		//virtual bool DecodeChangeData4Bytes(char* pBuff, int& pos, unsigned int len)
		//{
		//	return DecodeData4Bytes(pBuff, pos, len);
		//}
		virtual std::string ToType() = 0;
		virtual std::string ToString() = 0;
		virtual StackVarInfo ToScriptVal() { return StackVarInfo(); }
		virtual bool SetVal(StackVarInfo& var) { return false; }
		virtual bool SetVal(std::string str) =0;
		virtual void ClearChangeFlag(){}
		unsigned short m_flag;
		unsigned short m_index;
		std::string m_strAttrName;
		CScriptPointInterface* m_pMaster;

		void AddObserver(IClassAttributeObserver* pObserver);
		void NotifyObserver(StackVarInfo old);
		void RemoveObserver(IClassAttributeObserver* pObserver);

	private:
		std::vector<IClassAttributeObserver*> m_vObserver;
		std::mutex m_ObserverLock;
	};
	//struct CScriptCharAttribute : public CBaseScriptClassAttribute
	//{
	//	char m_val;
	//	operator char();
	//	char operator =(char val);
	//	char operator =(char& val);
	//	virtual void AddData2Bytes(std::vector<char>& vBuff);
	//	virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	//};
	//struct CScriptShortAttribute : public CBaseScriptClassAttribute
	//{
	//	short m_val;
	//	operator short();
	//	short operator =(short& val);
	//	virtual void AddData2Bytes(std::vector<char>& vBuff);
	//	virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len);
	//};
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
		virtual StackVarInfo ToScriptVal();
		bool SetVal(std::string str);
		bool SetVal(StackVarInfo& var);
		virtual void AddData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint);
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
		virtual StackVarInfo ToScriptVal();
		bool SetVal(std::string str);
		bool SetVal(StackVarInfo& var);
		virtual void AddData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint);
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
		virtual StackVarInfo ToScriptVal();
		bool SetVal(std::string str);
		bool SetVal(StackVarInfo& var);
		virtual void AddData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint);
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
		virtual StackVarInfo ToScriptVal();
		bool SetVal(std::string str);
		bool SetVal(StackVarInfo& var);
		virtual void AddData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint);
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
		virtual StackVarInfo ToScriptVal();
		const char* c_str();
		bool SetVal(std::string str);
		bool SetVal(StackVarInfo& var);
		virtual void AddData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint);
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
		virtual void AddData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual void AddChangeData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint);
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
		virtual void AddData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual void AddChangeData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint);
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
		operator PointVarInfo& ();
		PointVarInfo& operator =(CScriptPointInterface* pPoint);
		PointVarInfo& operator =(CScriptBasePointer* pPoint);
		PointVarInfo& operator =(__int64 val);

		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		bool SetVal(StackVarInfo& var);
		virtual void AddData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint);
	};

	struct CScriptVarAttribute : public CBaseScriptClassAttribute
	{
		CScriptVarAttribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master) :
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			//m_val = 0;
			unsigned short noflag = ~(E_FLAG_DB | E_FLAG_DB_PRIMARY | E_FLAG_DB_UNIQUE);
			m_flag = m_flag & noflag;
		}
		StackVarInfo m_val;
		std::mutex m_lock;
		operator StackVarInfo&();
		StackVarInfo& operator =(StackVarInfo val);

		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		bool SetVal(StackVarInfo& var);
		virtual void AddData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint);
	};

	struct CScriptVarArrayAttribute : public CBaseScriptClassAttribute
	{
		CScriptVarArrayAttribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master) :
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			//m_val = 0;
			//指针不能用于存档，只能用于同步
			unsigned short noflag = ~(E_FLAG_DB | E_FLAG_DB_PRIMARY | E_FLAG_DB_UNIQUE);
			m_flag = m_flag & noflag;
		}
		std::vector<StackVarInfo> m_vecVal;
		std::set<unsigned int> m_setFlag;
		std::mutex m_lock;
		void SetSize(unsigned int size);
		unsigned int GetSize();
		bool SetVal(unsigned int index, StackVarInfo Val);
		StackVarInfo GetVal(unsigned int index);

		void PushBack(StackVarInfo Val);
		bool Remove(unsigned int index);

		void clear();
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void ClearChangeFlag();
		virtual void AddData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual void AddChangeData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint);
	};
	struct CScriptVar2VarMapAttribute : public CBaseScriptClassAttribute
	{
		CScriptVar2VarMapAttribute(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master) :
			CBaseScriptClassAttribute(pName, flag, index, master)
		{
			//m_val = 0;
			//指针不能用于存档，只能用于同步
			unsigned short noflag = ~(E_FLAG_DB | E_FLAG_DB_PRIMARY | E_FLAG_DB_UNIQUE);
			m_flag = m_flag & noflag;
		}
		std::unordered_map<StackVarInfo, StackVarInfo, hash_SV> m_val;
		std::unordered_set<StackVarInfo, hash_SV> m_setFlag;
		std::mutex m_lock;
		unsigned int GetSize();
		bool SetVal(StackVarInfo index, StackVarInfo val);
		StackVarInfo GetVal(StackVarInfo index);
		bool Remove(StackVarInfo index);

		void clear();
		virtual std::string ToType();
		virtual std::string ToString();
		bool SetVal(std::string str);
		virtual void ClearChangeFlag();
		virtual void AddData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual void AddChangeData2Bytes(std::vector<char>& vBuff, std::vector<PointVarInfo>& vOutClassPoint);
		virtual bool DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<PointVarInfo>& vOutClassPoint);
	};
}