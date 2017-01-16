/**
 * \file userthread.cc
 * \brief Permet la création et l'execution de thread utilisateurs
 *
*/
#include "userthread.h"
#include "system.h"
#include "addrspace.h"
#include "machine.h"
#include "syscall.h"
#include "thread.h"

/**
 * \struct userThreadParams
 * \brief Permet de passer plusieurs paramètres a StartUserThread
 * \param f Fonction que doit executer le thread utilisateurs
 * \param arg Arguments passés à cette Fonction
 * \param retaddr L'adresse de la fonction de retour pour terminer le thread
*/

struct userThreadParams
{
	int arg;
	int f;
	int retaddr;
};

/**
 * \fn static void StartUserThread(int f)
 * \brief Initialise les registres, le StackReg, le PCReg et NextPCReg ainsi que
 * le AddrSpace du thread utilisateur.
 * Acquiert le verrou ThreadJoinMutex afin de permettre aux autre thread de se mettre en attente lors d'un appel de UserThreadJoin sur ce Thread.
 * Ajoute ce Thread (pointeur vers l'objet de la classe Thread) à la liste des Thread en train d'être exécuté
 * \param f Adresse de la structure userThreadParams contenant : l'adresse de la fonction utilisateur a appeler ainsi que l'adresse de l'argument utilisateur et l'adresse de la fonction de retour.
 *
*/
static void StartUserThread(int f)
{
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	struct userThreadParams * params = (struct userThreadParams *) f;
	machine->WriteRegister (PCReg, params->f);
	machine->WriteRegister (NextPCReg, params->f + 4);
	machine->WriteRegister (4, params->arg);
	machine->WriteRegister (RetAddrReg, params->retaddr);
	int spr = machine->ReadRegister (StackReg);
	machine->WriteRegister (StackReg, spr - (UserStackSize * currentThread->mapID));
	spr = machine->ReadRegister (StackReg);
	currentThread->ThreadJoinMutex = new Lock("joinLock Thread");
    currentThread->ThreadJoinMutex->Acquire();
	currentThread->space->PushMeInThreadList();
	delete params;
	Thread::OpOnUserThreadSem->V();
	machine->Run();
}

/**
 * \fn int do_UserThreadCreate(int f, int arg)
 * \brief Gère l'initialisation et la gestion d'un thread utilisateur
 *		incrémente le nombre de thread actuellement en cours d'exécution
 *		garanti la synchronisation des accès à la bitmap de l'espace des Threads
 *		Recherche un espace assez grand pour initialiser ce thread dans pile du thread courrant
 *		créé une structure avec les informations nécessaire pour lancer un thread (adresse de retour, paramètre utilisateur et adresse de la fonction utilisateur)
 *		effectu un appel à la fonction fork (class Thread), récupère le tid (du thread créé) retourner par fork pour le retourner à l'utilisateur
 * \param f Adresse de la fonction à executer
 * \param arg Argument passé à cette fonction
 * \return Retourne le TID du thread utilisateur créé
 * \exception retourne -1 en cas d'erreur (pas assez d'espace disponible pour la création)
 *		Invalide un assert si l'on a pas pu trouver assez d'espace dans la bitmap représentant l'espace utilisé par threads.
 *
*/
int do_UserThreadCreate(int f, int arg)
{
	Thread::OpOnUserThreadSem->P();
	currentThread->space->BindUserThread();
	currentThread->space->mapLock->Acquire();
	int tid;
	Thread *newThread = new Thread ("Thread Noyau");
	newThread->mapID = currentThread->space->threadMap->Find();
	if (newThread->mapID == -1)
	{
		Thread::OpOnUserThreadSem->V();
		currentThread->space->mapLock->Release();
		currentThread->space->UnbindUserThread();
		return -1;
	}
	ASSERT(newThread->mapID >= 0);
	struct argRetparams * addret = (struct argRetparams *) arg;

	struct userThreadParams * params = new(userThreadParams);
	params->arg = addret->arg;
	params->retaddr = addret->retaddr;
	params->f = f;

	tid = newThread->Fork (StartUserThread, (int) params); // return this au lieu du tid
	currentThread->space->mapLock->Release();
	return tid;

}

/**
 * \fn int do_UserThreadExit()
 * \brief Supprime le thread courrant de la liste des thread actif. Ceci permet notament de pouvoir demander d'attendre un thread.
 *		Supprime le thread courrant de la bitmap des thread afin de libérer l'espace associé à ce thread utilisateur. Cette opération est protéger par un verrou
 *		décrémente le nombre de Thread actuel du système
 *		Libère le verrou "ThreadJoinMutex". Ceci permet à d'autre thread d'aquérire ce verrou associé à ce thread. Un autre thread qui acquiert ce verrou signifie que ce thread est terminé.
 *		Termine le thread est détruit les structures associées
 * \return ne retourne rien.
*/
void do_UserThreadExit()
{
	Thread::OpOnUserThreadSem->P();
	currentThread->space->DeleteThreadList(currentThread);
	currentThread->space->mapLock->Acquire();
	currentThread->space->threadMap->Clear(currentThread->mapID);
	currentThread->space->mapLock->Release();
	currentThread->space->UnbindUserThread();
	currentThread->space->CheckIfWaitingThread(currentThread->getTID());
	Thread::OpOnUserThreadSem->V();
	currentThread->Finish();
}


/**
 * \fn int do_UserThreadJoin(int tid)
 * \brief met le Thread courrant (appelant) en attente jusqu'a ce que le Thread passer en paramètre se termine.
 * \param tid numéro du thread utilisateur à attendre. Ce paramètre doit être > 0.
 * \return Retourne -1 en cas d'erreur (jamais atteint)
 * 		 Retourne 1 si la fonction a attendu le thread avec succès (et permet au thread applant de continuer)
 * 		 Retourne 2 lorsque le Thread passé en paramètre est introuvable ( déjà terminé dans la plupars des cas ).
 * 		 Retourne 3 lorsque le Thread passé en paramètre est égal au thread appelant.
*/
int do_UserThreadJoin(int tid)
{
	Thread::OpOnUserThreadSem->P();
	if(currentThread->space->CheckThreadList(tid) == false){
		Thread::OpOnUserThreadSem->V();
		return 2; // si le thread n'existe plus
	}
	if(tid == currentThread->getTID()){
		Thread::OpOnUserThreadSem->V();
		return 3; // si le thread est le thread courrant
	}
	// on vérifie si le numéro du thread est impossible
	if(tid < 1){
		Thread::OpOnUserThreadSem->V();
		ASSERT(false);
		return -1; // never reached
	}
	currentThread->space->Attendre(tid);
	return 1;
}
