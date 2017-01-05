#include "syscall.h"

int main()
{
	SynchPutString("---------------------\n");
	char ubuffer[100];
	SynchGetString(ubuffer, 100);
	SynchPutString(ubuffer);
	return 0;
}