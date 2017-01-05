#include "syscall.h"

#define MAX_STRING_SIZE 50

int main()
{
	SynchPutString("---------------------\n> ");
	SynchPutString("---------------------\n");
	char buffer[MAX_STRING_SIZE];
	SynchGetString(buffer, MAX_STRING_SIZE);
	SynchPutString("---------------------\nResult : ");
	SynchPutString(buffer);
	SynchPutString("\n---------------------\n");
	return 0;
}