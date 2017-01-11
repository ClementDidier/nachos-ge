#include "syscall.h"

void threadHandler(void * arg)
{
	char * c = (char*)arg;
	int i;
	for(i = 0; i <10; i++)
		PutChar(*c);

	UserThreadExit();
}

int main()
{
	char a = 'a', b = 'b';

	UserThreadCreate(threadHandler, (void *) &a);
	UserThreadCreate(threadHandler, (void *) &b);

	int i;
	for(i = 0; i < 5000; i++) { } // Attente
	PutChar('\n');

	Halt();
	
	return 0;
}