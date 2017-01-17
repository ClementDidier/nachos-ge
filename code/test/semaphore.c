/**
 * \file semaphore.c
 * \brief Situation de producteur/consomateur. On créer 2 consomateur qui affichent
 * le contenu d'un même buffer et un producteur qui rempli ce buffer avec des
 * caractères de la table ASCII. Le buffer est protégé avec des sémaphores, de
 * la même façon que la situation de producteur/consomateur vu en cours.
 */

#include "syscall.h"

sem_t * semEmpty;
sem_t * semFull;
sem_t * semMutex;
#define BUFFER_SIZE 10
char buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

void consummer(void * arg)
{
  PutString("JE SUIS CONSUMMER ET LE THREAD NUMERO ");
  PutInt(*(int *)arg);
  PutChar('\n');

  for(;;)
  {
    UserSemP(semFull);
    UserSemP(semMutex);
    char nextConsummed = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    UserSemV(semMutex);
    UserSemV(semEmpty);
    char buf[6];
    buf[0] = (char) ( 48 + *(int *) arg);
    buf[1] = ' ';
    buf[2] = ':';
    buf[3] = ' ';
    buf[4] = nextConsummed;
    buf[5] = '\n';

    PutString(buf);
    int pause = 0;
    for(;pause<50000;++pause)
      ;
  }
}
void producer(void * arg){
  int i = 0;
  PutString("JE SUIS PRODUCER ET LE THREAD NUMERO ");
  PutInt(*(int *)arg);
  PutChar('\n');

  for(;;)
  {
    char nextProduced = (char) (i++ % 255);
    UserSemP(semEmpty);
    UserSemP(semMutex);
    buffer[in] =  nextProduced;// on load la table ascii, en boucle
    PutString("J'ai load un caractère 😌\n");
    in = (in + 1) % BUFFER_SIZE;
    UserSemV(semMutex);
    UserSemV(semFull);
    int pause = 0;
    for(;pause<10000;++pause)
      ;
  }
}

int
main()
{
  semMutex = UserSemCreate("Mutex",1);
  semFull =  UserSemCreate("Full",0);
  semEmpty = UserSemCreate("Empty",1);
  Assert (semMutex && semFull && semEmpty);
	UserThreadCreate(producer,  (void *) 3);
  UserThreadCreate(consummer, (void *) 1);
  UserThreadCreate(consummer, (void *) 2);

  PutString("Ici le thread main, j'ai fini mon travail et suis prêt à Exit");

//	Halt ();
	return 0;
}
