#pragma once
#include <vector>
#include <string>
#include "scriptcommon.h"
#include <atomic>
#include <mutex>
#include <functional> 
#include "StackVarInfo.h"
namespace zlscript
{
	class CScriptPointInterface;
	struct CBaseScriptClassFun;
	class CScriptCallState;

	class IClassFunObserver
	{
	public:
		virtual void RegisterScriptFun(CBaseScriptClassFun* pClassFun) = 0;
	};

	struct CScriptBaseClassFunInfo
	{
		struct tagParameterInfo
		{
			int type;//1 数值 2 浮点 3 字符串 4 类指针
			int classtype;//如果是类指针，类类型值
		};
		//virtual void init() = 0;
		std::vector<tagParameterInfo> vParmeterInfo;
		virtual int RunFun(CScriptCallState* pState) = 0;
		virtual CScriptBaseClassFunInfo* Copy() = 0;
		virtual const char* GetFunName() = 0;
	};

	struct CBaseScriptClassFun
	{
		CBaseScriptClassFun()
		{
			m_index = -1;
		}
		CBaseScriptClassFun(std::string name, std::function< int(CScriptCallState*)> pFun, IClassFunObserver* pMaster, int nFlag)
		{
			init(name, pFun, pMaster, nFlag);
		}
		virtual ~CBaseScriptClassFun();

		enum
		{
			E_FLAG_NONE = 0,
			E_FLAG_SYNC = 1,//同步函数
			E_FLAG_SYNC_RELAY_FUN = 2,//需要下行节点也执行一次函数
		};

		void init(std::string name, std::function< int(CScriptCallState*)> pFun, IClassFunObserver* pMaster, int nFlag);
//}
		int RunFun(CScriptCallState* pState);

		std::function< int(CScriptCallState*)> m_fun;
		std::string m_name;
		int m_nFlag;
		unsigned int m_index;
	private:

	};

}