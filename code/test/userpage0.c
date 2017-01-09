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

	return 0;
}