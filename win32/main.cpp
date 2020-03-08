#include "ZLScript.h"
#include <atomic>
#include <thread>
#include <chrono>
std::atomic_int g_nThreadRunState = 0;//0 退出 1 运行 2 暂停
void BackGroundThreadFun()
{
	zlscript::CScriptVirtualMachine Machine;
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
int main()
{
	zlscript::InitScript();
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