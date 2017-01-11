// threadtest.cc 
//      Simple test case for the threads assignment.
//
//      Create two threads, and have them context switch
//      back and forth between themselves by calling Thread::Yield, 
//      to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"

struct structTestCond{
    Condition * cond;
    Lock * mutex;
    int which;
};

//----------------------------------------------------------------------
// SimpleThread
//      Loop 5 times, yielding the CPU to another ready thread 
//      each iteration.
//
//      "which" is simply a number identifying the thread, for debugging
//      purposes.
//----------------------------------------------------------------------

void
SimpleThread (int condstrct)
{
    struct structTestCond * condParams = (struct structTestCond * )condstrct;
    int num;
    int which = condParams->which;
    for (num = 0; num < 5; num++)
        {
            condParams->mutex->Acquire();
            condParams->cond->Wait(condParams->mutex);
	        printf ("*** thread %d looped %d times\n", which, num);
             currentThread->Yield();
            condParams->mutex->Release();
            condParams->cond->Broadcast(condParams->mutex);

    }
}

//----------------------------------------------------------------------
// ThreadTest
//      Set up a ping-pong between two threads, by forking a thread 
//      to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest ()
{
    struct structTestCond paramsCond1;
    struct structTestCond paramsCond2;

    Lock * condLock = new Lock("condLock");
    Condition * testCond = new Condition("testCond");

    DEBUG ('t', "Entering SimpleTest\n");

    paramsCond1.cond = testCond;
    paramsCond1.mutex = condLock;
    paramsCond1.which = 1;
    paramsCond2.cond = testCond;
    paramsCond2.mutex = condLock;
    paramsCond2.which = 2;

    Thread *t = new Thread ("forked thread");

    t->Fork (SimpleThread, (int)&paramsCond1);
    
    Thread *t2 = new Thread ("forked thread");

    t2->Fork (SimpleThread, (int)&paramsCond2);
    
    //SimpleThread (0);

    while(1){
        currentThread->Yield();
    }
}
