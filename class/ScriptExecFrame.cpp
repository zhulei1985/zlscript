#include "ScriptEventMgr.h"
#include "ScriptExecFrame.h"

namespace zlscript
{
	CScriptExecFrame::CScriptExecFrame()
	{
	}
	CScriptExecFrame::~CScriptExecFrame()
	{
		Clear();
	}
	void CScriptExecFrame::OnInit()
	{
		m_nEventListIndex = CScriptEventMgr::GetInstance()->AssignID();
		CScriptEventMgr::GetInstance()->RegisterChannel(m_nEventListIndex);
	}
	void CScriptExecFrame::OnUpdate()
	{
		std::vector<tagScriptEvent*> vEvent;
		CScriptEventMgr::GetInstance()->GetEventByChannel(m_nEventListIndex, vEvent);
		for (unsigned int i = 0; i < m_vecEventIndexs.size(); i++)
		{
			CScriptEventMgr::GetInstance()->GetEventByChannel(m_vecEventIndexs[i], vEvent);
		}

		for (auto it = m_vecActiveEvent.begin(); it != m_vecActiveEvent.end(); it++)
		{
			CScriptEventMgr::GetInstance()->GetEventByType(*it, vEvent);
		}

		for (size_t i = 0; i < vEvent.size(); i++)
		{
			tagScriptEvent* pEvent = vEvent[i];
			if (pEvent)
			{
				auto it = m_mapEventProcess.find(pEvent->nEventType);
				if (it != m_mapEventProcess.end())
				{
					it->second(pEvent->nSendID, pEvent->m_Parm);
				}
			}
			CScriptEventMgr::GetInstance()->ReleaseEvent(pEvent);
		}
		vEvent.clear();
	}
	void CScriptExecFrame::Clear()
	{
		CScriptEventMgr::GetInstance()->RemoveChannel(m_nEventListIndex);
		for (unsigned int i = 0; i < m_vecEventIndexs.size(); i++)
		{
			CScriptEventMgr::GetInstance()->RemoveChannel(m_vecEventIndexs[i]);
		}
		m_nEventListIndex = 0;
		m_vecEventIndexs.clear();
		m_mapEventProcess.clear();
		m_vecActiveEvent.clear();
	}
	void CScriptExecFrame::InitEvent(int nEventType, EventProcessFun fun, bool bActive)
	{
		m_mapEventProcess[nEventType] = fun;
		if (bActive)
			m_vecActiveEvent.push_back(nEventType);
	}


}