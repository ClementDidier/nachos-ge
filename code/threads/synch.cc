// synch.cc
//      Routines for synchronizing threads.  Three kinds of
//      synchronization routines are defined here: semaphores, locks
//      and condition variables (the implementation of the last two
//      are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.
/**
 * \file synch.cc
 * \brief implémente des structures de synchronisation (lock,condition et sémaphore)
 */
#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
//      Initialize a semaphore, so that it can be used for synchronization.
//
//      "debugName" is an arbitrary name, useful for debugging.
//      "initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

/**
 * \fn Semaphore::Semaphore (const char *debugName, int initialValue)
 * \brief Initialize a semaphore, so that it can be used for synchronization.
 * \param "debugName" is an arbitrary name, useful for debugging.
 * \param "initialValue" is the initial value of the semaphore.
*/
Semaphore::Semaphore (const char *debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
//      De-allocate semaphore, when no longer needed.  Assume no one
//      is still waiting on the semaphore!
//----------------------------------------------------------------------
/**
 * \fn Semaphore::~Semaphore ()
 * \brief De-allocate semaphore, when no longer needed.  Assume no one
 *     is still waiting on the semaphore!
*/
Semaphore::~Semaphore ()
{
    delete queue;
}

/**
 * \fn bool Semaphore::checkUnTocken()
 * \brief vérifie si il n'y a qu'un seul token dans la sémaphore.
 * \return retourne vrai si la sémaphore ne contient qu'un token, faux sinon
 * \exception
*/
bool
Semaphore::checkUnTocken(){
    IntStatus oldLevel = interrupt->SetLevel (IntOff);
    if (value > 1){
        return false;
    }
    (void) interrupt->SetLevel (oldLevel);
    return true;
}

//----------------------------------------------------------------------
// Semaphore::P
//      Wait until semaphore value > 0, then decrement.  Checking the
//      value and decrementing must be done atomically, so we
//      need to disable interrupts before checking the value.
//
//      Note that Thread::Sleep assumes that interrupts are disabled
//      when it is called.
//----------------------------------------------------------------------

/**
 * \fn void Semaphore::P ()
 * \brief Wait until semaphore value > 0, then decrement.  Checking the
      value and decrementing must be done atomically, so we
      need to disable interrupts before checking the value.

      Note that Thread::Sleep assumes that interrupts are disabled
      when it is called.
*/
void
Semaphore::P ()
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts

    while (value == 0)
      {				// semaphore not available
	  queue->Append ((void *) currentThread);	// so go to sleep
	  currentThread->Sleep ();
      }
    value--;			// semaphore available,
    // consume its value

    (void) interrupt->SetLevel (oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
//      Increment semaphore value, waking up a waiter if necessary.
//      As with P(), this operation must be atomic, so we need to disable
//      interrupts.  Scheduler::ReadyToRun() assumes that threads
//      are disabled when it is called.
//----------------------------------------------------------------------

/**
 * \fn void Semaphore::V ()
 * \brief Increment semaphore value, waking up a waiter if necessary.
      As with P(), this operation must be atomic, so we need to disable
      interrupts.  Scheduler::ReadyToRun() assumes that threads
      are disabled when it is called.
*/
void
Semaphore::V ()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel (IntOff);

    thread = (Thread *) queue->Remove ();
    if (thread != NULL)		// make thread ready, consuming the V immediately
	scheduler->ReadyToRun (thread);
    value++;
    (void) interrupt->SetLevel (oldLevel);
}

// Dummy functions -- so we can compile our later assignments
// Note -- without a correct implementation of Condition::Wait(),
// the test case in the network assignment won't work!
/**
 * \fn Lock::Lock (const char *debugName)
 * \brief Initialise une sémaphore à 1 jeton représentant un mutex simple. Le pointeur du thread appelant est initialisé à NULL
 * \param debugName un nom éventuellement utilisable pour des opération de débugguages.
*/
Lock::Lock (const char *debugName)
{
    name = debugName;
    mutex = new Semaphore("mutex lock" , 1);
    ThreadP = NULL; // There is no thread acquire Lock at start
}

/**
 * \fn Lock::~Lock ()
 * \brief détruit la sémaphore utilisé par le mutex et détruit l'objet
 *  On assume que le lock n'est plus utilisé
*/
Lock::~Lock ()
{
    delete mutex;
}

/**
 * \fn void Lock::Acquire ()
 * \brief aquiert le verrou (prend un jeron dans la semaphore)
*/
void
Lock::Acquire ()
{
    mutex->P();
    ThreadP = currentThread; // Lock is acquired by ThreadP
}

/**
 * \fn void Lock::Release ()
 * \brief libère le verrou, défini à NULL le pointeur du thread ayant acqui le verrou.
 * \exception invalide un assert si le thread qui tente de libérer le verrou (mutex) n'est pas le thread appelant.
    invalide un assert en cas de plusieurs libération (si le nombre de jeton dépasse 1 dans la sémaphore après libération).
*/
void
Lock::Release ()
{
    ASSERT(mutex->checkUnTocken());
    ASSERT(this->isHeldByCurrentThread());
    ThreadP = NULL;
    mutex->V();
}

/**
 * \fn bool Lock::isHeldByCurrentThread ()
 * \brief
 * \param
 * \return true si le thread courrant à acqui le verrou, false sinon
*/
bool
Lock::isHeldByCurrentThread ()
{
    if(ThreadP == currentThread)
    {
        return true;
    }
    return false;
}

/**
 * \fn Condition::Condition (const char *debugName).
 * \brief créé une list pour la file d'attente et initialise la condition.
 * \param debugName nom de la condition pour debug.
*/
Condition::Condition (const char *debugName)
{
    LThreads = new List;
    name = debugName;
}

/**
 * \fn Condition::~Condition ()
 * \brief détruit la liste d'attente et l'objet condition
 *  On assume que la condition n'est plus utilisé
*/
Condition::~Condition ()
{
    delete LThreads;
}

/**
 * \fn void Condition::Wait (Lock * conditionLock)
 * \brief vérifie si des threads sont en attente dans la file, si aucun n'est dans la file, alors on peut continuer, sinon on place le thread dans la file d'attente.
 *  le lock doit être acqui par le thread courrant
 * \param conditionLock lock(mutex) asscié à la condition. Doit être obtenu par le thread appelant.
 * \exception invalide un assert si conditionLock n'est pas obtenu par le thread courrant.
*/
void
Condition::Wait (Lock * conditionLock)
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);
    ASSERT(conditionLock->isHeldByCurrentThread());
    LThreads->Append(((void *) currentThread));
    conditionLock->Release();
    currentThread->Sleep();
    (void) interrupt->SetLevel (oldLevel);
    conditionLock->Acquire();
}

