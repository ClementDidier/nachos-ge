// addrspace.h
//      Data structures to keep track of executing user programs
//      (address spaces).
//
//      For now, we don't keep any information about address spaces.
//      The user level CPU state is saved and restored in the thread
//      executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.
/**
 * \file addrspace.h
 * \brief Structures de données pour l'espace d'addressage utilisateur
 */

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "list.h"
#include "synch.h"
#include "thread.h"

#define UserStackSize		1024	// increase this as necessary!

#define noThreadJoinFound 1
#define noMutexJoinFound 2
#define successWait 3

#define structCompteurNotFound 1;
#define structCompteurDeleted 2;
#define structCompteurInUse 3;

#include "bitmap.h"
#define MaxThread 5

class Lock;
struct compteurVerrou{
  int tid;
  int compteur;
  Lock * mutexJoin;
};


class Semaphore; //Declaration de l'existence de Semaphore
class Thread;
class AddrSpace
{
  public:
    AddrSpace (OpenFile * executable);	// Create an address space,
    // initializing it with the program
    // stored in the file "executable"
    ~AddrSpace ();		// De-allocate an address space

    void InitRegisters ();	// Initialize user-level CPU registers,
    // before jumping to user code

    void SaveState ();		// Save/restore address space-specific
    void RestoreState ();	// info on a context switch
    /**
     * \fn void BindUserThread()
     * \brief Lie un thread à cette AddrSpace
     *
    */
    void BindUserThread();
    /**
     * \fn void AddrSpace::UnbindUserThread()
     * \brief Délie un thread de cet AddrSpace
     *
    */
    void UnbindUserThread();

    Semaphore *mutex; // Ne dois JAMAIS depasser 1 token;
    Semaphore *verrou;
    Thread * ThreadList[MaxThread];
    struct compteurVerrou * GCThreadVerrou[MaxThread]; // on ne connait pas le nombre de thread supprimé qui ont des verroux en attente...
    Lock * GCThreadVerrouLock;
    Lock * mapLock;
    BitMap* threadMap;


    void pushMeInThreadList();
    bool checkThreadList(int tid);
    Thread * findThreadList(int tid);
    void deleteThreadList(Thread * ThreadP);
    int attendre(int tid);
    struct compteurVerrou * findCompteurVerrou(int tid);
    Lock * LockThreadList;
    int checkIfWaitingThread (int tid);
    int AddInGCThreadVerrou(struct compteurVerrou *);
    void deleteInGC(struct compteurVerrou * compteur);

  private:

      int NbActiveThreads; //Est protégé par le mutex

      TranslationEntry * pageTable;	// Assume linear page table translation
    // for now!
    unsigned int numPages;	// Number of pages in the virtual
    // address space
};

#endif // ADDRSPACE_H
