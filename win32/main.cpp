#include "ZLScript.h"
#include <atomic>
#include <thread>
#include <chrono>
std::atomic_int g_nThreadRunState = 0;//0 退出 1 运行 2 暂停
void BackGroundThreadFun()
{
	zlscript::CScriptVirtualMachine Machine;
	Machine.InitEvent(zlscript::E_SCRIPT_EVENT_RETURN,
		std::bind(&zlscript::CScriptVirtualMachine::EventReturnFun,&Machine, std::placeholders::_1, std::placeholders::_2));
	Machine.InitEvent(zlscript::E_SCRIPT_EVENT_RUNSCRIPT,
		std::bind(&zlscript::CScriptVirtualMachine::EventRunScriptFun, &Machine, std::placeholders::_1, std::placeholders::_2));
	auto oldtime = std::chrono::steady_clock::now();
	while (g_nThreadRunState > 0)
	{
		if (g_nThreadRunState == 2)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}
		auto nowTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - oldtime);
		oldtime = nowTime;
		if (Machine.Exec(duration.count(), 9999) != 0)
		{
			break;
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
using namespace zlscript;
class CTest : public CScriptPointInterface
{
public:
	CTest()
	{
		AddClassObject(CScriptPointInterface::GetScriptPointIndex(), this);

		RegisterClassFun(Add, this, &CTest::Add2Script);
	}
	~CTest()
	{

	}

	int Add2Script(CScriptRunState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		int nVal1 = pState->PopIntVarFormStack();
		int nVal2 = pState->PopIntVarFormStack();

		pState->ClearFunParam();
		pState->PushVarToStack(nVal1 + nVal2);
		return ECALLBACK_FINISH;
	}
};
int main()
{
	zlscript::InitScript();

	//注册类和类函数
	RegisterClassType("CTest", CTest);
	RegisterClassFun1("Add", CTest);

	zlscript::LoadFile("test.script");
	g_nThreadRunState = 1;

	for (int i = 0; i < 10; i++)
	{
		std::thread tbg(BackGroundThreadFun);
		tbg.detach();
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	zlscript::RunScript("main");
	while (1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	g_nThreadRunState = 0;
}

