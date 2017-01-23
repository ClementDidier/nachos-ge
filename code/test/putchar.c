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

int
main()
{
	print('a', 5);
	char c = GetChar();
	PutChar(c);
	//Halt();

	/*Not reached */
	return 0;
}