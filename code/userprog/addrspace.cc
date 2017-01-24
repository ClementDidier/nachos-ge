// addrspace.cc
//      Routines to manage address spaces (executing user programs).
//
//      In order to run a user program, you must:
//
//      1. link with the -N -T 0 option
//      2. run coff2noff to convert the object file to Nachos format
//              (Nachos object code format is essentially just a simpler
//              version of the UNIX executable object code format)
//      3. load the NOFF file into the Nachos file system
//              (if you haven't implemented the file system yet, you
//              don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"

//#include <strings.h>		/* for bzero */

#include "bitmap.h"

//----------------------------------------------------------------------
// SwapHeader
//      Do little endian to big endian conversion on the bytes in the
//      object file header, in case the file was generated on a little
//      endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader (NoffHeader * noffH)
{
    noffH->noffMagic = WordToHost (noffH->noffMagic);
    noffH->code.size = WordToHost (noffH->code.size);
    noffH->code.virtualAddr = WordToHost (noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost (noffH->code.inFileAddr);
    noffH->initData.size = WordToHost (noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost (noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost (noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost (noffH->uninitData.size);
    noffH->uninitData.virtualAddr =
	WordToHost (noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost (noffH->uninitData.inFileAddr);
}

/**
 * \fn static void ReadAtVirtual(OpenFile *executable, int virtualaddr, int numBytes,
 *  int position, TranslationEntry *pageTable, unsigned numPages)
 * \brief Effectu une lecture du fichier donné et écrit en mémoire la portion voulue
 * \param executable Le fichier contenant le code du processus à mettre en mémoire
 * \param virtualaddr L'adresse virtuelle de stockage des données
 * \param numBytes La taille des données à stocker en mémoire
 * \param position La position (offset) dans la page de stockage
 * \param pageTable La table des pages mémoire
 * \param numPages La taille de la table des pages
 *
*/
static void ReadAtVirtual(OpenFile *executable, int virtualaddr, int numBytes,
    int position, TranslationEntry *pageTable, unsigned numPages)//numPages = nb pages dans la table
{
  TranslationEntry * oldEntry = machine->pageTable;
  unsigned oldNumPages = numPages;

  machine->pageTable = pageTable;
  machine->pageTableSize = numPages;

  char buffer[numBytes];
  executable->ReadAt(buffer, numBytes, position);

  int i;
  for(i = 0; i < numBytes; i++)
  {
    int g = (int)buffer[i];
    machine->WriteMem(virtualaddr + i, 1, g);
  }

  machine->pageTable = oldEntry;
  machine->pageTableSize = oldNumPages;
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
//      Create an address space to run a user program.
//      Load the program from a file "executable", and set everything
//      up so that we can start executing user instructions.
//
//      Assumes that the object code file is in NOFF format.
//
//      First, set up the translation from program memory to physical
//      memory.  For now, this is really simple (1:1), since we are
//      only uniprogramming, and we have a single unsegmented page table
//
//      "executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace (OpenFile * executable)
{
    NoffHeader noffH;
    unsigned int i, size;

    executable->ReadAt ((char *) &noffH, sizeof (noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
	(WordToHost (noffH.noffMagic) == NOFFMAGIC))
	SwapHeader (&noffH);
    ASSERT (noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize * MaxThread;	// we need to increase the size


    // to leave room for the stack
    numPages = divRoundUp (size, PageSize);
    size = numPages * PageSize;

    printf("DEMANDE : %d pages\n", numPages);

    ASSERT (numPages <= NumPhysPages);	// check we're not trying
    // to run anything too big --
    // at least until we have
    // virtual memory

    DEBUG ('a', "Initializing address space, num pages %d, size %d\n",
	   numPages, size);
// first, set up the translation
    pageTable = new TranslationEntry[numPages];

    ASSERT(frameProvider->NumAvailFrame() >= 0);
    if((unsigned)frameProvider->NumAvailFrame() < numPages)
    {
      printf("NumAvailFrame : %d   numPages : %d\n", frameProvider->NumAvailFrame(), numPages);
      DEBUG('a', "Nombre de pages physiques insuffisant\n");
      ASSERT(FALSE);
    }

    for (i = 0; i < numPages; i++)
      {
	  pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
	  pageTable[i].physicalPage = frameProvider->GetEmptyFrame();
	  pageTable[i].valid = TRUE;
	  pageTable[i].use = FALSE;
	  pageTable[i].dirty = FALSE;
	  pageTable[i].readOnly = FALSE;	// if the code segment was entirely on
	  // a separate page, we could set its
	  // pages to be read-only
      }

// zero out the entire address space, to zero the unitialized data segment
// and the stack segment
    //bzero (machine->mainMemory, size);

// then, copy in the code and data segments into memory
    if (noffH.code.size > 0)
      {
	  DEBUG ('a', "Initializing code segment, at 0x%x, size %d\n",
		 noffH.code.virtualAddr, noffH.code.size);
	  /*executable->ReadAt (&(machine->mainMemory[noffH.code.virtualAddr]),
			      noffH.code.size, noffH.code.inFileAddr);*/

        ReadAtVirtual(executable, noffH.code.virtualAddr, noffH.code.size,
          noffH.code.inFileAddr, pageTable, numPages);
      }
    if (noffH.initData.size > 0)
      {
	  DEBUG ('a', "Initializing data segment, at 0x%x, size %d\n",
		 noffH.initData.virtualAddr, noffH.initData.size);
	  /*executable->ReadAt (&
			      (machine->mainMemory
			       [noffH.initData.virtualAddr]),
			      noffH.initData.size, noffH.initData.inFileAddr);*/

        ReadAtVirtual(executable, noffH.initData.virtualAddr, noffH.initData.size,
          noffH.initData.inFileAddr, pageTable, numPages);
      }

      verrou = new Semaphore("verrouHalt", 1);
      mutex = new Semaphore("mutexNbActiveThread", 1);
      GCThreadVerrouLock = new Lock("Verrou pour la liste des verrous");
      NbActiveThreads = 1;
      mapLock = NULL;
      mapLock = new Lock("mapLock");
      threadMap = new BitMap(MaxThread);
      threadMap->Mark(0);
      int j;
      for (j = 0;j<MaxThread;++j){
        ThreadList[j] = NULL; // on initialize la bitmap des thread id à NULL
      }
      for (j = 0;j<MaxThread * 2;++j){
        GCThreadVerrou[j] = NULL;
      }

      LockThreadList = new Lock("LockThreadList");
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//      Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace ()
{
  // LB: Missing [] for delete
  // delete pageTable;
  delete [] pageTable;
  delete threadMap;
  delete mapLock;
  delete GCThreadVerrouLock;
  delete verrou;
  delete mutex;
  delete LockThreadList;

  // End of modification
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
//      Set the initial values for the user-level register set.
//
//      We write these directly into the "machine" registers, so
//      that we can immediately jump to user code.  Note that these
//      will be saved/restored into the currentThread->userRegisters
//      when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters ()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister (i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister (PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister (NextPCReg, 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    machine->WriteRegister (StackReg, numPages * PageSize - 16);
    DEBUG ('a', "Initializing stack register to %d\n",
	   numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
//      On a context switch, save any machine state, specific
//      to this address space, that needs saving.
//
//      For now, nothing!
//----------------------------------------------------------------------

void
AddrSpace::SaveState ()
{
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
//      On a context switch, restore the machine state so that
//      this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void
AddrSpace::RestoreState ()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

void
AddrSpace::BindUserThread()
{
  mutex->P();
  if (NbActiveThreads++ == 1){
    verrou->P();
  }
  mutex->V();

}

void
AddrSpace::UnbindUserThread()
{
  mutex->P();
  if (--NbActiveThreads <= 1){
    verrou->V();
  }
  mutex->V();

}

// ajoute le thread "currentThread" dans le tableau des thread
void
AddrSpace::PushMeInThreadList(){
  LockThreadList->Acquire();
  int i = 0;

  // recherche d'un emplacement...
  while (ThreadList[i] != NULL && i < MaxThread ){
    i++;
  }
  if(i >= MaxThread){
    LockThreadList->Release();
    ASSERT(false);
  }

  ThreadList[i] = currentThread;
  LockThreadList->Release();

  // initialisation du "Garbage Collector" des verrou de join inutilisé

  struct compteurVerrou * newCompteur = new struct compteurVerrou;

  newCompteur->tid = currentThread->getTID();
  newCompteur->compteur = 1;
  newCompteur->mutexJoin = currentThread->ThreadJoinMutex;

  AddInGCThreadVerrou(newCompteur);
}

int
AddrSpace::AddInGCThreadVerrou(struct compteurVerrou * compteur)
{
  GCThreadVerrouLock->Acquire();
  int i;
  for(i = 0; i< MaxThread * 2; i++){
    if(GCThreadVerrou[i] == NULL){
      GCThreadVerrou[i] = compteur;
      GCThreadVerrouLock->Release();
      return 1;
    }
  }
  GCThreadVerrouLock->Release();
  ASSERT(false);
  return -1;
}

// vérifie si le thread #id est dans le tableau
bool
AddrSpace::CheckThreadList(int tid)
{
  LockThreadList->Acquire();
  int i = 0;
  bool trouve = false;
  while (trouve == false && i < MaxThread){
    if(ThreadList[i] != NULL){
      if(ThreadList[i]->getTID() == tid){
       trouve = true;
      }
    }
    i++;
  }
  LockThreadList->Release();
  return trouve;
}

// vérifie si le thread #id est dans le tableau
Thread *
AddrSpace::FindThreadList(int tid)
{
  LockThreadList->Acquire();
  int i;
  for(i = 0; i < MaxThread; ++i)
  {
    if(ThreadList[i] != NULL){
      if(ThreadList[i]->getTID() == tid){
        LockThreadList->Release();
        return ThreadList[i];
      }
    }
  }
  LockThreadList->Release();
  return NULL;
}

void
AddrSpace::DeleteThreadList(Thread * ThreadP)
{
  LockThreadList->Acquire();
  int i = 0;

  if(ThreadP == NULL)
  {
    LockThreadList->Release();
    ASSERT(false);
  }

  while (ThreadList[i] != ThreadP && i < MaxThread){
    i++;
  }

  if(ThreadList[i] == ThreadP)
  {
    ThreadList[i] = NULL;
    LockThreadList->Release();
  }
  else
  {
    LockThreadList->Release();
    ASSERT(false);
  }
}

struct compteurVerrou *
AddrSpace::FindCompteurVerrou(int tid)
{
  int i;
  for (i = 0; i< MaxThread * 2; i++){
    if( GCThreadVerrou[i] != NULL && GCThreadVerrou[i]->tid == tid ){
      return GCThreadVerrou[i];
    }
  }
  return NULL;
}

int
AddrSpace::Attendre(int tid)
{
  Thread * ThreadToJoin = FindThreadList(tid);
  Lock * mutexJoin = ThreadToJoin->ThreadJoinMutex;

  if (ThreadToJoin == NULL){
    Thread::OpOnUserThreadSem->V();
    return noThreadJoinFound;
  }

  if (mutexJoin == NULL){
    Thread::OpOnUserThreadSem->V();
    return noMutexJoinFound;
  }

  // dans les 2 cas précédent nous n'avons pas eu besoin de prendre le mutex !

  GCThreadVerrouLock->Acquire();
  struct compteurVerrou * structCompteur = FindCompteurVerrou(tid);

  // aucune structure donc le thread n'existe plus et a était nettoyer
  if(structCompteur == NULL){
    GCThreadVerrouLock->Release();
    Thread::OpOnUserThreadSem->V();
    return structCompteurWaseDeleted;
  }

  structCompteur->compteur = structCompteur->compteur + 1; // on s'ajoute en temps que thread attendant

  GCThreadVerrouLock->Release();

  Thread::OpOnUserThreadSem->V();

  structCompteur->mutexJoin->Acquire();

  GCThreadVerrouLock->Acquire();

  ASSERT(structCompteur != NULL);

  structCompteur->compteur = structCompteur->compteur - 1;

  GCThreadVerrouLock->Release();

  structCompteur->mutexJoin->Release();

  GarbageCollector(); // on appel le garbage collector pour détruire les structures inutilisées.

  return successWait;
}

// assume mutexJoin->Acquire()
int
AddrSpace::CheckIfWaitingThread(int tid)
{
  GCThreadVerrouLock->Acquire();
  struct compteurVerrou * structCompteur = FindCompteurVerrou(tid);

  if(structCompteur == NULL){
    return structCompteurNotFound;
  }

  structCompteur->compteur = structCompteur->compteur - 1;
  structCompteur->mutexJoin->Release();
  GCThreadVerrouLock->Release();


  GarbageCollector();
  return structCompteurInUse;
}

void
AddrSpace::GarbageCollector()
{
  int i;
  GCThreadVerrouLock->Acquire();
  for(i = 0; i<MaxThread * 2; i++){
    if(GCThreadVerrou[i] != NULL && GCThreadVerrou[i]->compteur == 0){
      delete GCThreadVerrou[i]->mutexJoin;
      delete GCThreadVerrou[i];
      GCThreadVerrou[i] = NULL;
    }
  }
  GCThreadVerrouLock->Release();
}
