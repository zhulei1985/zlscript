#include "ScriptPointInterface.h"
#include "zByteArray.h"
#include "ScriptClassAttributes.h"

namespace zlscript
{
	void CBaseScriptClassAttribute::init(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* master)
	{
		if (pName)
		{
			m_strAttrName = pName;
		}
		m_flag = flag;
		m_index = index;
		m_master = master;
		master->RegisterScriptClassAttr(m_flag,this);
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
		snprintf(strbuff, sizeof(strbuff), "%d", m_val.load());
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
		snprintf(strbuff, sizeof(strbuff), "%lld", m_val.load());
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
		std::lock_guard<std::mutex> Lock(m_lock);
		return m_val;
	}

	float CScriptFloatAttribute::operator=(float val)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
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
		std::lock_guard<std::mutex> Lock(m_lock);
		char strbuff[32];
		snprintf(strbuff, sizeof(strbuff), "%f", m_val);
		return strbuff;
	}

	bool CScriptFloatAttribute::SetVal(std::string str)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_val = atof(str.c_str());
		return true;
	}

	void CScriptFloatAttribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		AddFloat2Bytes(vBuff, m_val);
	}

	bool CScriptFloatAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_val = DecodeBytes2Float(pBuff, pos, len);
		return true;
	}

	CScriptDoubleAttribute::operator double()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		return m_val;
	}

	double CScriptDoubleAttribute::operator=(double val)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
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
		std::lock_guard<std::mutex> Lock(m_lock);
		char strbuff[32];
		snprintf(strbuff, sizeof(strbuff),"%f", m_val);
		return strbuff;
	}

	bool CScriptDoubleAttribute::SetVal(std::string str)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_val = atof(str.c_str());
		return true;
	}

	void CScriptDoubleAttribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		AddDouble2Bytes(vBuff, m_val);
	}

	bool CScriptDoubleAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_val = DecodeBytes2Double(pBuff, pos, len);
		return true;
	}
	const int CScriptStringAttribute::s_maxStrLen = 1024;

	CScriptStringAttribute::operator std::string&()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		return m_val;
	}

	std::string& CScriptStringAttribute::operator=(std::string& val)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (m_master && m_val != val)
		{
			m_master->ChangeScriptAttribute(m_flag, this);
		}
		m_val = val;
		return m_val;
	}

	std::string& CScriptStringAttribute::operator=(char* val)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (m_master && m_val != val)
		{
			m_master->ChangeScriptAttribute(m_flag, this);
		}
		m_val = val;
		return m_val;
	}

	std::string& CScriptStringAttribute::operator=(const char* val)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (val == nullptr)
		{
			return m_val;
		}
		if (m_master && m_val != val)
		{
			m_master->ChangeScriptAttribute(m_flag, this);
		}
		m_val = val;
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
		std::lock_guard<std::mutex> Lock(m_lock);
		return m_val;
	}

	const char* CScriptStringAttribute::c_str()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		return m_val.c_str();
	}

	bool CScriptStringAttribute::SetVal(std::string str)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_val = str;
		return true;
	}

	void CScriptStringAttribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		AddString2Bytes(vBuff, m_val.c_str());
	}

	bool CScriptStringAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
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

	void CScriptInt64ArrayAttribute::SetSize(unsigned int size)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_vecVal.resize(size, 0);
	}

	unsigned int CScriptInt64ArrayAttribute::GetSize()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		return m_vecVal.size();
	}

	bool CScriptInt64ArrayAttribute::SetVal(unsigned int index, __int64 nVal)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (m_vecVal.size() <= index)
			return false;
		if (m_master && m_vecVal[index] != nVal)
		{
			m_master->ChangeScriptAttribute(m_flag, this);
		}
		m_vecVal[index] = nVal;
		m_setFlag.insert(index);
		return true;
	}

	__int64 CScriptInt64ArrayAttribute::GetVal(unsigned int index)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (m_vecVal.size() <= index)
			return 0;
		return m_vecVal[index];
	}

	void CScriptInt64ArrayAttribute::clear()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_vecVal.clear();
	}

	std::string CScriptInt64ArrayAttribute::ToType()
	{
		return "TEXT";
	}

	std::string CScriptInt64ArrayAttribute::ToString()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		std::string str;
		char strbuff[32];
		for (unsigned int i = 0; i < m_vecVal.size(); i++)
		{
			if (i > 0)
			{
				str += ",";
			}
			snprintf(strbuff, sizeof(strbuff), "%lld", m_vecVal[i]);
			str += strbuff;
		}
		return str;
	}

	bool CScriptInt64ArrayAttribute::SetVal(std::string str)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_setFlag.clear();
		m_vecVal.clear();
		char ch[2] = { 0,0 };
		std::string strSub;
		for (unsigned int i = 0; i < str.length(); i++)
		{
			char curChar = str[i];
			if (curChar == ',')
			{
				auto val = _atoi64(str.c_str());
				m_vecVal.push_back(val);
			}
			else
			{
				strSub.push_back(curChar);
			}
		}
		if (!str.empty())
		{
			auto val = _atoi64(str.c_str());
			m_vecVal.push_back(val);
		}
		return true;
	}

	void CScriptInt64ArrayAttribute::ClearChangeFlag()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_setFlag.clear();
	}

	void CScriptInt64ArrayAttribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		AddUInt2Bytes(vBuff, m_vecVal.size());//第一个是数组大小
		AddUInt2Bytes(vBuff, m_vecVal.size());//第二个是要更新多少数据
		for (unsigned int i = 0; i < m_vecVal.size(); i++)
		{
			AddUInt2Bytes(vBuff, i);
			AddInt642Bytes(vBuff, m_vecVal[i]);
		}
	}

	void CScriptInt64ArrayAttribute::AddChangeData2Bytes(std::vector<char>& vBuff)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		AddUInt2Bytes(vBuff, m_vecVal.size());
		AddUInt2Bytes(vBuff, m_setFlag.size());
		auto it = m_setFlag.begin();
		for (; it != m_setFlag.end(); it++)
		{
			unsigned int index = *it;
			if (index < m_vecVal.size())
			{
				AddUInt2Bytes(vBuff, index);
				AddInt642Bytes(vBuff, m_vecVal[index]);
			}
			else
			{
				AddUInt2Bytes(vBuff, index);
				AddInt642Bytes(vBuff, 0);
			}
		}
	}

	bool CScriptInt64ArrayAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		unsigned int nArraySize = DecodeBytes2Int(pBuff, pos, len);
		if (m_vecVal.size() != nArraySize)
		{
			m_vecVal.resize(nArraySize);
		}
		unsigned int nSize = DecodeBytes2Int(pBuff, pos, len);
		for (unsigned int i = 0; i < nSize; i++)
		{
			unsigned int index = DecodeBytes2Int(pBuff, pos, len);
			__int64 val = DecodeBytes2Int64(pBuff, pos, len);
			if (index < m_vecVal.size())
			{
				m_vecVal[index] = val;
			}
		}
		return true;
	}

}