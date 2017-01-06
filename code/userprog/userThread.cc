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
	printf ("\ndebut StartUserThread");



	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	struct userThreadParams * params = (struct userThreadParams *) f;

	int spr = machine->ReadRegister (StackReg);

	machine->WriteRegister (PCReg, params->f);
	machine->WriteRegister (NextPCReg, params->f + 4);
	machine->WriteRegister (4, params->arg);
	machine->WriteRegister (StackReg, spr - (PageSize*3));
printf("\nfin StartUserThread");
	machine->Run();
}

int do_UserThreadCreate(int f, int arg){
	struct userThreadParams * params = new(userThreadParams);

	params->arg = arg;
	params->f = f;

	Thread *newThread = new Thread ("Thread Noyau");

	newThread->Fork (StartUserThread, (int) params);
	/*while(1){
		currentThread->Yield();
	}*/
	return 0;
}

