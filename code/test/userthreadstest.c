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
	PutString("abc");
	UserThreadExit();

}

int
main()
{
	PutString("<-  😨  😱  😮  😲  --->");
	UserThreadCreate(aff, (void *) 4);
	UserThreadCreate(aff, (void *) 4);
	PutString("<||||>");

	Halt ();

	return 0;
}
