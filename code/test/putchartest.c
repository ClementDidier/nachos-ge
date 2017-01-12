#include "syscall.h"

void threadHandler(void * arg)
{
	int n = *(int *) arg;
	//char c = (char)n;
	PutChar((char)n);
}

int main()
{
	int i, g;
	for(i = 47; i < 128; i++)
	{
		UserThreadCreate(threadHandler, (void *) &i);

		for(g = 0; g < 5000; g++) {} // Attente
	}
	
	Halt();
	return 0;
}