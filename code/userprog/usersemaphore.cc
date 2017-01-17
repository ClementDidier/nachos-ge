#include "usersemaphore.h"
#include "synch.h"

/**
 * \fn Semaphore * do_UserSemCreate(char * name,int nb)
 * \brief créé une Sémaphore et retourne le pointeur vers le sémaphore.
 * \param name : le nom du sémaphore
 * \param name : nb le nombre de jeton de départ
 * \return un pointeur vers le semaphore créé
 * \exception invalide un assert si la semaphore créée est null.
*/
Semaphore * do_UserSemCreate(char * name,int nb)
{
	Semaphore * sem = new Semaphore(name, nb);
	ASSERT(sem != NULL);
	return sem;
}

/**
 * \fn void do_UserSemP(Semaphore * sem)
 * \brief tente de prendre un jeton dans la semaphore passé en paramètre. 
 *	Si aucun jeton n'est disponible alors met le thread courrant (demandeur) en attente.
 * \param sem un pointeur vers une semaphore
 * \exception invalide un assert si la semaphore passée en paramètre est null.
*/
void do_UserSemP(Semaphore * sem)
{
	ASSERT(sem != NULL);
	sem->P();
}

/**
 * \fn void do_UserSemV(Semaphore * sem)
 * \brief "Post" un jeton dans la sémaphore passé en paramètre. 
 * \param sem un pointeur vers une semaphore
 * \exception invalide un assert si la semaphore passée en paramètre est null.
*/
void do_UserSemV(Semaphore * sem)
{
	ASSERT(sem != NULL);
	sem->V();
}

/**
 * \fn void do_UserSemDelete(Semaphore * sem)
 * \brief supprime la sémaphore passé en paramètre. 
 *	Comportement indéfini en cas de double appel de cette fonction avec une même sémaphore.
 * \param sem un pointeur vers une semaphore
 * \exception invalide un assert si la semaphore passée en paramètre est null.
*/
void do_UserSemDelete(Semaphore * sem)
{
	ASSERT(sem != NULL);
	delete sem;
}
