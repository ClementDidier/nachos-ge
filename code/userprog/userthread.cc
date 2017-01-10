/**
 * \file userthread.h
 * \brief Permet la création et l'execution de thread utilisateurs
 *
*/
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
	int spr = machine->ReadRegister (StackReg);
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	struct userThreadParams * params = (struct userThreadParams *) f;
	machine->WriteRegister (PCReg, params->f);
	machine->WriteRegister (NextPCReg, params->f + 4);
	machine->WriteRegister (4, params->arg);
	machine->WriteRegister (StackReg, spr - (PageSize*3));
	//machine->WriteRegister (RetAddrReg, UserThreadExit);
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
	int tid;
	if (currentThread->isStackFull())
		return -1;
	struct userThreadParams * params = new(userThreadParams);
	params->arg = arg;
	params->f = f;
	Thread *newThread = new Thread ("Thread Noyau");
	currentThread->space->BindUserThread();
	tid = newThread->Fork (StartUserThread, (int) params);
	Thread::setThreadBitmap(tid, 1);
	return tid;
}
/**
 * \fn int do_UserThreadExit()
 * \brief Stop et désalloue le thread
 * \return Retourn qqch
*/
int do_UserThreadExit()
{
	Thread::setThreadBitmap(currentThread->getTID(), 0);
	currentThread->space->UnbindUserThread();
	currentThread->space = NULL;
	currentThread->Finish();
	return 0;
}

int do_UserThreadJoin(int tid)
{
	currentThread->space->mutex->P();

	if(tid > MaxNThread || tid < 1){
		return -1;
	}

	if(Thread::getThreadBitmap(tid) == 0){
		return 1;
	}

	while(Thread::getThreadBitmap(tid) == 1){
		currentThread->space->mutex->V();
		currentThread->Yield();
		currentThread->space->mutex->P();
	}
		return 1;

	currentThread->space->mutex->V();
}