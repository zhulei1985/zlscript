#include "ZLScript.h"
#include "zByteArray.h"
#include "ScriptClassAttributes.h"
#include "ScriptSuperPointer.h"
#include "EScriptVariableType.h"
#include "ScriptPointInterface.h"
#include "ScriptVarAssignmentMgr.h"
namespace zlscript
{
	CBaseScriptClassAttribute::~CBaseScriptClassAttribute()
	{
		for (unsigned int i = 0; i < m_vObserver.size(); i++)
		{
			auto pObserver = m_vObserver[i];
			if (pObserver)
			{
				pObserver->RemoveScriptAttribute(this);
			}
		}
		m_vObserver.clear();
	}
	void CBaseScriptClassAttribute::init(const char* pName, unsigned short flag, unsigned short index, CScriptPointInterface* pMaster)
	{
		if (pName)
		{
			m_strAttrName = pName;
		}
		m_flag = flag;
		m_index = index;
		m_pMaster = pMaster;
		
		AddObserver(pMaster);
	}
	void CBaseScriptClassAttribute::AddObserver(IClassAttributeObserver* pObserver)
	{
		std::lock_guard<std::mutex> Lock(m_ObserverLock);
		for (unsigned int i = 0; i < m_vObserver.size(); i++)
		{
			if (pObserver == m_vObserver[i])
			{
				return;
			}
		}
		pObserver->RegisterScriptAttribute(this);
		m_vObserver.push_back(pObserver);
	}

	void CBaseScriptClassAttribute::NotifyObserver(CBaseVar* pOld)
	{
		std::lock_guard<std::mutex> Lock(m_ObserverLock);
		for (unsigned int i = 0; i < m_vObserver.size(); i++)
		{
			auto pObserver = m_vObserver[i];
			if (pObserver)
			{
				pObserver->ChangeScriptAttribute(this, pOld);
			}
		}
	}

	void CBaseScriptClassAttribute::RemoveObserver(IClassAttributeObserver* pObserver)
	{
		std::lock_guard<std::mutex> Lock(m_ObserverLock);
		auto it = m_vObserver.begin();
		for (; it != m_vObserver.end(); it++)
		{
			if (*it == pObserver)
			{
				m_vObserver.erase(it);
				break;
			}
		}
	}


	CScriptIntAttribute::operator int()
	{
		return m_val.ToInt();
	}
	int CScriptIntAttribute::operator=(int val)
	{
		if (m_val.ToInt() != val)
		{
			CIntVar old(m_val);
			m_val.Set((__int64)val);
			NotifyObserver(&old);
		}
		return m_val.ToInt();
	}

