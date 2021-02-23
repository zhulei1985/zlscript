#include "ScriptPointInterface.h"
#include "zByteArray.h"
#include "ScriptClassAttributes.h"

namespace zlscript
{
	void CBaseScriptClassAttribute::init(unsigned short flag, unsigned short index, CScriptPointInterface* master)
	{
		m_flag = flag;
		m_index = index;
		m_master = master;
	}
	CScriptIntAttribute::operator int()
	{
		return m_val;
	}
	int CScriptIntAttribute::operator=(int val)
	{
		if (m_master && m_val != val)
		{
			m_master->ChangeScriptAttribute(m_flag, this);
		}
		m_val = val;
		return m_val;
	}

	std::string CScriptIntAttribute::ToType()
	{
		return "INT(11)";
	}
	std::string CScriptIntAttribute::ToString()
	{
		char strbuff[16];
		snprintf(strbuff, sizeof(strbuff), "%d", m_val);
		return strbuff;
	}
	bool CScriptIntAttribute::SetVal(std::string str)
	{
		m_val = atoi(str.c_str());
		return true;
	}
	void CScriptIntAttribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		AddInt2Bytes(vBuff, m_val);
	}
	bool CScriptIntAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
	{
		m_val = DecodeBytes2Int(pBuff, pos, len);
		return true;
	}

	CScriptInt64Attribute::operator __int64()
	{
		return m_val;
	}

	__int64 CScriptInt64Attribute::operator=(__int64 val)
	{
		if (m_master && m_val != val)
		{
			m_master->ChangeScriptAttribute(m_flag, this);
		}
		m_val = val;
		return m_val;
	}

	std::string CScriptInt64Attribute::ToType()
	{
		return "BIGINT(20)";
	}

	std::string CScriptInt64Attribute::ToString()
	{
		char strbuff[32];
		snprintf(strbuff, sizeof(strbuff), "%lld", m_val);
		return strbuff;
	}

	bool CScriptInt64Attribute::SetVal(std::string str)
	{
		m_val = _atoi64(str.c_str());
		return true;
	}

	void CScriptInt64Attribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		AddInt642Bytes(vBuff, m_val);
	}

	bool CScriptInt64Attribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
	{
		m_val = DecodeBytes2Int64(pBuff, pos, len);
		return true;
	}

	CScriptFloatAttribute::operator float()
	{
		return m_val;
	}

	float CScriptFloatAttribute::operator=(float val)
	{
		if (m_master && m_val != val)
		{
			m_master->ChangeScriptAttribute(m_flag, this);
		}
		m_val = val;
		return m_val;
	}

	std::string CScriptFloatAttribute::ToType()
	{
		return "FLOAT";
	}

	std::string CScriptFloatAttribute::ToString()
	{
		char strbuff[32];
		snprintf(strbuff, sizeof(strbuff), "%f", m_val);
		return strbuff;
	}

	bool CScriptFloatAttribute::SetVal(std::string str)
	{
		m_val = atof(str.c_str());
		return true;
	}

	void CScriptFloatAttribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		AddFloat2Bytes(vBuff, m_val);
	}

	bool CScriptFloatAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
	{
		m_val = DecodeBytes2Float(pBuff, pos, len);
		return true;
	}

	CScriptDoubleAttribute::operator double()
	{
		return m_val;
	}

	double CScriptDoubleAttribute::operator=(double val)
	{
		if (m_master && m_val != val)
		{
			m_master->ChangeScriptAttribute(m_flag, this);
		}
		m_val = val;
		return m_val;
	}

	std::string CScriptDoubleAttribute::ToType()
	{
		return "DOUBLE";
	}

	std::string CScriptDoubleAttribute::ToString()
	{
		char strbuff[32];
		snprintf(strbuff, sizeof(strbuff),"%f", m_val);
		return strbuff;
	}

	bool CScriptDoubleAttribute::SetVal(std::string str)
	{
		m_val = atof(str.c_str());
		return true;
	}

	void CScriptDoubleAttribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		AddDouble2Bytes(vBuff, m_val);
	}

	bool CScriptDoubleAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
	{
		m_val = DecodeBytes2Double(pBuff, pos, len);
		return true;
	}
	const int CScriptStringAttribute::s_maxStrLen = 1024;

	CScriptStringAttribute::operator std::string&()
	{
		return m_val;
	}

	std::string& CScriptStringAttribute::operator=(std::string& val)
	{
		m_val = val;
		if (m_master)
		{
			m_master->ChangeScriptAttribute(m_flag, this);
		}
		return m_val;
	}

	std::string& CScriptStringAttribute::operator=(char* val)
	{
		m_val = val;
		if (m_master)
		{
			m_master->ChangeScriptAttribute(m_flag, this);
		}
		return m_val;
	}

	std::string& CScriptStringAttribute::operator=(const char* val)
	{
		m_val = val;
		if (m_master)
		{
			m_master->ChangeScriptAttribute(m_flag, this);
		}
		return m_val;
	}

	std::string CScriptStringAttribute::ToType()
	{
		if (m_flag & E_FLAG_DB_PRIMARY)
		{
			return  "VARCHAR(128)";
		}
		if (m_flag & E_FLAG_DB_UNIQUE)
		{
			return  "VARCHAR(128)";
		}
		return "TEXT";
	}

	std::string CScriptStringAttribute::ToString()
	{
		return m_val;
	}

	const char* CScriptStringAttribute::c_str()
	{
		return m_val.c_str();
	}

	bool CScriptStringAttribute::SetVal(std::string str)
	{
		m_val = str;
		return true;
	}

	void CScriptStringAttribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		AddString2Bytes(vBuff, m_val.c_str());
	}

	bool CScriptStringAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
	{
		//TODO 以后加入缓存
		char* strbuff = new char[s_maxStrLen];

		bool bResult = DecodeBytes2String(pBuff, pos, len, strbuff, s_maxStrLen);
		if (bResult)
		{
			m_val = strbuff;
		}
		delete [] strbuff;
		return bResult;
	}

	//CSyncAttributeMake CSyncAttributeMake::s_Instance;
	//CSyncAttributeMake::CSyncAttributeMake()
	//{
	//}

}