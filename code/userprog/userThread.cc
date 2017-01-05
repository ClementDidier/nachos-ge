#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include "userThread.h"
#include "machine.h"

struct userThreadParams{
	int arg;
	int f;
};

static void StartUserThread(int f){
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	struct userThreadParams * params = (struct userThreadParams *) f;

	int pc = machine->ReadRegister (PCReg);

	machine->WriteRegister (PCReg, params->f);
	machine->WriteRegister (NextPCReg, params->f + PageSize);
	machine->WriteRegister (4, params->arg);
	machine->WriteRegister (StackReg, pc + (PageSize*3));

	machine->Run();
}

int do_UserThreadCreate(int f, int arg){
	struct userThreadParams params;

	params.arg = arg;
	params.f = f;

	Thread *newThread = new Thread ("Thread Noyau");

	newThread->Fork (StartUserThread, (int) &params);

	return 0;
}

