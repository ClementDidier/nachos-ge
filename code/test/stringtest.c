#include "syscall.h"

int main()
{
	char ubuffer[5];
	GetString(ubuffer, 5);
	PutString(ubuffer);
	return 0;
}