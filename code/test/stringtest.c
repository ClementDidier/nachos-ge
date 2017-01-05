#include "syscall.h"

int main()
{
	char ubuffer[5];
	SynchGetString(ubuffer, 5);
	SynchPutString(ubuffer);
	return 0;
}