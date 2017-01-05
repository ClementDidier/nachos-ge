#include "syscall.h"

void plus1(void * x)
{
	// do stuff
}

int
main()
{
	UserThreadCreate(plus1, (void *) 4);
	return 0;
}