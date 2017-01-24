#include "usersemaphore.h"
#include "synch.h"

Semaphore * do_UserSemCreate(char * name,int nb)
{
	Semaphore * sem = new Semaphore(name, nb);
	ASSERT(sem != NULL);
	return sem;
}

void do_UserSemP(Semaphore * sem)
{
	ASSERT(sem != NULL);
	sem->P();
}

void do_UserSemV(Semaphore * sem)
{
	ASSERT(sem != NULL);
	sem->V();
}

void do_UserSemDelete(Semaphore * sem)
{
	ASSERT(sem != NULL);
	delete sem;
}
