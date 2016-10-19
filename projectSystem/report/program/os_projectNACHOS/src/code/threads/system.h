// system.h 
//      All global variables used in Nachos are defined here.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.


#ifndef SYSTEM_H
#define SYSTEM_H

#include "copyright.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "timer.h"

// FoxTox 08.01.2016
// simbadSid 10.01.2016
// goubetc 13.01.2016

// Initialization and cleanup routines
extern void Initialize (int argc, char **argv);	// Initialization, called before anything else
extern void Cleanup ();							// Cleanup, called when Nachos is done.
extern Thread *currentThread;					// the thread holding the CPU
extern Thread *threadToBeDestroyed;				// the thread that just finished
extern Scheduler *scheduler;					// the ready list
extern Interrupt *interrupt;					// interrupt status
extern Statistics *stats;						// performance metrics
extern Timer *timer;							// the hardware alarm clock

#ifdef USER_PROGRAM
#include "machine.h"
#include "synchconsole.h"
#include "userthread.h"
#include "keylist.h"
#include "synch.h"
#include "frameprovider.h"
extern Machine			*machine;				// user program memory and registers
extern SynchConsole		*synchconsole;
extern KeyList			*userThreadList;		// Set of all the threads currently existing
extern FrameProvider	*frameProvider;			// Physical frame manager
//+b goubetc 13.01.16
extern Lock *joinCondition;              		// condition variable for use of join
extern Lock *haltCondition;
extern Condition *variableCondition;
//+e goubetc 13.01.16

#endif

#ifdef FILESYS_NEEDED							// FILESYS or FILESYS_STUB
#include "filesys.h"
extern FileSystem *fileSystem;
#endif

#ifdef FILESYS
#include "synchdisk.h"
extern SynchDisk *synchDisk;
#endif

#ifdef NETWORK
#include "post.h"
extern PostOffice *postOffice;
#endif

#endif // SYSTEM_H
