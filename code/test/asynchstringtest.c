/**
 * \file asynchstringtest.c
 * \brief lance 10 thread utilisateur qui affiche une première ou une seconde chaine de caractère de façon contigüe (pas d'entrelacement)
 */
#include "syscall.h"


void threadHandler1(void * arg)
{
	PutString("Une première chaîne de caractères affichée de façon contigüe...\n");
}

void threadHandler2(void * arg)
{
	PutString("Une deuxième chaîne de caractères affichée de façon contigüe...\n");
}

int main()
{
	int i;
	for(i = 1; i < 5; i++)
	{
		UserThreadCreate(threadHandler1, (void *) i);
		UserThreadCreate(threadHandler2, (void *) (i*2));
	}

	return 0;
}
