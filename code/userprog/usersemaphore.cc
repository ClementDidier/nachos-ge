#include "usersemaphore.h"
#include "synch.h"

/**
 * \fn 
 * \brief 
 * \param 
 * \return 
 * \exception 
*/
Semaphore * do_UserSemCreate(char * name,int nb)
{
  Semaphore * sem = new Semaphore(name, nb);
  return sem;
}

/**
 * \fn 
 * \brief 
 * \param 
 * \return 
 * \exception 
*/
void do_UserSemP(Semaphore * sem)
{
  sem->P();
}

/**
 * \fn 
 * \brief 
 * \param 
 * \return 
 * \exception 
*/
void do_UserSemV(Semaphore * sem)
{
  sem->V();
}

/**
 * \fn 
 * \brief 
 * \param 
 * \return 
 * \exception 
*/
void do_UserSemDelete(Semaphore * sem)
{
  delete sem;
}
