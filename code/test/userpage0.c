/**
 * \file userpage0.c
 * \brief Créer 3 threads, chacun fera appel a la fonction threadHandler et lui
 * passera en paramètre un caractère qu'il affichera 10 fois. On doit donc se retrouver
 * dans la console avec 10 * a, 10 * b et 10 * c répartis de manière plutôt aléatoirement
 * (selon le scheduler).
 */
#include "syscall.h"

void threadHandler(void * arg)
{
	char c = *(char *) arg;
	int i;
	for(i = 0; i <10; i++)
		PutChar(c);
}

int main()
{

	char a = 'a';
	char b = 'b';
	char c = 'c';


	UserThreadCreate(threadHandler, (void *) &a);
	UserThreadCreate(threadHandler, (void *) &b);
	UserThreadCreate(threadHandler, (void *) &c);

	//Halt();

	return 0;
}
