#include "syscall.h"

void threadHandler(void * arg)
{
	PutString("Une chaîne de caractères affichée de façon contigüe...\n");
	//PutInt(((int) arg) * 10);
	UserThreadExit();
}

int main()
{
	int i;
	for(i = 1; i < 10; i++)
		UserThreadCreate(threadHandler, (void *) i);


	while(1) { }

	return 0;
}
