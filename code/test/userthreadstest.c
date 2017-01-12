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
	int i = 5;
	PutInt(i);
	while(i <= 50){
		i++;
		PutInt((int) x);
		PutChar(' ');
		PutInt(i);
		PutString("\n");
		if(i == 25){
			UserThreadJoin(2);
		}
	}
}

int
main()
{

	int tid = UserThreadCreate(aff, (void *) 1);
	int tid2 = UserThreadCreate(aff, (void *) 2);
	int tid3 =UserThreadCreate(aff, (void *) 3);
	UserThreadJoin(tid3);

	PutString("\n--- tid3 doit avoir fini maintenant--- \n");

	UserThreadJoin(tid);
	UserThreadJoin(tid2);
	UserThreadJoin(tid3);

	Halt ();

	return 0;
}
