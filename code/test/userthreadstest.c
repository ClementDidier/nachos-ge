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
		PutInt(i);
		PutString("\n");
	}
}

int
main()
{
	//PutString("<---->");
	int tid = UserThreadCreate(aff, (void *) 4);
	int tid2 = UserThreadCreate(aff, (void *) 4);
	PutString("\nTID du Thread Créé :");
	PutInt(tid);
	PutString("\n");

	UserThreadJoin(tid);
	UserThreadJoin(tid2);

	PutString("\n");
	PutString("\n");
	PutString("\n");

	Halt ();

	return 0;
}
