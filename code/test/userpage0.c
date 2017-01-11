#include "syscall.h"

void threadHandler(void * arg)
{
	char c = *(char *) arg;
	int i;
	for(i = 0; i <10; i++)
		PutChar(c);
}

int main()
{
	char a = 'a';
	char b = 'b';
	char c = 'c';


	UserThreadCreate(threadHandler, (void *) &a);
	UserThreadCreate(threadHandler, (void *) &b);
	UserThreadCreate(threadHandler, (void *) &c);


	Halt();

	return 0;
}
