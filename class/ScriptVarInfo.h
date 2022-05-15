#pragma once
#include "scriptcommon.h"
#include "zStringBuffer.h"
#include "zBinaryBuffer.h"
#include <zByteArray.h>
//基础变量对象
namespace zlscript
{
	//class CScriptBasePointer;
	class CScriptPointInterface;
	//struct PointVarInfo;

	class CBaseVar
	{
	public:
		int GetType() 
		{
			return m_nType;
		}
		void SetType(int type)
		{
			m_nType = type;
		}
	protected:
		int m_nType;
	public:

		virtual std::size_t GetHash() const = 0;

		virtual bool Set(std::string val) { return false; }
		virtual bool Set(CScriptPointInterface* pVal) { return false; }
		virtual bool Set(CBaseVar* pVar)
		{
			return false;
		}

	};
	class CIntVar : public CBaseVar
	{
	public:
		CIntVar(){}
		CIntVar(CIntVar& var)
		{
			nValue = var.nValue;
		}
	public:
		std::size_t GetHash() const
		{
			std::size_t h1 = std::hash<char>()(m_nType);
			std::size_t h2 = std::hash<__int64>()(nValue);
			return h1 ^ h2;
		}

		virtual __int64 ToInt();
		virtual double ToFloat();
		virtual std::string ToString();
		virtual tagByteArray ToBinary();

		virtual bool Set(CBaseVar* pVar);
		virtual bool Set(__int64 val);
		virtual bool Set(double val);
		virtual bool Set(std::string val);
		virtual bool Set(tagByteArray data);
		virtual bool Set(CScriptPointInterface* pVal);

	private:
		__int64 nValue{0};
	};

	class CFloatVar : public CBaseVar
	{
	public:
		CFloatVar() {}
		CFloatVar(CFloatVar& var)
		{
			fValue = var.fValue;
		}
	public:

		std::size_t GetHash() const
		{
			std::size_t h1 = std::hash<char>()(m_nType);
			std::size_t h2 = std::hash<double>()(fValue);
			return h1 ^ h2;
		}

		virtual __int64 ToInt();
		virtual double ToFloat();
		virtual std::string ToString();
		virtual tagByteArray ToBinary();

		virtual bool Set(CBaseVar* pVar);
		virtual bool Set(__int64 val);
		virtual bool Set(double val);
		virtual bool Set(std::string val);
		virtual bool Set(tagByteArray data);
		virtual bool Set(CScriptPointInterface* pVal);

	private:
		double fValue{ 0.f };
	};
	class CStringVar : public CBaseVar
	{
	public:
		CStringVar() {}
		CStringVar(CStringVar &var)
		{
			s_strPool.UseString(nStringIndex);
			nStringIndex = var.nStringIndex;
		}
		~CStringVar();
	public:
		std::size_t GetHash() const
		{
			std::size_t h1 = std::hash<char>()(m_nType);
			std::size_t h2 = std::hash<__int64>()(nStringIndex);
			return h1 ^ h2;
		}

		virtual __int64 ToInt();
		virtual double ToFloat();
		virtual std::string& ToString();
		virtual tagByteArray ToBinary();

		virtual bool Set(CBaseVar* pVar);
		virtual bool Set(__int64 val);
		virtual bool Set(double val);
		virtual bool Set(std::string val);
		virtual bool Set(tagByteArray data);
		virtual bool Set(CScriptPointInterface* pVal);

	private:
		void clear();
		__int64 nStringIndex{ 0 };
		static zlscript::CStringPool s_strPool;
	};
	class CBinaryVar : public CBaseVar
	{
	public:
		CBinaryVar() {}
		~CBinaryVar();
	public:

		std::size_t GetHash() const
		{
			std::size_t h1 = std::hash<char>()(m_nType);
			std::size_t h2 = std::hash<__int64>()(nBinaryIndex);
			return h1 ^ h2;
		}

		virtual tagByteArray ToBinary();

		virtual bool Set(CBaseVar* pVar);
		virtual bool Set(CScriptPointInterface* pVal);

	private:
		void clear();
		__int64 nBinaryIndex{ 0 };
		static zlscript::CBinaryPool s_binPool;
	};

	class CPointVar : public CBaseVar
	{
	public:
		CPointVar() {}
		~CPointVar();
	public:
		std::size_t GetHash() const
		{
			std::size_t h1 = std::hash<char>()(m_nType);
			std::size_t h2 = std::hash<CScriptPointInterface*>()(pPoint);
			return h1 ^ h2;
		}

		virtual CScriptPointInterface* ToPoint();

		virtual bool Set(CBaseVar* pVar);
		virtual bool Set(CScriptPointInterface* pVal);

	private:
		void clear();
		CScriptPointInterface* pPoint{nullptr};
	};

	//此类用于将CBaseVar的子类实例包装成unordered_map和unordered_set的key
	//注意，此类不负责CBaseVar的子类实例的生命周期管理
	struct ScriptVarKey
	{
		ScriptVarKey(CBaseVar* pVar)
		{
			m_pVar = pVar;
		}
		bool operator==(const ScriptVarKey& key) const {
			if (m_pVar == key.m_pVar)
			{
				return true;
			}
			if (m_pVar == nullptr || key.m_pVar == nullptr)
			{
				return false;
			}
			if (m_pVar->GetHash() == key.m_pVar->GetHash())
			{
				return true;
			}
			return false;
		}
		CBaseVar* m_pVar{nullptr};
	};
	struct hash_BaseVar
	{
		size_t operator() (zlscript::ScriptVarKey& key)
		{
			if (key.m_pVar == nullptr)
			{
				return 0;
			}
			return key.m_pVar->GetHash();
		}
	};

}