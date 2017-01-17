/**
 * \file semaphore.c
 * \brief Créer 10 threadsqui doivent afficher des chaines de caractères différentes.
 * Il est possible que tous les threads ne soient pas créés selon le nb max de threads
 * pouvant etre créés
 */
#include "syscall.h"


threadHandler1(void * arg)
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
		UserThreadCreate(threadHandler2, (void *) i*2);
	}

	return 0;
}