/**
 * \fn void Condition::Signal (Lock * conditionLock)
 * \brief Libère le lock conditionLock et réveil le premier Thread (instance de Thread) de la file d'attente (list LThreads)
 * \param conditionLock lock(mutex) asscié à la condition. Doit être obtenu par le thread appelant.
 * \exception invalide un assert si conditionLock n'est pas obtenu par le thread courrant.
*/
void
Condition::Signal (Lock * conditionLock)
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);
    if(LThreads->IsEmpty () == false)
    {
        scheduler->ReadyToRun ((Thread *)LThreads->Remove());
    }
    (void) interrupt->SetLevel (oldLevel);
}

/**
 * \fn void Condition::Broadcast (Lock * conditionLock)
 * \brief Libère le lock conditionLock et réveil tout les Thread (instance de Thread) mis en attente dans la condition
 * \param conditionLock lock(mutex) asscié à la condition. Doit être obtenu par le thread appelant.
 * \exception invalide un assert si conditionLock n'est pas obtenu par le thread courrant.
*/
void
Condition::Broadcast (Lock * conditionLock)
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);
    while(LThreads->IsEmpty () == false)
    {
        scheduler->ReadyToRun ((Thread *) LThreads->Remove());
    }

    (void) interrupt->SetLevel (oldLevel);
}
