#include "fork.h"

void ProcessHandler(int arg)
{
	AddrSpace* space = (AddrSpace*) arg;
	currentThread->space = space;
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();

	machine->Run();
}

int ForkExec(char* s)
{
	OpenFile *executable = fileSystem->Open (s);
	Thread* t = new Thread("forked thread");
	t->PID = Thread::PIDcnt;
	t->PIDcnt++;
    if (executable == NULL)
    {
		printf ("Unable to open file %s\n", s);
		return -1;
    }
    printf("%d\n", t->PID);

    AddrSpace *space = new AddrSpace (executable);
    
    delete executable;

    t->Fork(ProcessHandler, (int) space);

    currentThread->Yield();

    return 0;
}