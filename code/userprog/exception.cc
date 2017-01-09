// exception.cc
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

/**
 * \file exception.cc
 * \brief Gestion des syscalls
*/
#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userthread.h"

//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void
UpdatePC ()
{
  int pc = machine->ReadRegister (PCReg);
  machine->WriteRegister (PrevPCReg, pc);
  pc = machine->ReadRegister (NextPCReg);
  machine->WriteRegister (PCReg, pc);
  pc += 4;
  machine->WriteRegister (NextPCReg, pc);
}


//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions
//      are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler (ExceptionType which)
{
  int type = machine->ReadRegister (2);

  #ifndef CHANGED

  if ((which == SyscallException) && (type == SC_Halt))
  {
    DEBUG ('a', "Shutdown, initiated by user program.\n");
    interrupt->Halt ();
  }
 else
 {
    printf ("Unexpected user mode exception %d %d\n", which, type);
    ASSERT (FALSE);
 }

  #else

  if(which == SyscallException)
  {
    switch(type)
    {
      case SC_Exit:
      {
        break;
      }
      case SC_Halt:
      {
        currentThread->space->verrou->P();
        DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
        break;
      }
      case SC_PutChar:
      {
        char arg = (char)machine->ReadRegister (4);
        DEBUG('a', "Appel systeme PutChar réalisé\n");
        synchconsole->SynchPutChar(arg);
        break;
      }
      case SC_PutString:
      {
        // lecture de l'adresse de la chaine de caractères MIPS
        int arg = machine->ReadRegister (4);

        // Buffer de la chaine de caractères LINUX (initialement vide et de taille maximale)
        char buffer[MAX_STRING_SIZE];

        // Conversion String MIPS --> String LINUX
        synchconsole->copyStringFromMachine(arg, buffer, MAX_STRING_SIZE);

        synchconsole->SynchPutString(buffer);
        DEBUG('a', "Appel systeme PutString réalisé\n");
        break;
      }
      case SC_GetChar:
      {
        machine->WriteRegister(2, (int)synchconsole->SynchGetChar());
        DEBUG('a', "Appel systeme GetChar réalisé\n");
      }
      case SC_GetString:
      {
        int result = machine->ReadRegister(4);
        int maxStringSize = machine->ReadRegister(5);
        char buffer[maxStringSize];
        synchconsole->SynchGetString(buffer, maxStringSize);
        synchconsole->copyMachineFromString(buffer, result, maxStringSize);
        machine->WriteRegister(2, result);

        DEBUG('a', "Appel systeme GetString réalisé\n");
        break;
      }
      case SC_GetInt:
      {
        int addr = machine->ReadRegister(4);
        int n;
        //synchconsole->copyMachineFromInt(&n, result);
        synchconsole->SynchGetInt(&n);
        machine->WriteMem(addr, 1, n);
        DEBUG('a', "Appel systeme GetInt réalisé\n");
        break;
      }
      case SC_PutInt:
      {
        int n = machine->ReadRegister(4);
        //synchconsole->copyIntFromMachine(arg, &n);
        synchconsole->SynchPutInt(n);
        DEBUG('a', "Appel systeme PutInt réalisé\n");
        break;
      }
      case SC_UserThreadCreate:
      {
        int f = machine->ReadRegister(4);
        int arg = machine->ReadRegister(5);
        //int retaddr = machine->ReadRegister(6)
        machine->WriteRegister(2,do_UserThreadCreate(f, arg));
        DEBUG('a', "Appel systeme SC_UserThreadCreate réalisé\n");
        break;
      }
      case SC_UserThreadExit:
      {
        machine->WriteRegister(2,do_UserThreadExit());
        DEBUG('a', "Appel systeme SC_UserThreadExit réalisé\n");
        break;
      }
      default:
      {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
        break;
      }
    }
  }
  #endif
    // LB: Do not forget to increment the pc before returning!
  UpdatePC ();
    // End of addition
}
