#include "syscall.h"

void threadHandler(void * arg)
{
	char c = (char)((int) arg);
	int i;
	for(i = 0; i <10; i++)
		PutChar(c);
	//PutInt(((int) arg) * 10);
	UserThreadExit();
}

int main()
{
	UserThreadCreate(threadHandler, (void *) 'a');
	UserThreadCreate(threadHandler, (void *) 'b');

	while(1) { }

	return 0;
}