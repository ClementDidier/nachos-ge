/**
 * \file usertheadstes.c
 * \brief Créer 10 threads, tous les 3 thread, la boucle pour les créer attends que
 * ces threads se soient terminés pour continuer. Assert est utilisé pour verifier
 * que tous les threads sont bien créés.
 */
#include "syscall.h"

#define nbthread 50

sem_t * sem;

void aff(void * t)
{
	int tid = (int) t;
	int i = 1;
	while(i <= 10){
		UserSemP(sem);
		PutString("Thread #");
		PutInt(tid);
		PutString(" en cours d'exécution ; i = ");
		PutInt(i);
		PutChar('\n');
		UserSemV(sem);
		i++;
	}
}

int
main()
{
	sem = UserSemCreate("affichages lisibles", 1);
	int i;
	for(i = 1; i<nbthread; i++){
		Assert(UserThreadCreate(aff, (void *) i) != -1); // on sait que tid = i
		if(i % 3 == 0){
			UserThreadJoin(i-2);
			UserThreadJoin(i);
			UserThreadJoin(i-1);
			PutString("\n\nFin regoupement (désordonné) des Threads utilisateur \n\n");
		}
	}
	UserThreadJoin(i-2);
	UserThreadJoin(i);
	UserThreadJoin(i-1);
	UserSemDelete(sem);
	return 0;
}
