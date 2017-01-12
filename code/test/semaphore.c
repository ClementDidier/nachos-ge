#include "syscall.h"

sem_t * semEmpty;
sem_t * semFull;
sem_t * semMutex;
#define BUFFER_SIZE 3
char buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

void consummer(void * arg)
{
  PutString("JE SUIS CONSUMMER\n");
  for(;;)
  {
    UserSemP(semFull);
    UserSemP(semMutex);
    char nextConsummed = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    UserSemV(semMutex);
    UserSemV(semEmpty);
    char temp[6];
    temp[0] = (char) ( 48 + *(int *) arg);
    temp[1] = ' ';
    temp[2] = ':';
    temp[3] = ' ';
    temp[4] = nextConsummed;
    temp[5] = '\n';
    PutString(temp);
    int pause = 0;
    for(;pause<50000;++pause)
      ;
  }
}
void producer(void * num){
  int i = 0;
  PutString("JE SUIS PRODUCER\n");

  for(;;)
  {
    char nextProduced = (char) (i++ % 255);
    UserSemP(semEmpty);
    UserSemP(semMutex);
    buffer[in] =  nextProduced;// on load la table ascii, en boucle
    PutString("J'ai load 😌\n");
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
  PutString("Semaphore créés\n");

	UserThreadCreate(producer,  (void *) 3);
  UserThreadCreate(consummer, (void *) 1);
  UserThreadCreate(consummer, (void *) 2);

  PutString("jobs done");

//	Halt ();
	return 0;
}
