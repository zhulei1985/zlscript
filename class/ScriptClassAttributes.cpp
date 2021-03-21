#include "ZLScript.h"
#include "zByteArray.h"
#include "ScriptClassAttributes.h"
#include "ScriptSuperPointer.h"
#include "EScriptVariableType.h"
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

	unsigned int CScriptInt64toInt64MapAttribute::GetSize()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		return m_mapVal.size();
	}

	bool CScriptInt64toInt64MapAttribute::SetVal(__int64 index, __int64 nVal)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_mapVal[index] = nVal;
		m_setFlag.insert(index);
		return true;
	}

	__int64 CScriptInt64toInt64MapAttribute::GetVal(__int64 index)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		auto it = m_mapVal.find(index);
		if (it != m_mapVal.end())
		{
			return it->second;
		}
		return 0;
	}

	bool CScriptInt64toInt64MapAttribute::Remove(__int64 index)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		auto it = m_mapVal.find(index);
		if (it != m_mapVal.end())
		{
			m_mapVal.erase(it);
			m_setFlag.insert(index);
			return true;
		}
		return false;
	}

	void CScriptInt64toInt64MapAttribute::clear()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_mapVal.clear();
	}

	std::string CScriptInt64toInt64MapAttribute::ToType()
	{
		return "TEXT";
	}

	std::string CScriptInt64toInt64MapAttribute::ToString()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		std::string str;
		char strbuff[32];
		auto it = m_mapVal.begin();
		for (unsigned int i = 0; it != m_mapVal.end(); i++,it++)
		{
			if (i > 0)
			{
				str += ";";
			}
			snprintf(strbuff, sizeof(strbuff), "%lld", it->first);
			str += strbuff;
			str += ",";
			snprintf(strbuff, sizeof(strbuff), "%lld", it->second);
			str += strbuff;
		}
		return str;
	}

	bool CScriptInt64toInt64MapAttribute::SetVal(std::string str)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_setFlag.clear();
		m_mapVal.clear();
		char ch[2] = { 0,0 };
		std::string strSub;
		__int64 nFirst = 0;
		for (unsigned int i = 0; i < str.length(); i++)
		{
			char curChar = str[i];
			if (curChar == ';')
			{
				auto nSecond = _atoi64(str.c_str());
				m_mapVal[nFirst] = nSecond;
			}
			else if (curChar == ',')
			{
				nFirst = _atoi64(str.c_str());
			}
			else
			{
				strSub.push_back(curChar);
			}
		}
		if (!str.empty())
		{
			auto nSecond = _atoi64(str.c_str());
			m_mapVal[nFirst] = nSecond;
		}
		return true;
	}

	void CScriptInt64toInt64MapAttribute::ClearChangeFlag()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_setFlag.clear();
	}

	void CScriptInt64toInt64MapAttribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		AddChar2Bytes(vBuff, 1);//第一是是否全部更新，1是，0否
		AddUInt2Bytes(vBuff, m_mapVal.size());//第二个是要更新多少数据
		auto it = m_mapVal.begin();
		for (; it != m_mapVal.end(); it++)
		{
			AddInt642Bytes(vBuff, it->first);
			AddInt642Bytes(vBuff, it->second);
		}
	}

	void CScriptInt64toInt64MapAttribute::AddChangeData2Bytes(std::vector<char>& vBuff)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		AddChar2Bytes(vBuff, 0);//第一是是否全部更新，1是，0否
		AddUInt2Bytes(vBuff, m_setFlag.size());//第二个是要更新多少数据
		auto itFlag = m_setFlag.begin();
		for (; itFlag != m_setFlag.end(); itFlag++)
		{
			auto it = m_mapVal.find(*itFlag);
			if (it != m_mapVal.end())
			{
				AddInt642Bytes(vBuff, it->first);
				AddInt642Bytes(vBuff, it->second);
			}
			else
			{
				AddInt642Bytes(vBuff, *itFlag);
				AddInt642Bytes(vBuff, -1);
			}
		}
	}

	bool CScriptInt64toInt64MapAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		char mode = DecodeBytes2Char(pBuff, pos, len);
		int nNum = DecodeBytes2Int(pBuff, pos, len);
		if (mode == 1)
		{
			m_mapVal.clear();
			for (int i = 0; i < nNum; i++)
			{
				__int64 nIndex = DecodeBytes2Int64(pBuff, pos, len);
				__int64 nVal = DecodeBytes2Int64(pBuff, pos, len);
				m_mapVal[nIndex] = nVal;
			}
		}
		else
		{
			for (int i = 0; i < nNum; i++)
			{
				__int64 nIndex = DecodeBytes2Int64(pBuff, pos, len);
				__int64 nVal = DecodeBytes2Int64(pBuff, pos, len);
				if (nVal != -1)
				{
					m_mapVal[nIndex] = nVal;
				}
				else
				{
					auto it = m_mapVal.find(nIndex);
					if (it != m_mapVal.end())
					{
						m_mapVal.erase(it);
					}
				}
			}
		}
		return true;
	}

	CScriptClassPointAttribute::operator PointVarInfo& ()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		return m_val;
	}

	PointVarInfo& CScriptClassPointAttribute::operator=(CScriptPointInterface* pPoint)
	{
		// TODO: 在此处插入 return 语句
		std::lock_guard<std::mutex> Lock(m_lock);
		if (pPoint)
		{
			m_val = pPoint->GetScriptPointIndex();
		}
		else
		{
			m_val.Clear();
		}
		return m_val;
	}

	PointVarInfo& CScriptClassPointAttribute::operator=(CScriptBasePointer* pPoint)
	{
		std::lock_guard<std::mutex> Lock(m_lock);

		m_val = pPoint;

		return m_val;
	}

	PointVarInfo& CScriptClassPointAttribute::operator=(__int64 val)
	{
		// TODO: 在此处插入 return 语句
		std::lock_guard<std::mutex> Lock(m_lock);

		m_val = val;
		
		return m_val;
	}

	std::string CScriptClassPointAttribute::ToType()
	{
		return std::string();
	}

	std::string CScriptClassPointAttribute::ToString()
	{
		return std::string();
	}

	bool CScriptClassPointAttribute::SetVal(std::string str)
	{
		return false;
	}

	void CScriptClassPointAttribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (m_master)
		{
			AddInt2Bytes(vBuff, m_master->GetSyncInfo_ClassPoint2Index(m_val.pPoint));
		}
	}

	bool CScriptClassPointAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (m_master)
		{
			int index = DecodeBytes2Int(pBuff, pos, len);
			m_val = m_master->GetSyncInfo_Index2ClassPoint(index);

			return true;
		}
		return false;
	}

	void CScriptClassPointArrayAttribute::SetSize(unsigned int size)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_vecVal.resize(size);
	}

	unsigned int CScriptClassPointArrayAttribute::GetSize()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		return m_vecVal.size();
	}

	bool CScriptClassPointArrayAttribute::SetVal(unsigned int index, CScriptPointInterface* pPoint)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (index < m_vecVal.size())
		{
			m_setFlag.insert(index);
			m_vecVal[index] = pPoint->GetScriptPointIndex();
			return true;
		}
		return false;
	}

	bool CScriptClassPointArrayAttribute::SetVal(unsigned int index, CScriptBasePointer* pPoint)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (index < m_vecVal.size())
		{
			m_setFlag.insert(index);
			m_vecVal[index] = pPoint;
			return true;
		}
		return false;
	}

	bool CScriptClassPointArrayAttribute::SetVal(unsigned int index, __int64 nVal)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (index < m_vecVal.size())
		{
			m_setFlag.insert(index);
			m_vecVal[index] = nVal;
			return true;
		}
		return false;
	}

	PointVarInfo CScriptClassPointArrayAttribute::GetVal(unsigned int index)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (index < m_vecVal.size())
		{
			return m_vecVal[index];
		}
		return PointVarInfo();
	}

	void CScriptClassPointArrayAttribute::PushBack(CScriptPointInterface* pPoint)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_setFlag.insert(m_vecVal.size());
		m_vecVal.push_back(pPoint->GetScriptPointIndex());
	}

	void CScriptClassPointArrayAttribute::PushBack(CScriptBasePointer* pPoint)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_setFlag.insert(m_vecVal.size());
		m_vecVal.push_back(pPoint);
	}

	void CScriptClassPointArrayAttribute::PushBack(__int64 nVal)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_setFlag.insert(m_vecVal.size());
		m_vecVal.push_back(nVal);
	}

	bool CScriptClassPointArrayAttribute::Remove(CScriptPointInterface* pPoint)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		auto it = m_vecVal.begin();
		for (; it != m_vecVal.end(); it++)
		{
			if ((*it).pPoint && (*it).pPoint->GetPoint() == pPoint)
			{
				m_vecVal.erase(it);
				return true;
			}
		}
		return false;
	}

	bool CScriptClassPointArrayAttribute::Remove(CScriptBasePointer* pPoint)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		auto it = m_vecVal.begin();
		for (; it != m_vecVal.end(); it++)
		{
			if ((*it).pPoint == pPoint)
			{
				m_vecVal.erase(it);
				return true;
			}
		}
		return false;
	}

	bool CScriptClassPointArrayAttribute::Remove(unsigned int index)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		auto it = m_vecVal.begin();
		for (unsigned int i = 0; it != m_vecVal.end(); it++, i++)
		{
			if (i == index)
			{
				m_vecVal.erase(it);
				return true;
			}
		}
		return false;
	}

	void CScriptClassPointArrayAttribute::clear()
	{
		for (unsigned int i = 0; i < m_vecVal.size(); i++)
		{
			if (m_vecVal[i].pPoint)
			{
				m_setFlag.insert(i);
			}
			m_vecVal[i].Clear();
		}
	}

	std::string CScriptClassPointArrayAttribute::ToType()
	{
		return std::string();
	}

	std::string CScriptClassPointArrayAttribute::ToString()
	{
		return std::string();
	}

	bool CScriptClassPointArrayAttribute::SetVal(std::string str)
	{
		return false;
	}

	void CScriptClassPointArrayAttribute::ClearChangeFlag()
	{
		m_setFlag.clear();
	}

	void CScriptClassPointArrayAttribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		AddUInt2Bytes(vBuff, m_vecVal.size());//第一个是数组大小
		AddUInt2Bytes(vBuff, m_vecVal.size());//第二个是要更新多少数据
		for (unsigned int i = 0; i < m_vecVal.size(); i++)
		{
			AddUInt2Bytes(vBuff, i);
			AddInt2Bytes(vBuff, m_master->GetSyncInfo_ClassPoint2Index(m_vecVal[i].pPoint));
		}
	}

	void CScriptClassPointArrayAttribute::AddChangeData2Bytes(std::vector<char>& vBuff)
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
				AddInt2Bytes(vBuff, m_master->GetSyncInfo_ClassPoint2Index(m_vecVal[index].pPoint));
			}
			else
			{
				AddUInt2Bytes(vBuff, index);
				AddInt2Bytes(vBuff, 0);
			}
		}
	}

	bool CScriptClassPointArrayAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
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
			int val = DecodeBytes2Int(pBuff, pos, len);
			if (index < m_vecVal.size())
			{
				m_vecVal[index] = m_master->GetSyncInfo_Index2ClassPoint(val);
			}
		}
		return true;
	}

	unsigned int CScriptInt64toClassPointMapAttribute::GetSize()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		return m_val.size();
	}

	bool CScriptInt64toClassPointMapAttribute::SetVal(unsigned int index, CScriptPointInterface* pPoint)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (pPoint)
		{
			m_val[index] = pPoint->GetScriptPointIndex();
			m_setFlag.insert(index);
			return true;
		}
		return false;
	}

	bool CScriptInt64toClassPointMapAttribute::SetVal(unsigned int index, CScriptBasePointer* pPoint)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		if (pPoint)
		{
			m_val[index] = pPoint;
			m_setFlag.insert(index);
			return true;
		}
		return false;
	}

	bool CScriptInt64toClassPointMapAttribute::SetVal(__int64 index, __int64 nVal)
	{
		std::lock_guard<std::mutex> Lock(m_lock);

		m_val[index] = nVal;
		m_setFlag.insert(index);
		return true;
	}

	PointVarInfo CScriptInt64toClassPointMapAttribute::GetVal(__int64 index)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		auto it = m_val.find(index);
		if (it != m_val.end())
		{
			return it->second;
		}
		return PointVarInfo();
	}

	bool CScriptInt64toClassPointMapAttribute::Remove(__int64 index)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		auto it = m_val.find(index);
		if (it != m_val.end())
		{
			m_val.erase(it);
			m_setFlag.insert(index);
			return true;
		}
		return false;
	}

	void CScriptInt64toClassPointMapAttribute::clear()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_val.clear();
	}

	std::string CScriptInt64toClassPointMapAttribute::ToType()
	{
		return std::string();
	}

	std::string CScriptInt64toClassPointMapAttribute::ToString()
	{
		return std::string();
	}

	bool CScriptInt64toClassPointMapAttribute::SetVal(std::string str)
	{
		return false;
	}

	void CScriptInt64toClassPointMapAttribute::ClearChangeFlag()
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		m_setFlag.clear();
	}

	void CScriptInt64toClassPointMapAttribute::AddData2Bytes(std::vector<char>& vBuff)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		AddChar2Bytes(vBuff, 1);//第一是是否全部更新，1是，0否
		AddUInt2Bytes(vBuff, m_val.size());//第二个是要更新多少数据
		auto it = m_val.begin();
		for (;it != m_val.end();it++)
		{
			AddInt642Bytes(vBuff, it->first);

			AddInt2Bytes(vBuff, m_master->GetSyncInfo_ClassPoint2Index(it->second.pPoint));
		}
	}

	void CScriptInt64toClassPointMapAttribute::AddChangeData2Bytes(std::vector<char>& vBuff)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		AddChar2Bytes(vBuff, 0);//第一是是否全部更新，1是，0否
		AddUInt2Bytes(vBuff, m_setFlag.size());//第二个是要更新多少数据
		auto itFlag = m_setFlag.begin();
		for (; itFlag != m_setFlag.end(); itFlag++)
		{
			auto it = m_val.find(*itFlag);
			if (it != m_val.end())
			{
				AddInt642Bytes(vBuff, it->first);

				AddInt2Bytes(vBuff, m_master->GetSyncInfo_ClassPoint2Index(it->second.pPoint));
			}
			else
			{
				AddInt642Bytes(vBuff, *itFlag);
				AddInt2Bytes(vBuff, -1);
			}
		}
	}

	bool CScriptInt64toClassPointMapAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len)
	{
		std::lock_guard<std::mutex> Lock(m_lock);
		char mode = DecodeBytes2Char(pBuff, pos, len);
		int nNum = DecodeBytes2Int(pBuff, pos, len);
		if (mode == 1)
		{
			m_val.clear();
			for (int i = 0; i < nNum; i++)
			{
				__int64 nIndex = DecodeBytes2Int64(pBuff, pos, len);
				int nPointIndex = DecodeBytes2Int(pBuff, pos, len);
				m_val[nIndex] = m_master->GetSyncInfo_Index2ClassPoint(nPointIndex);
			}
		}
		else
		{
			for (int i = 0; i < nNum; i++)
			{
				__int64 nIndex = DecodeBytes2Int64(pBuff, pos, len);
				int nPointIndex = DecodeBytes2Int(pBuff, pos, len);
				if (nPointIndex != -1)
				{
					m_val[nIndex] = m_master->GetSyncInfo_Index2ClassPoint(nPointIndex);
				}
				else
				{
					auto it = m_val.find(nIndex);
					if (it != m_val.end())
					{
						m_val.erase(it);
					}
				}
			}
		}
		return true;
	}

}