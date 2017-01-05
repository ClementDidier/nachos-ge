#include "syscall.h"

void print(char c, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		PutChar((char)(c+i));
	}
	PutChar('\n');
}

void aff(void * x)
{
	print('a',3);
}

int
main()
{
	print('a',2);
	print('b',1);
	UserThreadCreate(aff, (void *) 4);
	print('j',3);
	return 0;
}