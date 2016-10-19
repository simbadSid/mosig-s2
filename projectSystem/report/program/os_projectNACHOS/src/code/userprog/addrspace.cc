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

#include <strings.h>		/* for bzero */


// +b simbadSid 15.01.2015


// +b simbadSid 19.01.2016
// --------------------------------------------------------------------
// Convert the page number of the top stack pointer to its virtual address.
// Consider the address alignment of the stack top pointer.
// Do not use out of this class (do not put it in the .h)
// --------------------------------------------------------------------
#define stackTopPageToStackTopVirtualAddress(stackTopPage)	((stackTopPage * PageSize) - 16)
// +e simbadSid 19.01.2016


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

// +b simbadSid 15.01.2015
// --------------------------------------------------------------------
// Reads in the input file numBytes starting at the given position.
// The read bytes are stored at the given virtual address in the
// memory described by (pageTable, numPages)
// --------------------------------------------------------------------
static void ReadAtVirtual(OpenFile *executable, int virtualaddr, int numBytes, int position,
						  TranslationEntry *pageTable, unsigned numPages)
{
	IntStatus			oldLevel		= interrupt->SetLevel(IntOff);
	TranslationEntry	*oldPageTable	= machine->pageTable;
	unsigned			oldNnumPages	= machine->pageTableSize;

	machine->pageTable		= pageTable;
	machine->pageTableSize	= numPages;
	interrupt->SetLevel(oldLevel);

	char buffer[numBytes];
	int nbrRedBytes	= executable->ReadAt(buffer, numBytes, position);

    oldLevel = interrupt->SetLevel(IntOff);
    for(int virtualByteShift = 0; virtualByteShift < nbrRedBytes; ++virtualByteShift)
    {
        machine->WriteMem(virtualaddr+virtualByteShift, 1, buffer[virtualByteShift]);
    }
	machine->pageTable		= oldPageTable;
	machine->pageTableSize	= oldNnumPages;
    interrupt->SetLevel(oldLevel);
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
AddrSpace::AddrSpace (OpenFile * executable, int maxNbrThread)
{
	NoffHeader noffH;
	unsigned int i, size;

	executable->ReadAt ((char *) &noffH, sizeof (noffH), 0);								// Reads the header of the executable file
	if ((noffH.noffMagic != NOFFMAGIC) && (WordToHost (noffH.noffMagic) == NOFFMAGIC))		// Convert the header to the host endian
		SwapHeader (&noffH);
	ASSERT (noffH.noffMagic == NOFFMAGIC);													// Ensure that the executable has the good file type

																							// how big is address space?
	size = noffH.code.size + noffH.initData.size + noffH.uninitData.size					// Read the expected addrspace size expected by the
				+ maxNbrThread * UserStackSize;												//		executable file.
																							//		We need to increase the size to leave room for the stack
	numPages	= divRoundUp (size, PageSize);
	pageBitmap	= new BitMap(numPages);
	size		= numPages * PageSize;
	ASSERT (numPages <= NumPhysPages);														// check we're not trying to run anything too big --
																							// TODO at least until we have virtual memory
	ASSERT (frameProvider->NumAvailFrame() >= numPages);									// check we're not trying to run anything too big --

	DEBUG ('a', "Initializing address space, num pages %d, size %d\n", numPages, size);
																							// first, set up the translation
	pageTable = new TranslationEntry[numPages];												// Initialize the page table of the addrSpace
	for (i = 0; i < numPages; i++)
	{
		pageTable[i].virtualPage	= i;													//		TODO for now, virtual page # = phys page #
		pageTable[i].physicalPage	= frameProvider->GetEmptyFrame();
		pageTable[i].valid			= TRUE;
		pageTable[i].use			= FALSE;
		pageTable[i].dirty			= FALSE;
		pageTable[i].readOnly		= FALSE;												//		if the code segment was entirely on
																							//		a separate page, we could set its pages to be read-only
		DEBUG ('a', "\t->Virtual page %d\t<-> physical page %d\n",
				pageTable[i].virtualPage, pageTable[i].physicalPage);
	}
	this->threadStackList = new KeyList();
																							// Copy code and segments of the executable into addrSpace
																							//		Writes the file code section in memory
	ReadAtVirtual(executable, noffH.code.virtualAddr, noffH.code.size, noffH.code.inFileAddr, pageTable, numPages);
	this->codeFirstPage	= noffH.code.virtualAddr/ PageSize;
	this->codeNbrPage	= noffH.code.size		/ PageSize;
	DEBUG ('a', "Initializing code segment: size %d (bytes)\n", noffH.code.size);
	DEBUG ('a', "\t-> Virtual  address:\t0x%x\n",	noffH.code.virtualAddr);
	DEBUG ('a', "\t-> First page:\t\t%d\n", 		this->codeFirstPage);
	DEBUG ('a', "\t-> Number of page:\t%d\n", 		this->codeNbrPage);
	for (i=codeFirstPage; i<codeFirstPage+codeNbrPage; i++)
	{
		this->pageBitmap->Mark(i);															//		Notify the code pages as used
		pageTable[i].readOnly	= true;														//		Ensure that the code will not be corrupted
	}
																							// Copy data segments of the executable into addrSpace
																							//		Writes the file data section in memory
	ReadAtVirtual(executable, noffH.initData.virtualAddr, noffH.initData.size, noffH.initData.inFileAddr, pageTable, numPages);
	this->initDataFirstPage		= noffH.initData.virtualAddr	/ PageSize;
	this->uninitDataFirstPage	= noffH.uninitData.virtualAddr	/ PageSize;
	this->initDataNbrPage		= noffH.initData.size			/ PageSize;
	this->uninitDataNbrPage		= noffH.uninitData.size			/ PageSize;
	DEBUG ('a', "Initializing data segment: size %d (bytes)\n", noffH.initData.size);
	DEBUG ('a', "\t-> Virtual  address:\t0x%x\n",	noffH.initData.virtualAddr);
	DEBUG ('a', "\t-> First page:\t\t%d\n", 		this->initDataFirstPage);
	DEBUG ('a', "\t-> Number of page:\t%d\n", 		this->initDataNbrPage);
	DEBUG ('a', "\t-> First page:\t\t%d\n", 		this->initDataFirstPage);
	DEBUG ('a', "\t-> Number of page:\t%d\n", 		this->initDataNbrPage);
	for (i=initDataFirstPage; i<initDataFirstPage+initDataNbrPage; i++)
	{
		this->pageBitmap->Mark(i);														//		Notify the data pages as used
	}
	for (i=uninitDataFirstPage; i<uninitDataFirstPage+uninitDataNbrPage; i++)
	{
		this->pageBitmap->Mark(i);														//		Notify the data pages as used
	}
	if (noffH.uninitData.size % PageSize) {
		this->pageBitmap->Mark(i++);
	}
	this->pageBitmap->Mark(i++);
/*
	unsigned int nbrCodePages = divRoundUp (noffH.code.size, PageSize);
	unsigned int nbrDataPages = divRoundUp (noffH.initData.size, PageSize);
// TODO to change
	int remainingCode	= nbrCodePages % PageSize;
	int remainingData	= nbrDataPages % PageSize;
	if ((remainingCode != 0) && (remainingData != 0) && ((remainingCode + remainingData) > PageSize))
		 this->pageBitmap->Mark(nbrCodePages + nbrDataPages - 1);
*/
}
// +e simbadSid 15.01.2015

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//      Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace ()
{
	unsigned int i;

	for (i = 0; i < numPages; i++)
	{
		frameProvider->ReleaseFrame(pageTable[i].physicalPage);			// Mark the phisycal page as free
	}
	delete [] pageTable;
	delete pageBitmap;
	this->threadStackList->FreeAllList();
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
	int i, stackPointer;

	for (i = 0; i < NumTotalRegs; i++)
		machine->WriteRegister (i, 0);

	machine->WriteRegister (PCReg, 0);			// Initial program counter -- must be location of "Start"
	machine->WriteRegister (NextPCReg, 4);		// Need to also tell MIPS where next instruction is, because
												// of branch delay possibility
												// Set the stack register to the end of the address space, where we
												// allocated the stack; but subtract off a bit, to make sure we don't
												// accidentally reference off the end!
	// +b simbadSid 19.01.2016

	stackPointer = GetThreadTopStackPointer(currentThread->getTID());
	ASSERT(stackPointer >= 0);
	// +e simbadSid 19.01.2016
	machine->WriteRegister (StackReg, stackPointer);
	DEBUG ('a', "Initializing stack register to %d\n", stackPointer);
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

// +b simbadSid 15.01.2016

//----------------------------------------------------------------------
// Allocate memory in the userStack for the new thread tid.   The corresponding memory is filled with zeroes.
// Marks all the memory as allocated in the address space bitmap.
// The created stack pointer is returned in the parameter newStackPointer.
// Return 0 if the allocation has succeeded and -1 otherwise.
// The pointer on the new allocated stack is put in the variable newStackPointer (virtual address).
// The only managed error is the memory out of space.
//----------------------------------------------------------------------
int AddrSpace::AllocateThreadStack(int tid, int *newStackPointer)
{
// TODO begin critical section
	// Get the index of the lowest free page
	int stackTopPage = this->pageBitmap->FindLast(USER_THREAD_STACK_PAGES + 1);
	int lowestPage		= stackTopPage - USER_THREAD_STACK_PAGES-1;
	int page;
	void *physicalPageAddress;

	if(stackTopPage == -1)														// Case full memory:
	{
// TODO replace this by a page creation
		DEBUG('t', "\t*** AllocateThreadStack: full memory***\n");
		return -1;
	}
	stackTopPage--;
	for(page=stackTopPage; page>lowestPage; page--)								// For each page of the allocated stack
	{
		this->pageBitmap->Mark(page);											//		Mark the page as allocated
		physicalPageAddress = &(machine->mainMemory[pageTable[page].physicalPage]);
		bzero(physicalPageAddress, PageSize);									//		Initialize the corresponding physical memory with zeros
	}
	this->threadStackList->Prepend(tid, (void*)stackTopPage);
	if (newStackPointer != NULL)
	{
		*newStackPointer = stackTopPageToStackTopVirtualAddress(stackTopPage);	// Compute the virtual address of the new stack pointer;
	}

// TODO stop critical section
	return 1;
}
// --------------------------------------------------------
// Mark the stack allocated by the given thread as free.
// Return 1 in case of success and put the virtual address of the
// stack in the output parameter newStackPointer.
// If the given thread did not allocate any stack in the current addrSpace, -1 is returned.
// --------------------------------------------------------
int AddrSpace::FreeThreadStack(int tid, int *newStackPointer)
{
	int stackTopPage = -1;
	bool test = this->threadStackList->Remove(tid, (void**)&stackTopPage);

	if (!test)
	{
		return -1;
	}
	if (newStackPointer != NULL)
	{
		*newStackPointer = stackTopPageToStackTopVirtualAddress(stackTopPage);	// Compute the virtual address of the stack pointer;
	}
	for (int p=0; p<USER_THREAD_STACK_PAGES; p++)								// Mark the virtual pages as free
	{
		this->pageBitmap->Clear(stackTopPage-p);
	}
	return 1;
}
// --------------------------------------------------------------------
// Return the address space size in bytes
// --------------------------------------------------------------------
int AddrSpace::GetSize()
{
	return this->numPages*PageSize;
}
// --------------------------------------------------------------------
// Return the top stack pointer of the given thread within the current
// address space.
// Return -1 if the given thread did not allocate a stack in the current addrSpace.
// --------------------------------------------------------------------
int AddrSpace::GetThreadTopStackPointer(int tid)
{
	int stackTopPage = 0;
	bool test = this->threadStackList->IsInList(tid, (void**)&stackTopPage);
	if (!test)
	{
		return -1;
	}
	else		return stackTopPageToStackTopVirtualAddress(stackTopPage);
}

// --------------------------------------------------------------------
// Return the number of thread that have allocated a stack in the current addrSpace.
// --------------------------------------------------------------------
int AddrSpace::GetNbrThreadStack()
{
	return this->threadStackList->GetNbrElement();
}
