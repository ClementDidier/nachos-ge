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
#include "synch.h"
#define UserStackSize		1024	// increase this as necessary!

#ifdef CHANGED
#include "bitmap.h"
#endif
#define MaxThread 4


class Semaphore; //Declaration de l'existence de Semaphore
class Lock;
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
     * \return retourne le numéro du thread
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

    #ifdef CHANGED
    Lock* mapLock;
    BitMap* threadMap;
    #endif

  private:

      int NbActiveThreads; //Est protégé par le mutex

      int tid;

      TranslationEntry * pageTable;	// Assume linear page table translation
    // for now!
    unsigned int numPages;	// Number of pages in the virtual
    // address space
};

#endif // ADDRSPACE_H
