/**
 * \file putchartest.c
 * \brief Affiche les caractères de 47 à 127 sur la table ASCII grace à PutChar.
 */

#include "syscall.h"

int main()
{
	int i, g;
	for(i = 47; i < 128; i++)
	{
		PutChar((char)i);
	}

	Halt();
	return 0;
}
