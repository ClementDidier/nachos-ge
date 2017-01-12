#include "syscall.h"

void threadHandler(void * arg)
{
	PutString("Une chaîne de caractères affichée de façon contigüe...\n");
}

int main()
{
	int i;
	for(i = 1; i < 10; i++)
		UserThreadCreate(threadHandler, (void *) i);


	Halt();

	return 0;
}
