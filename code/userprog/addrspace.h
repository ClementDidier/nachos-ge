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
#include "machine.h"

#define UserStackSize		1024	// increase this as necessary!

#define noThreadJoinFound 1
#define noMutexJoinFound 2
#define successWait 3
#define structCompteurWaseDeleted 4

#define structCompteurNotFound 1;
#define structCompteurInUse 2;

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
     * \fn void AddrSpace::BindUserThread()
     * \brief ajoute un thread sur le compteur des thread actif et verouille le lock "verrou" utilisé pour l'attente lors du halt.
    */
    void BindUserThread();

    /**
     * \fn void AddrSpace::UnbindUserThread()
     * \brief retire un thread sur le compteur des thread actif et déverouille le lock "verrou" utilisé pour l'attente lors du halt.
    */
    void UnbindUserThread();

    Semaphore *mutex; // Ne dois JAMAIS depasser 1 token;
    Semaphore *verrou;
    Thread * ThreadList[MaxThread];
    struct compteurVerrou * GCThreadVerrou[MaxThread*2]; // il y a au maximum Maxthread et chaque thread attend au maximum 1 autre thread...
    Lock * GCThreadVerrouLock;
    Lock * mapLock;
    BitMap* threadMap;

    /**
     * \fn void AddrSpace::PushMeInThreadList()
     * \brief ajoute le pointeur vers l'objet thread du thread courrant dans le tableau des threads actifs. Les opération sur les listes sont effectué de manière protégées.
     * \exception invalide un assert si aucun emplacement libre n'existe dans le tableau.
     *  Ceci signifie que l'on a dépassé le nombre de thread autorisé à s'éxécuter sur un thread noyau.
    */
    void PushMeInThreadList();

    /**
     * \fn void AddrSpace::checkThreadList(int tid)
     * \brief vérifie dans le tableau des thread actif si un thread a le tid passé en paramètre.
     * \param tid : un numéro (int) the thread utilisateur
     * \return retourne vrai si on trouve un thread actif ayant le tid passé en paramètre, faux sinon.
    */
    bool CheckThreadList(int tid);

    /**
     * \fn void AddrSpace::FindThreadList(int tid)
     * \brief recherche dans le tableau des thread actif le thread ayant le tid passé en paramètre.
     * \param tid : un numéro (int) the thread utilisateur
     * \return retourne un pointeur vers l'objet thread associé au thread ayant l'id passé en paramètre (tid).
     *  Si aucun thread actif n'a ce tid alors la fonction retourne NULL
    */
    Thread * FindThreadList(int tid);

    /**
     * \fn void AddrSpace::deleteThreadList(Thread * ThreadP)
     * \brief Supprime le pointeur du thread pointé par threadP de le tableau des threads actif (le tableau ThreadList).
     * \param ThreadP un pointeur vers un objet thread. ThreadP ne doit pas être null;
     * \exception invalide in assert si threadP est NULL ou si on n'a pas trouvé threadP dans le tableau ThreadList (thread actif).
    */
    void DeleteThreadList(Thread * ThreadP);

    /**
     * \fn int AddrSpace::attendre(int tid)
     * \brief attend la fin du thread ayant le tid passé en paramètre.
     *  Si un thread utilisateur possède ce tid alors on attend la fin de celui ci en demandant son verrou threadjoin.
     *  si aucun thread actif (ie actuellement en cours d'exécution) n'est trouvé alors on termine et return immédiatement.
     * \param tid : un numéro (int) the thread utilisateur
     * \return noThreadJoinFound si aucun thread n'a été trouvé,
     *  noMutexJoinFound si le verrou de join à déjà été libéré (on peut continuer,
     *  le thread ne devrait plus exister) ou successWait après une attente réussie.
     * \exception invalide un assert si la structure structCompteur a été supprimé,
     *  ayant incrémenter le compteur, le garbage collector ne devrait pas la supprimé car le compteur > 0.
    */
    int Attendre(int tid);

    /**
     * \fn struct compteurVerrou * AddrSpace::findCompteurVerrou(int tid)
     * \brief cherche dans le tableau GCThreadVerrou si une entrée possède le tid passé en paramètre
     *  assume GCThreadVerrouLock->Acquire() avant l'appel...
     * \param tid : un numéro (int) the thread utilisateur
     * \return un pointeur vers la structure compteurVerrou associé au thread utilisateur ayant ou ayant eu (= si le thread est déjà terminé) le tid passé en paramètre.
     *  Si aucune structure n'est trouvé retourne NULL.
    */
    struct compteurVerrou * FindCompteurVerrou(int tid);

    Lock * LockThreadList;

    /**
     * \fn int AddrSpace::checkIfWaitingThread(int tid)
     * \brief Assume que le verrou ThreadJoinMutex de l'addrspace ( ici défini par mutexJoin ) est acqui.
     *  recherche une structure compteurVerrou dans la liste GCThreadVerrou. Si celle si existe alors elle est détruit si aucun thread n'attend la terminaison du thread courrant.
     *  Libère le verrou ThreadJoinMutex.
     * \param tid : un numéro (int) the thread utilisateur
     * \return un entier associé à l'opération effectuée (structCompteurNotFound, structCompteurInUse)
     * \exception Si ThreadJoinMutex n'a pas été verouillé alors une erreur sera provoqué lors du Release.
    */
    int CheckIfWaitingThread (int tid);
    /**
     * \fn void AddrSpace::AddInGCThreadVerrou(struct compteurVerrou *)
     * \brief ajoute le pointeur vers la structure passé en paramètre vers un espace libre du tableau du garbage collector. 
     *  Permettant de la supprimer lorsque plus aucun thread n'attends le verrou
     * \return 1 si on a pu ajouter la structure dans la tableau GCThreadVerrou, -1 sinon
     * \exception invalide in assert on a pas trouvé de place de libre dans le tableau.
    */
    int AddInGCThreadVerrou(struct compteurVerrou *);

    /**
     * \fn int AddrSpace::GarbageCollector()
     * \brief parcourt le tableau GCThreadVerrou à la recherche de structure de compteur avec un compteur de thread à 0.
     *  Supprime les struture trouvées ainsi que les mutex utilisés pour pour se mettre en attente sur le thread.
     *  Met à NULL les cases du tableau concernées une fois fini pour réutilisation.
     *  Cette opération sur le tableau GCThreadVerrou est protégé par le verrou GCThreadVerrouLock.
    */
    void GarbageCollector();

  private:

      int NbActiveThreads; //Est protégé par le mutex

      TranslationEntry * pageTable;	// Assume linear page table translation
    // for now!
    unsigned int numPages;	// Number of pages in the virtual
    // address space
};

#endif // ADDRSPACE_H
