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
		//AddClassObject(CScriptPointInterface::GetScriptPointIndex(), this);

		//RegisterClassFun(Add, this, &CTest::Add2Script);
	}
	~CTest()
	{

	}
	ATTR_INT(aaa,1);
	ATTR_INT(bbb, 2);

	CLASS_SCRIPT_FUN(CTest, Add);

	CLASS_SCRIPT_FUN(CTest, Fun1);
	CLASS_SCRIPT_FUN(CTest, Fun2);
};
int CTest::Add2Script(CScriptCallState* pState)
{
	if (pState == nullptr)
	{
		return ECALLBACK_ERROR;
	}
	GET_VAR_4_STACK(CIntVar, pVar1, pState->m_stackRegister, 0);
	GET_VAR_4_STACK(CIntVar, pVar2, pState->m_stackRegister, 1);
	__int64 nVal1 = pVar1? pVar1->ToInt():0;
	__int64 nVal2 = pVar1 ? pVar2->ToInt() : 0;
	//int aaa = nVal1 + nVal2;
	//printf("event %d\n", pState->m_pMachine->GetEventIndex());
	CIntVar result;
	result.Set((__int64)(nVal1 + nVal2));
	pState->SetResult(&result);
	return ECALLBACK_FINISH;
}
int CTest::Fun12Script(CScriptCallState* pState)
{
	if (pState == nullptr)
	{
		return ECALLBACK_ERROR;
	}
	printf("this is test fun1\n");
	return ECALLBACK_FINISH;
}
int CTest::Fun22Script(CScriptCallState* pState)
{
	if (pState == nullptr)
	{
		return ECALLBACK_ERROR;
	}
	printf("this is test fun2\n");
	return ECALLBACK_FINISH;
}
class CSubTest : public CTest
{
public:
	CSubTest()
	{
		//AddClassObject(CScriptPointInterface::GetScriptPointIndex(), this);

		//RegisterClassFun(Add, this, &CTest::Add2Script);
	}
	~CSubTest()
	{

	}
	ATTR_INT(aaa, 1);
	ATTR_INT(bbb, 2);


	CLASS_SCRIPT_FUN(CSubTest, Fun1);
	CLASS_SCRIPT_FUN(CSubTest, Fun3);
};
int CSubTest::Fun12Script(CScriptCallState* pState)
{
	if (pState == nullptr)
	{
		return ECALLBACK_ERROR;
	}
	printf("this is sub test fun1\n");
	return ECALLBACK_FINISH;
}
int CSubTest::Fun32Script(CScriptCallState* pState)
{
	if (pState == nullptr)
	{
		return ECALLBACK_ERROR;
	}
	printf("this is sub test fun3\n");
	return ECALLBACK_FINISH;
}
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
	RegisterClassType("CSubTest", CSubTest);
	//RegisterClassFun1("Add", CTest);

	CScriptCompiler compiler;
	zlscript::LoadFile("test.script", &compiler);
	compiler.MakeExeCode();
	//zlscript::CScriptCodeLoader::GetInstance()->MakeICode2Code(0);
	//zlscript::CScriptCodeLoader::GetInstance()->ClearICode();
	zlscript::CScriptCodeLoader::GetInstance()->PrintAllCode("debug.txt");
	g_nThreadRunState = 1;

	//zlscript::CScriptStack stackParm;
	auto t1 = std::chrono::steady_clock::now();
	__int64 sum = 0;
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
	//double sum = 0;
	//double i = 0;
	//while (i < 1000000000.0)
	//{
	//	sum = sum + 2.0;
	//	i = i + 1.0;
	//}
	auto t2 = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	printf("Calculation result : %lld\n", sum);
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
	tagScriptVarStack parm;
	//Machine.RunFunImmediately("init", parm);

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

