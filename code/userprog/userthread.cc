#include "system.h"
#include "addrspace.h"
#include "userthread.h"
#include "machine.h"
#include "syscall.h"

struct userThreadParams
{
	int arg;
	int f;
};

static void StartUserThread(int f)
{

	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	struct userThreadParams * params = (struct userThreadParams *) f;

	int spr = machine->ReadRegister (StackReg);

	machine->WriteRegister (PCReg, params->f);
	machine->WriteRegister (NextPCReg, params->f + 4);
	machine->WriteRegister (4, params->arg);
	machine->WriteRegister (StackReg, spr - (PageSize*3));
	//machine->WriteRegister (RetAddrReg, UserThreadExit);
	delete params;
	currentThread->space->BindUserThread();
	machine->Run();
}

int do_UserThreadCreate(int f, int arg)
{
	if (currentThread->isStackFull())
		return -1;
	struct userThreadParams * params = new(userThreadParams);

	params->arg = arg;
	params->f = f;

	Thread *newThread = new Thread ("Thread Noyau");
	newThread->Fork (StartUserThread, (int) params);

	return 0;
}

int do_UserThreadExit()
{
	currentThread->space->UnbindUserThread();
	currentThread->space = NULL;
	currentThread->Finish();
	return 0;
}
