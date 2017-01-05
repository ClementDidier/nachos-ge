#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include "userThread.h"

int do_UserThreadCreate(int f, int arg){
	// on vérifie la quantité de mémoire disponible (ici ou en mode noyau ?)
	/*NoffHeader noffH;

	if(noffH.uninitData.size < UserStackSize){
		return -1;
	}*/


	// on effectue l'appel système fork

	Thread *newThread = new Thread ("Thread Noyau");

    newThread->Fork (StartUserThread, 1);

	newThread->space = currentThread->space;
	return 0;
}

static void StartUserThread(int f){

}