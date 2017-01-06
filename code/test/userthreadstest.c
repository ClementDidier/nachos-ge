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
	SynchPutString("\nabc\n");

}

int
main()
{
	SynchPutString("\ndef\n");
	UserThreadCreate(aff, (void *) 4);
	UserThreadCreate(aff, (void *) 4);
	UserThreadCreate(aff, (void *) 4);

	while(1);

	return 0;
}