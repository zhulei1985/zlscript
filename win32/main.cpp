#include "ZLScript.h"
int main()
{
	zlscript::InitScript();
	zlscript::LoadFile("test.script");
	zlscript::CScriptVirtualMachine* pMachine = new zlscript::CScriptVirtualMachine;
	pMachine->SetEventIndex(0);

	RUNSCRIPT(pMachine, "main", nullptr);

	while (pMachine->Exec(100, 9999)==0)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}


}