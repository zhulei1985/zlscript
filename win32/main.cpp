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

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
	ATTR_INT(aaa,1);

	int Add2Script(CScriptCallState* pState)
	{
		if (pState == nullptr)
		{
			return ECALLBACK_ERROR;
		}
		int nVal1 = pState->PopIntVarFormStack();
		int nVal2 = pState->PopIntVarFormStack();
		aaa = nVal1 + nVal2;
		//printf("event %d\n", pState->m_pMachine->GetEventIndex());
		pState->SetResult((__int64)aaa);
		return ECALLBACK_FINISH;
	}
};
void DebugPrint(const char* pStr)
{
	if (pStr)
		printf("%s\n", pStr);
}
void DebugPrintToFile(const char* pStr)
{
	if (pStr)
	{
		FILE* fp = fopen("testlog.txt", "a");
		if (fp)
		{
			fprintf(fp, "%s\n", pStr);
			fclose(fp);
		}
	}
}
int main()
{
	zlscript::InitScript();
	zlscript::CScriptDebugPrintMgr::GetInstance()->RegisterCallBack_PrintFun(DebugPrint);
	//zlscript::CScriptDebugPrintMgr::GetInstance()->RegisterCallBack_PrintFun(DebugPrintToFile);
	//注册类和类函数
	RegisterClassType("CTest", CTest);
	RegisterClassFun1("Add", CTest);

	zlscript::LoadFile("test.script");
	zlscript::CScriptCodeLoader::GetInstance()->MakeICode2Code(0);
	zlscript::CScriptCodeLoader::GetInstance()->ClearICode();
	zlscript::CScriptCodeLoader::GetInstance()->PrintAllCode("debug.txt");
	g_nThreadRunState = 1;

	//zlscript::CScriptStack stackParm;
	auto t1 = std::chrono::steady_clock::now();
	int sum = 0;
	int i = 1;
	while (i <= 1000)
	{
		int j = 1;
		while (j <= 1000)
		{
			sum = sum + i * j;
			j = j + 1;
		}
		i = i + 1;
	}
	auto t2 = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	printf("Calculation time : %d\n", duration.count());

	//for (int i = 0; i < 10; i++)
	//{
	//	std::thread tbg(BackGroundThreadFun);
	//	tbg.detach();
	//}
	zlscript::CScriptVirtualMachine Machine(1);
	Machine.InitEvent(zlscript::E_SCRIPT_EVENT_RETURN,
		std::bind(&zlscript::CScriptVirtualMachine::EventReturnFun, &Machine, std::placeholders::_1, std::placeholders::_2));
	Machine.InitEvent(zlscript::E_SCRIPT_EVENT_RUNSCRIPT,
		std::bind(&zlscript::CScriptVirtualMachine::EventRunScriptFun, &Machine, std::placeholders::_1, std::placeholders::_2));
	auto oldtime = std::chrono::steady_clock::now();
	Machine.SetInstance();
	CScriptStack parm;
	Machine.RunFunImmediately("init", parm);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	zlscript::RunScript("main");
	while (1)
	{
		auto nowTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - oldtime);
		oldtime = nowTime;
		Machine.Exec(duration.count(), 9999);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	g_nThreadRunState = 0;
}

