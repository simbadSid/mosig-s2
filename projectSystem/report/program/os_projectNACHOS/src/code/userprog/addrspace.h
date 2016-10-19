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

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "bitmap.h"
#include "keylist.h"



#define UserStackSize			1000				// increase this as necessary!   // +b simbadSid 15.01.2016
#define USER_THREAD_STACK_PAGES	4
#define DEFAULT_NBR_THREAD		5


class AddrSpace
{
	public:
		// Builder/destructor
		AddrSpace (OpenFile * executable, int maxNbrThread=DEFAULT_NBR_THREAD);
													// Create an address space, initializing it with the
													//		program stored in the file "executable"
		~AddrSpace ();								// De-allocate an address space

		// Local methods
		void InitRegisters ();						// Initialize user-level CPU registers, before jumping to user code
		void SaveState ();							// Save/restore address space-specific
		void RestoreState ();						// info on a context switch
		int AllocateThreadStack(int tid, int *newStackPointer);	// Allocate a free part of the stack for a new thread. Marks the allocated memory in the bitmap
		int FreeThreadStack(int tid, int *newStackPointer);	// Free the stack allocated by the given thread.
		int GetSize();								// Return the address space size in bytes
		int GetThreadTopStackPointer(int tid);		// Return the stack pointer of the given thread
		int GetNbrThreadStack();					// Return the number of thread that have allocated a stack in the current addrSpace.

	private:
		TranslationEntry	*pageTable;				// Assume linear page table translation for now!
		unsigned int		numPages;				// Number of pages in the virtual address space
		//+b simbadSid 15.01.2015
		BitMap				*pageBitmap;			// Identify the pages reserved (mainly for the stack)
		KeyList				*threadStackList;		// List of the stackPointer of each thread
		unsigned int		codeFirstPage;
		unsigned int		codeNbrPage;
		unsigned int		initDataFirstPage;
		unsigned int		initDataNbrPage;
		unsigned int		uninitDataFirstPage;
		unsigned int		uninitDataNbrPage;
		//+e simbadSid 15.01.2015
};


#endif // ADDRSPACE_H
