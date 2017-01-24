/**
 * \file inttest.c
 * \brief demande à l'utilisateur d'entrer un int et l'affiche, permet de tester les appels systèmes sur la console getCHarInt, GetInt et PutInt
*/
#include "syscall.h"

int main()
{
	int n;
	GetInt(&n);
	PutInt(n);

	n = GetCharInt();
	PutInt(n);
	return 0;
}