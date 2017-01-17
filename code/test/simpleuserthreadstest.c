/**
 * \file simpleusertheadstes.c
 * \brief Créer 3 threads, vérifie a l'aide de Assert que ces threads sont créés
 * correctement et vérifie que la variable passée en paramètre s'incrémente a chaque thread.
 */

#include "syscall.h"
void aff(void * x)
{
	int i = 1;
	while(i <= 10){
		i++;
		PutInt((int) x);
		PutString("\n");
	}
}

int
main()
{
	int i = 1;
	Assert(UserThreadCreate(aff, (void *) i++) != -1);
	Assert(UserThreadCreate(aff, (void *) i++) != -1);
	Assert(UserThreadCreate(aff, (void *) i++) != -1);
	Assert(i == 4);

	return 0;
}
