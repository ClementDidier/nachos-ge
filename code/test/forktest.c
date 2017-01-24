/**
 * \file forktest.c
 * \brief lance 10 programmes utilisateur dans de nouveaux processus.
*/
#include "syscall.h"

int main()
{
	ForkExec("./userpage0");
	ForkExec("./userpage1");
	ForkExec("./userpage0");
	ForkExec("./userpage1");
	ForkExec("./userpage0");
	ForkExec("./userpage1");
	ForkExec("./userpage0");
	ForkExec("./userpage1");
	ForkExec("./userpage0");
	ForkExec("./userpage1");	
	return 0;
}