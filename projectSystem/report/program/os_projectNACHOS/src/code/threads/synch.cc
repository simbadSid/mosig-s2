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

#include "copyright.h"
#include "synch.h"
#include "system.h"
#include "list.h"
//+ goubetc 12.01.16 13.01.16 18.01.16
//+ FoxTox 13.01.2015

//----------------------------------------------------------------------
// Semaphore::Semaphore
//      Initialize a semaphore, so that it can be used for synchronization.
//
//      "debugName" is an arbitrary name, useful for debugging.
//      "initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

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

Semaphore::~Semaphore ()
{
    delete queue;
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

void
Semaphore::P ()
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts
    while (value == 0)
	{				// semaphore not available
	    DEBUG('s', "Semaphore: Semaphore value null, setting thread to sleep: name = \"%s\", tid = %d value:%d.\n", currentThread->getName(), currentThread->getTID(), value); //+ goubetc 13.01.16
	    queue->Append ((void *) currentThread);	// so go to sleep
	    currentThread->Sleep ();
	}
    value--;			// semaphore available,
    DEBUG('s', "Semaphore: Semaphore value-- by name = \"%s\", tid = %d.\n", currentThread->getName()); //+ goubetc 13.01.16
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

void
Semaphore::V ()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel (IntOff);

    thread = (Thread *) queue->Remove ();
    if (thread != NULL){		// make thread ready, consuming the V immediately
	scheduler->ReadyToRun (thread);
	DEBUG('s', "Semaphore: Semaphore value++ (%d), waking up thread: name = \"%s\", tid = %d. \n", (value+1), thread->getName(), thread->getTID(), currentThread->getTID()); //+ goubetc 13.01.16
    } else {
	DEBUG('s', "Semaphore: Semaphore value++ by name = \"%s\", tid = %d.\n", currentThread->getName()); //+ goubetc 13.01.16
    }
    value++;
    (void) interrupt->SetLevel (oldLevel);
}

//+b goubetc 11.01.16
//----------------------------------------------------------------------
// Semaphore::P2
//      Waits until semaphore value == 0.  
//      Note that Thread::Sleep assumes that interrupts are disabled
//      when it is called.
//----------------------------------------------------------------------

// void
// Semaphore::P_Count ()
// {
//     IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts
//     value--;
//     while (value > 0)
//       {				// semaphore not available
// 	  queue->Append ((void *) currentThread);	// so go to sleep
// 	  currentThread->Sleep ();
//       }

//     (void) interrupt->SetLevel (oldLevel);	// re-enable interrupts
// }


// //----------------------------------------------------------------------
// // Semaphore::P_Count
// //      Increments semaphore value.  
// //----------------------------------------------------------------------

// void
// Semaphore::Count ()
// {
//     IntStatus oldLevel = interrupt->SetLevel (IntOff);
//     value++;
//     (void) interrupt->SetLevel (oldLevel);
// }


// //----------------------------------------------------------------------
// // Semaphore::V2
// //      Decrement semaphore value
// //      As with P(), this operation must be atomic, so we need to disable
// //      interrupts.  Scheduler::ReadyToRun() assumes that threads
// //      are disabled when it is called.
// //----------------------------------------------------------------------

// void
// Semaphore::V_Count ()
// {
//     Thread *thread;
//     IntStatus oldLevel = interrupt->SetLevel (IntOff);

//     thread = (Thread *) queue->Remove ();
//     if (thread != NULL)		// make thread ready, consuming the V immediately
// 	scheduler->ReadyToRun (thread);
//     value--;
//     (void) interrupt->SetLevel (oldLevel);
// }

//+e goubetc 11.01.16

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock (const char *debugName)
{
    name = debugName;
    busy = false;
    queue = new List;
}

Lock::~Lock ()
{
    delete queue;
}
void
Lock::Acquire ()
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts
    while (busy)
    	{				// semaphore not available
	    DEBUG('s', "Lock name(%s): Lock busy, setting thread to sleep: name = \"%s\", tid = %d.\n",name, currentThread->getName(), currentThread->getTID()); //+ goubetc 13.01.16
	    queue->Append ((void *) currentThread);	// so go to sleep
	    currentThread->Sleep ();
	}
    busy = true;			// semaphore available, 
    // consume its value

    (void) interrupt->SetLevel (oldLevel);	// re-enable interrupts

}
void
Lock::Release ()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel (IntOff);

    thread = (Thread *) queue->Remove ();
    if (thread != NULL){		// make thread ready, consuming the V immediately
	scheduler->ReadyToRun (thread);
	DEBUG('s', "Lock name(%s): Lock free, waking up thread: name = \"%s\", tid = %d.\n :: current thread: %d", name, thread->getName(), thread->getTID(), currentThread->getTID()); //+ goubetc 13.01.16
    }
    busy = false;
    (void) interrupt->SetLevel (oldLevel);
}
 
// Lock::Lock (const char *debugName)
// {
//     name = debugName;
//     sem = new Semaphore(debugName,1);
// }

// Lock::~Lock ()
// {
//     //delete queue;
// }
// void
// Lock::Acquire ()
// {
//     sem->P();
// }
// void
// Lock::Release ()
// {
//     sem->V();
// }


//+b goubetc 18.01.16
Condition::Condition (const char *debugName)
{
    name = debugName;
    sleeping = new List();
}

Condition::~Condition ()
{
}

void
Condition::Wait (Lock * conditionLock)
{
    conditionLock->Release();
    Semaphore *thread_Lock = new Semaphore("sleeping", 0);
    sleeping->Append((void *) thread_Lock);
    thread_Lock->P();
    conditionLock->Acquire();
}

void
Condition::Signal (Lock * conditionLock)
{
    if (!sleeping->IsEmpty()){
	Semaphore* temp = (Semaphore *) sleeping->Remove();
	temp->V();
    }
}

void
Condition::Broadcast (Lock * conditionLock)
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);
    DEBUG('s', "Condition(%s): Semaphore value++ by name = \"%s\", tid = %d.\n",
    		conditionLock->getName(), currentThread->getName()); //+ goubetc 13.01.16
    //conditionLock->Release(); //+ goubetc 13.01.16
    while (!sleeping->IsEmpty()) {
	Semaphore* temp = (Semaphore *) sleeping->Remove();
	temp->V();
	//scheduler->ReadyToRun((Thread *)conditionLock->queue->Remove ());
    }
    (void) interrupt->SetLevel (oldLevel);
}
// +e goubetc 18.01.16