	std::string CScriptIntAttribute::ToType()
	{
		return "INT(11)";
	}
	std::string CScriptIntAttribute::ToString()
	{
		char strbuff[16];
		snprintf(strbuff, sizeof(strbuff), "%d", (int)m_val.ToInt());
		return strbuff;
	}
	CBaseVar* CScriptIntAttribute::ToScriptVal()
	{
		return &m_val;
	}
	bool CScriptIntAttribute::SetVal(std::string str)
	{
		return m_val.Set(str);
	}
	bool CScriptIntAttribute::SetVal(CBaseVar* var)
	{
		return AssignVar(&m_val, var);
	}
	void CScriptIntAttribute::AddData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
	{
		AddInt2Bytes(vBuff, m_val.ToInt());
	}
	bool CScriptIntAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint)
	{
		m_val.Set((__int64)DecodeBytes2Int(pBuff, pos, len));
		return true;
	}

	CScriptInt64Attribute::operator __int64()
	{
		return m_val.ToInt();
	}

	__int64 CScriptInt64Attribute::operator=(__int64 val)
	{
		if (m_val.ToInt() != val)
		{
			CIntVar old(m_val);
			m_val.Set((__int64)val);
			NotifyObserver(&old);
		}
		return m_val.ToInt();
	}

	std::string CScriptInt64Attribute::ToType()
	{
		return "BIGINT(20)";
	}

	std::string CScriptInt64Attribute::ToString()
	{
		char strbuff[32];
		snprintf(strbuff, sizeof(strbuff), "%lld", m_val.ToInt());
		return strbuff;
	}

	CBaseVar* CScriptInt64Attribute::ToScriptVal()
	{
		return &m_val;
	}

	bool CScriptInt64Attribute::SetVal(std::string str)
	{
		return m_val.Set(str);
	}

	bool CScriptInt64Attribute::SetVal(CBaseVar* var)
	{
		return AssignVar(&m_val, var);
	}

	void CScriptInt64Attribute::AddData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
	{
		AddInt642Bytes(vBuff, m_val.ToInt());
	}

	bool CScriptInt64Attribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint)
	{
		m_val.Set(DecodeBytes2Int64(pBuff, pos, len));
		return true;
	}

	CScriptFloatAttribute::operator float()
	{
		return m_val.ToFloat();
	}

	float CScriptFloatAttribute::operator=(float val)
	{
		if (m_val.ToFloat() != val)
		{
			CFloatVar old(m_val);
			m_val.Set((double)val);
			NotifyObserver(&old);
		}
		return m_val.ToFloat();
	}

	std::string CScriptFloatAttribute::ToType()
	{
		return "FLOAT";
	}

	std::string CScriptFloatAttribute::ToString()
	{
		char strbuff[32];
		snprintf(strbuff, sizeof(strbuff), "%f", m_val.ToFloat());
		return strbuff;
	}

	CBaseVar* CScriptFloatAttribute::ToScriptVal()
	{
		return &m_val;
	}

	bool CScriptFloatAttribute::SetVal(std::string str)
	{
		return m_val.Set(str);
	}

	bool CScriptFloatAttribute::SetVal(CBaseVar* var)
	{
		return AssignVar(&m_val, var);
	}

	void CScriptFloatAttribute::AddData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
	{
		AddFloat2Bytes(vBuff, m_val.ToFloat());
	}

	bool CScriptFloatAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint)
	{
		m_val.Set((double)DecodeBytes2Float(pBuff, pos, len));
		return true;
	}

	CScriptDoubleAttribute::operator double()
	{
		return m_val.ToFloat();
	}

	double CScriptDoubleAttribute::operator=(double val)
	{
		if (m_val.ToFloat() != val)
		{
			CFloatVar old(m_val);
			m_val.Set(val);
			NotifyObserver(&old);
		}
		return m_val.ToFloat();
	}

	std::string CScriptDoubleAttribute::ToType()
	{
		return "DOUBLE";
	}

	std::string CScriptDoubleAttribute::ToString()
	{
		char strbuff[32];
		snprintf(strbuff, sizeof(strbuff),"%f", m_val.ToFloat());
		return strbuff;
	}

	CBaseVar* CScriptDoubleAttribute::ToScriptVal()
	{
		return &m_val;
	}

	bool CScriptDoubleAttribute::SetVal(std::string str)
	{
		return m_val.Set(str);
	}

	bool CScriptDoubleAttribute::SetVal(CBaseVar* var)
	{
		return AssignVar(&m_val, var);
	}

	void CScriptDoubleAttribute::AddData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
	{
		AddDouble2Bytes(vBuff, m_val.ToFloat());
	}

	bool CScriptDoubleAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint)
	{
		m_val.Set(DecodeBytes2Double(pBuff, pos, len));
		return true;
	}
	const int CScriptStringAttribute::s_maxStrLen = 1024;

	CScriptStringAttribute::operator std::string&()
	{
		return m_val.ToString();
	}

	std::string& CScriptStringAttribute::operator=(std::string& val)
	{
		if (m_val.ToString() != val)
		{
			CStringVar old(m_val);
			m_val.Set(val);
			NotifyObserver(&old);
		}
		return m_val.ToString();
	}

	std::string& CScriptStringAttribute::operator=(char* val)
	{
		if (val == nullptr)
		{
			return m_val.ToString();
		}
		if (m_val.ToString() != val)
		{
			CStringVar old(m_val);
			m_val.Set(std::string(val));
			NotifyObserver(&old);
		}
		return m_val.ToString();
	}

	std::string& CScriptStringAttribute::operator=(const char* val)
	{
		if (val == nullptr)
		{
			return m_val.ToString();
		}
		if (m_val.ToString() != val)
		{
			CStringVar old(m_val);
			m_val.Set(std::string(val));
			NotifyObserver(&old);
		}
		return m_val.ToString();
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
		return m_val.ToString();
	}

	CBaseVar* CScriptStringAttribute::ToScriptVal()
	{
		return &m_val;
	}

	const char* CScriptStringAttribute::c_str()
	{
		return m_val.ToString().c_str();
	}

	bool CScriptStringAttribute::SetVal(std::string str)
	{
		m_val.Set(str);
		return true;
	}

	bool CScriptStringAttribute::SetVal(CBaseVar* var)
	{
		return AssignVar(&m_val, var);;
	}

	void CScriptStringAttribute::AddData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
	{
		AddString2Bytes(vBuff, m_val.ToString().c_str());
	}

	bool CScriptStringAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint)
	{
		//TODO 以后加入缓存
		char* strbuff = new char[s_maxStrLen];

		bool bResult = DecodeBytes2String(pBuff, pos, len, strbuff, s_maxStrLen);
		if (bResult)
		{
			m_val.Set(std::string(strbuff));
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
		//if (m_vecVal[index] != nVal)
		//{
		//	NotifyObserver(StackVarInfo((__int64)index));
		//}
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

	void CScriptInt64ArrayAttribute::AddData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
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

	void CScriptInt64ArrayAttribute::AddChangeData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
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

	bool CScriptInt64ArrayAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint)
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

	void CScriptInt64toInt64MapAttribute::AddData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
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

	void CScriptInt64toInt64MapAttribute::AddChangeData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
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

	bool CScriptInt64toInt64MapAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint)
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


	CScriptClassPointAttribute::operator CScriptPointInterface* ()
	{
		return m_val.ToPoint();
	}

	CScriptPointInterface* CScriptClassPointAttribute::operator=(CScriptPointInterface* pPoint)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);
		m_val.Set(pPoint);
		return m_val.ToPoint();
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

	bool CScriptClassPointAttribute::SetVal(CBaseVar* var)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);

		return AssignVar(&m_val, var);
	}

	void CScriptClassPointAttribute::AddData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);
		AddInt2Bytes(vBuff, vOutClassPoint.size());
		CBaseVar* pPoint1 = &m_val;
		CBaseVar* pResult = nullptr;
		SCRIPTVAR_COPY_VAR(pResult, pPoint1);
		vOutClassPoint.push_back(dynamic_cast<CPointVar*>(pResult));
	}

	bool CScriptClassPointAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);

		unsigned int index = DecodeBytes2Int(pBuff, pos, len);
		if (index < vOutClassPoint.size())
		{
			CBaseVar* pPoint1 = &m_val;
			CBaseVar* pPoint2 = vOutClassPoint[index];
			
			return AssignVar(pPoint1, pPoint2);
		}
		return false;
	}

	CScriptVarAttribute::operator CBaseVar* ()
	{
		return m_pVal;
	}

	CBaseVar* CScriptVarAttribute::operator=(CBaseVar* val)
	{
		// TODO: 在此处插入 return 语句
		//std::lock_guard<std::mutex> Lock(m_lock);
		AssignVar(m_pVal,val);
		return m_pVal;
	}


	std::string CScriptVarAttribute::ToType()
	{
		return std::string();
	}

	std::string CScriptVarAttribute::ToString()
	{
		return std::string();
	}

	bool CScriptVarAttribute::SetVal(std::string str)
	{
		return false;
	}

	bool CScriptVarAttribute::SetVal(CBaseVar* var)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);

		SCRIPTVAR_COPY_VAR(m_pVal, var);
		return true;
	}

	void CScriptVarAttribute::AddData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);
		if (m_pMaster)
		{
			m_pMaster->AddVar2Bytes(vBuff, m_pVal, vOutClassPoint);
		}
	}

	bool CScriptVarAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);
		if (m_pMaster)
		{
			CBaseVar *pVal = m_pMaster->DecodeVar4Bytes(pBuff, pos, len, vOutClassPoint);
			SCRIPTVAR_MOVE_VAR(m_pVal, pVal);
			return true;
		}
		return false;
	}

	void CScriptVarArrayAttribute::SetSize(unsigned int size)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);
		m_vecVal.resize(size);
	}

	unsigned int CScriptVarArrayAttribute::GetSize()
	{
		//std::lock_guard<std::mutex> Lock(m_lock);
		return m_vecVal.size();
	}

	bool CScriptVarArrayAttribute::SetVal(unsigned int index, CBaseVar* Val)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);
		if (index < m_vecVal.size())
		{
			m_setFlag.insert(index);
			
			SCRIPTVAR_COPY_VAR(m_vecVal[index], Val);
			return true;
		}
		return false;
	}

	CBaseVar* CScriptVarArrayAttribute::GetVal(unsigned int index)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);
		if (index < m_vecVal.size())
		{
			return m_vecVal[index];
		}
		return nullptr;
	}

	void CScriptVarArrayAttribute::PushBack(CBaseVar* Val)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);
		m_setFlag.insert(m_vecVal.size());
		CBaseVar* pTemp = nullptr;
		SCRIPTVAR_COPY_VAR(pTemp, Val);
		m_vecVal.push_back(pTemp);
	}

	bool CScriptVarArrayAttribute::Remove(unsigned int index)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);
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

	void CScriptVarArrayAttribute::clear()
	{
		for (unsigned int i = 0; i < m_vecVal.size(); i++)
		{
			if (m_vecVal[i])
			{
				m_setFlag.insert(i);
			}
			SCRIPTVAR_RELEASE(m_vecVal[i]);
		}
	}

	std::string CScriptVarArrayAttribute::ToType()
	{
		return std::string();
	}

	std::string CScriptVarArrayAttribute::ToString()
	{
		return std::string();
	}

	bool CScriptVarArrayAttribute::SetVal(std::string str)
	{
		return false;
	}

	void CScriptVarArrayAttribute::ClearChangeFlag()
	{
		m_setFlag.clear();
	}

	void CScriptVarArrayAttribute::AddData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
	{
		if (m_pMaster == nullptr)
		{
			return;
		}
		//std::lock_guard<std::mutex> Lock(m_lock);
		AddUInt2Bytes(vBuff, m_vecVal.size());//第一个是数组大小
		AddUInt2Bytes(vBuff, m_vecVal.size());//第二个是要更新多少数据
		for (unsigned int i = 0; i < m_vecVal.size(); i++)
		{
			AddUInt2Bytes(vBuff, i);
			m_pMaster->AddVar2Bytes(vBuff, m_vecVal[i], vOutClassPoint);
		}
	}

	void CScriptVarArrayAttribute::AddChangeData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);
		AddUInt2Bytes(vBuff, m_vecVal.size());
		AddUInt2Bytes(vBuff, m_setFlag.size());
		auto it = m_setFlag.begin();
		for (; it != m_setFlag.end(); it++)
		{
			unsigned int index = *it;
			if (index < m_vecVal.size())
			{
				AddUInt2Bytes(vBuff, index);
				m_pMaster->AddVar2Bytes(vBuff, m_vecVal[index], vOutClassPoint);
			}
			else
			{
				AddUInt2Bytes(vBuff, index);
				AddInt2Bytes(vBuff, EScriptVal_None);
			}
		}
	}

	bool CScriptVarArrayAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint)
	{
		//std::lock_guard<std::mutex> Lock(m_lock);
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
				CBaseVar *pTemp = m_pMaster->DecodeVar4Bytes(pBuff, pos, len, vOutClassPoint);
				SCRIPTVAR_MOVE_VAR(m_vecVal[index], pTemp);
			}
		}
		return true;
	}

	//unsigned int CScriptVar2VarMapAttribute::GetSize()
	//{
	//	//std::lock_guard<std::mutex> Lock(m_lock);
	//	return m_val.size();
	//}

	//bool CScriptVar2VarMapAttribute::SetVal(CBaseVar* index, CBaseVar* val)
	//{
	//	//std::lock_guard<std::mutex> Lock(m_lock);

	//	auto it = m_val.find(ScriptVarKey(index));
	//	if (it != m_val.end())
	//	{
	//		m_val[ScriptVarKey(index)] = val;
	//	}
	//	else
	//	{
	//		CBaseVar* pKey = nullptr;
	//		SCRIPTVAR_COPY_VAR(pKey, index);
	//		m_val.insert(std::pair<ScriptVarKey, CBaseVar*>(ScriptVarKey(pKey), val));

	//		m_setFlag.insert(index);
	//	}
	//	
	//	return true;
	//}

	//CBaseVar* CScriptVar2VarMapAttribute::GetVal(CBaseVar* index)
	//{
	//	//std::lock_guard<std::mutex> Lock(m_lock);
	//	auto it = m_val.find(index);
	//	if (it != m_val.end())
	//	{
	//		return it->second;
	//	}
	//	return nullptr;
	//}

	//bool CScriptVar2VarMapAttribute::Remove(CBaseVar* index)
	//{
	//	//std::lock_guard<std::mutex> Lock(m_lock);
	//	auto it = m_val.find(index);
	//	if (it != m_val.end())
	//	{
	//		m_val.erase(it);
	//		m_setFlag.insert(index);
	//		return true;
	//	}
	//	return false;
	//}

	//void CScriptVar2VarMapAttribute::clear()
	//{
	//	//std::lock_guard<std::mutex> Lock(m_lock);
	//	m_val.clear();
	//}

	//std::string CScriptVar2VarMapAttribute::ToType()
	//{
	//	return std::string();
	//}

	//std::string CScriptVar2VarMapAttribute::ToString()
	//{
	//	return std::string();
	//}

	//bool CScriptVar2VarMapAttribute::SetVal(std::string str)
	//{
	//	return false;
	//}

	//void CScriptVar2VarMapAttribute::ClearChangeFlag()
	//{
	//	//std::lock_guard<std::mutex> Lock(m_lock);
	//	m_setFlag.clear();
	//}

	//void CScriptVar2VarMapAttribute::AddData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
	//{
	//	//std::lock_guard<std::mutex> Lock(m_lock);
	//	//AddChar2Bytes(vBuff, 1);//第一是是否全部更新，1是，0否
	//	//AddUInt2Bytes(vBuff, m_val.size());//第二个是要更新多少数据
	//	//auto it = m_val.begin();
	//	//for (;it != m_val.end();it++)
	//	//{
	//	//	StackVarInfo index = it->first;
	//	//	m_pMaster->AddVar2Bytes(vBuff, &index, vOutClassPoint);
	//	//	//AddChar2Bytes(vBuff, 1);
	//	//	m_pMaster->AddVar2Bytes(vBuff, &it->second, vOutClassPoint);
	//	//}
	//}

	//void CScriptVar2VarMapAttribute::AddChangeData2Bytes(std::vector<char>& vBuff, std::vector<CPointVar*>& vOutClassPoint)
	//{
	//	//std::lock_guard<std::mutex> Lock(m_lock);
	//	//AddChar2Bytes(vBuff, 0);//第一是是否全部更新，1是，0否
	//	//AddUInt2Bytes(vBuff, m_setFlag.size());//第二个是要更新多少数据
	//	//auto itFlag = m_setFlag.begin();
	//	//for (; itFlag != m_setFlag.end(); itFlag++)
	//	//{
	//	//	auto it = m_val.find(*itFlag);
	//	//	if (it != m_val.end())
	//	//	{
	//	//		StackVarInfo index = it->first;
	//	//		m_pMaster->AddVar2Bytes(vBuff, &index, vOutClassPoint);
	//	//		AddChar2Bytes(vBuff, 1);
	//	//		m_pMaster->AddVar2Bytes(vBuff, &it->second, vOutClassPoint);
	//	//	}
	//	//	else
	//	//	{
	//	//		StackVarInfo index = *itFlag;
	//	//		m_pMaster->AddVar2Bytes(vBuff, &index, vOutClassPoint);
	//	//		AddChar2Bytes(vBuff, 0);
	//	//	}
	//	//}
	//}

	//bool CScriptVar2VarMapAttribute::DecodeData4Bytes(char* pBuff, int& pos, unsigned int len, std::vector<CPointVar*>& vOutClassPoint)
	//{
	//	//std::lock_guard<std::mutex> Lock(m_lock);
	//	//char mode = DecodeBytes2Char(pBuff, pos, len);
	//	//int nNum = DecodeBytes2Int(pBuff, pos, len);
	//	//if (mode == 1)
	//	//{
	//	//	m_val.clear();
	//	//	for (int i = 0; i < nNum; i++)
	//	//	{
	//	//		StackVarInfo Index = m_pMaster->DecodeVar4Bytes(pBuff, pos, len, vOutClassPoint);
	//	//		m_val[Index] = m_pMaster->DecodeVar4Bytes(pBuff, pos, len, vOutClassPoint);
	//	//	}
	//	//}
	//	//else
	//	//{
	//	//	for (int i = 0; i < nNum; i++)
	//	//	{
	//	//		StackVarInfo Index = m_pMaster->DecodeVar4Bytes(pBuff, pos, len, vOutClassPoint);
	//	//		char cHas = DecodeBytes2Char(pBuff, pos, len);
	//	//		if (cHas != 0)
	//	//		{
	//	//			m_val[Index] = m_pMaster->DecodeVar4Bytes(pBuff, pos, len, vOutClassPoint);
	//	//		}
	//	//		else
	//	//		{
	//	//			auto it = m_val.find(Index);
	//	//			if (it != m_val.end())
	//	//			{
	//	//				m_val.erase(it);
	//	//			}
	//	//		}
	//	//	}
	//	//}
	//	return true;
	//}



}