/**
 * \file userthread.h
 * \brief Permet la création et l'execution de thread utilisateurs
 *
*/
#ifdef CHANGED
#include "userthread.h"
#include "system.h"
#include "addrspace.h"
#include "machine.h"
#include "syscall.h"

/**
 * \struct userThreadParams
 * \brief Permet de passer plusieurs paramètres a StartUserThread
 * \param f Fonction que doit executer le thread utilisateurs
 * \param arg Arguments passés à cette Fonction
 * \param pt L'adresse mémoire du thread
*/

struct userThreadParams
{
	int arg;
	int f;
};
/**
 * \fn static void StartUserThread(int f)
 * \brief Initialise les registres, le StackReg, le PCReg et NextPCReg ainsi que
 * le AddrSpace du thread utilisateur.
 * \param f Adresse de la fonction à executer et argument passé à cette fonction
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
	int spr = machine->ReadRegister (StackReg);
	machine->WriteRegister (StackReg, spr - (UserStackSize * currentThread->mapID));
	spr = machine->ReadRegister (StackReg);

	//machine->WriteRegister (RetAddrReg, UserThreadExit);
	currentThread->space->mapLock->Release();
	Thread::pushThreadBitmap(currentThread);
	delete params;
	machine->Run();
}

/**
 * \fn int do_UserThreadCreate(int f, int arg)
 * \brief Gère l'initialisation et la gestion d'un thread utilisateur
 * \param f Adresse de la fonction à executer
 * \param arg Argument passé à cette fonction
 * \return Retourne le PID ou -1
 *
*/
int do_UserThreadCreate(int f, int arg)
{
	currentThread->space->BindUserThread();
	currentThread->space->mapLock->Acquire();
	int tid;
	Thread *newThread = new Thread ("Thread Noyau");
	newThread->mapID = currentThread->space->threadMap->Find();

	if (newThread->mapID == -1)
	{
		currentThread->space->mapLock->Release();
		currentThread->space->UnbindUserThread();
		return -1;
	}
	ASSERT(newThread->mapID >= 0);

	struct userThreadParams * params = new(userThreadParams);
	params->arg = arg;
	params->f = f;

	tid = newThread->Fork (StartUserThread, (int) params); // return this au lieu du tid

	return tid;

}
/**
 * \fn int do_UserThreadExit()
 * \brief Stop et désalloue le thread
 * \return Retourn qqch
*/
int do_UserThreadExit()
{
	currentThread->space->mapLock->Acquire();
	Thread::deleteThreadBitmap(currentThread);
	currentThread->space->threadMap->Clear(currentThread->mapID);
	currentThread->space->mapLock->Release();
	currentThread->space->UnbindUserThread();
	//currentThread->space = NULL;
	currentThread->Finish();
	return 0;
}


int do_UserThreadJoin(int tid)
{
	currentThread->space->mapLock->Acquire();
	// on vérifie si le numéro du thread est impossible
	if(tid > MaxNThread || tid < 1 || Thread::checkThreadBitmap(tid) == false){
		currentThread->space->mapLock->Release();
		ASSERT(false);
	}

	if(Thread::checkThreadBitmap(tid) == false || tid == currentThread->getTID()){
		currentThread->space->mapLock->Release();
		return 1; // si le thread n'existe plus ou si c'est le thread 0
	}

	currentThread->space->mapLock->Release();
	Thread::attendre(tid);
	
	return 1;

	
}

#endif
