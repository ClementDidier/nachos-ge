#include "usersemaphore.h"
#include "synch.h"

Semaphore * do_UserSemCreate(char * name,int nb)
{
  Semaphore * sem = new Semaphore(name, nb);
  return sem;
}
void do_UserSemP(Semaphore * sem)
{
  sem->P();
}
void do_UserSemV(Semaphore * sem)
{
  sem->V();
}
void do_UserSemDelete(Semaphore * sem)
{
  delete sem;
}
