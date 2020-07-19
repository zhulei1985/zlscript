/****************************************************************************
	Copyright (c) 2019 ZhuLei
	Email:zhulei1985@foxmail.com

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
 ****************************************************************************/
#include <functional>
#include "ScriptStack.h"

namespace zlscript
{
	typedef std::function<void(int, zlscript::CScriptStack&)> EventProcessFun;

	//用于脚本执行，返回确认,事件接收处理的接口
	class CScriptExecFrame
	{
	public:
		CScriptExecFrame();
		~CScriptExecFrame();

	public:
		void OnInit();
		void OnUpdate();

		void Clear();
	public:
		__int64 GetEventIndex()
		{
			return m_nEventListIndex;
		}
		//函数参数 : nEventType为E_SCRIPT_EVENT_TYPE枚举,fun为处理用的函数指针,bActive为是否主动获取对应事件
		void InitEvent(int nEventType, EventProcessFun fun, bool bActive=true);
	protected:
		__int64 m_nEventListIndex;
		std::map<int, EventProcessFun> m_mapEventProcess;
		std::vector<int> m_vecActiveEvent;
	public:
		//"我"要求"别人"执行脚本
		virtual void RunTo(std::string funName, CScriptStack& pram, __int64 nReturnID, __int64 nEventIndex) = 0;
		//"我"向"别人"返回执行脚本的结果
		virtual void ResultTo(CScriptStack& pram, __int64 nReturnID, __int64 nEventIndex) = 0;

		//"别人"要求"我"执行脚本
		virtual void RunFrom(std::string funName, CScriptStack& pram, __int64 nReturnID, __int64 nEventIndex)
		{
		}
		//"别人"向"我"返回执行脚本的结果
		virtual void ResultFrom(CScriptStack& pram, __int64 nReturnID)
		{

		}
	};
}