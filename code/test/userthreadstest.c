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
		PutInt(*(int *) x);
		PutChar(' ');
		PutInt(i);
		PutString("\n");
		if(i == 25){
			UserThreadJoin(1);
		}
	}
}

int
main()
{

	PutString("<---->");
	int tid = UserThreadCreate(aff, (void *) 1);
	PutString("<---->");
	PutInt(tid);
	PutString("<---->");

	int tid2 = UserThreadCreate(aff, (void *) 2);
	PutString("\nTID du Thread Créé :");
	PutInt(tid);
	PutString("\n");

//UserThreadJoin(tid);
	UserThreadJoin(tid2);

	PutString("\n");
	PutString("\n");
	PutString("\n");

	Halt ();

	return 0;
}
