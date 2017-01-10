#include "syscall.h"

void threadHandler(void * arg)
{
	int n = *(int *) arg;
	char c = (char)n;
	int i;
	for(i = 0; i <10; i++)
		PutChar(c);

	UserThreadExit();
}

int main()
{
	UserThreadCreate(threadHandler, (void *) 'a');
	UserThreadCreate(threadHandler, (void *) 'b');
	UserThreadCreate(threadHandler, (void *) 'c');

	int i;
	for(i = 0; i < 500000; i++)
	{}

	UserThreadCreate(threadHandler, (void *) 'd');
	UserThreadCreate(threadHandler, (void *) 'e');
	
	return 0;
}