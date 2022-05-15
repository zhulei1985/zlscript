#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include "ScriptVarInfo.h"
namespace zlscript
{
	struct stGlobalVar
	{
	public:
		stGlobalVar()
		{

		}
		stGlobalVar(const stGlobalVar& var)
		{
			index = var.index;
			pVar = var.pVar;
		}
		unsigned int index{0xffffffff};
		CBaseVar* pVar{nullptr};

		std::mutex lock;
	};
	class CScriptGlobalVarMgr
	{
	public:
		CScriptGlobalVarMgr();
		~CScriptGlobalVarMgr();
	public:
		static CScriptGlobalVarMgr* GetInstance()
		{
			return &s_Instance;
		}
	private:
		static CScriptGlobalVarMgr s_Instance;
	public:
		bool New(std::string name, int type);
		stGlobalVar* Get(std::string name);
		stGlobalVar* Get(unsigned int index);

		void Revert(stGlobalVar *pGlobalVar);
	private:
		std::unordered_map<std::string, unsigned int> m_mapDicName2Index;
		//全局变量库
		std::vector<stGlobalVar> m_vGlobalVar;//变量

		std::mutex m_lock;
	};
}