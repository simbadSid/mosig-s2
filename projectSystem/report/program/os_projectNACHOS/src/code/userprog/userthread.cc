/*
 * userthread.cc
 *
 *  Created on: Jan 10, 2016
 *      Author: littlegirle
 */

// FoxTox 19.01.2015

#include "userthread.h"

static int nbrUserThread = 0;

// ------------------------------------------
// ThreadCreationParameter:
// Class used to communicate data to the delayed thread management handlers
// ------------------------------------------
ThreadCreationParameter::ThreadCreationParameter(int FUNC, int ARG,  int EXIT_FUNC, int STACK_POINTER)
{
    this->func			= FUNC;
    this->arg			= ARG;
    this->exitFunc		= EXIT_FUNC;
    this->stackPointer	= STACK_POINTER;
}
ThreadCreationParameter::~ThreadCreationParameter() {}


// ------------------------------------------
// Thread management handlers (delayed function: executed by the schedular)
// ------------------------------------------
static void StartUserThread(int f)
{
	AddrSpace *space = currentThread->space;

	space->InitRegisters ();											// set the initial register values of the processor
	space->RestoreState ();												// load page table register
#ifndef USER_PROGRAM
	return;
#endif
	ThreadCreationParameter *tcp = (ThreadCreationParameter*) f;		// Case of a user thread: Inisome processor register:
	machine->WriteRegister(4,			(int)tcp->arg);					//		Write the parameters of the first function to be executed by the thread
	machine->WriteRegister(PCReg,		(int)tcp->func);				//		Update pc with the address of the first function to be executed
	machine->WriteRegister(NextPCReg,	(int)tcp->func+4);
	machine->WriteRegister(StackReg,	(int)tcp->stackPointer);		//		Update the stack pointer
	machine->WriteRegister(RetAddrReg,	(int)tcp->exitFunc);

	delete tcp;
	DEBUG ('t', "Run the thread \"%s\" at the address %d\n", currentThread->getName(), (int)tcp->func);
	machine->Run();
}

//----------------------------------------------------------------------
// System functions to manage the thread
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// - Initialize the memory space of the caller thread using the memory space of the current thread.
// - Allocates space of the caller thread stack (according to the need of the function f and the arguments arg).
// - Makes the pointer register of caller thread indicate the function f address.
// Parameters:
//		* func	: Kernel pointer on the function to executed by the new thread
//		* arg	: Kernel pointer on the arguments
// Returns:
//		* TID of the created thread in case of success (TID > 0)
//		* -1 if the new thread stack can not be allocated(only error detected is the lack of memory for the stack allocation)
//----------------------------------------------------------------------
int do_UserThreadCreate(int func, int arg, int exitFunc)
{
	ThreadCreationParameter *tcp;
	char *name = new char ();
	// +b FoxTox 19.01.2015
	int		tid				= initThreadName(name);
	Thread	*createdThread	= new Thread(name, tid);
	int		newThreadStack	= -1;
	int		test;

	test = createdThread->UserThreadCreate(currentThread, &newThreadStack);	// Allocate space for the new thread stack pointer in the currentThread Address space
	if (test < 0) {															// Case no space left
		return test;
	}
	// +e FoxTox 19.01.2015
	userThreadList->Prepend(tid, createdThread);
	tcp = new ThreadCreationParameter(func, arg, exitFunc, newThreadStack);
	createdThread->Fork(StartUserThread, (int)tcp);
	return tid;
}

//----------------------------------------------------------------------
// Finishes the thread current thread and removes it from the thread list.
//----------------------------------------------------------------------
void do_UserThreadExit ()
{
	Thread *removedThread;
	int		tid	= currentThread->getTID();
	bool	test= userThreadList->Remove(tid, (void**)&removedThread);

	ASSERT(test);
	ASSERT(currentThread == removedThread);
	removedThread->UserThreadExit();										// Unallocates the space allocated for the stack

	//+b goubetc 13.01.16
	variableCondition->Broadcast(joinCondition);
	variableCondition->Signal(haltCondition);
	//+e goubetc

	DEBUG('e', "\tEnd of user thread exit\n");
	// +b FoxTox 19.01.2015
	if (userThreadList->IsEmpty())
	{
		interrupt->Halt();
	}
	else
	{
		removedThread->Finish();
	}
	// +e FoxTox 19.01.2015
}

// +b FoxTox 19.01.2015
// ----------------------------------------------------
//+b simbadSid 22.01.2016
// Creates a new address space totally different from the current one.
// Creates a new thread within the new address space  to execute the program represented by
// the given program name.
// Return the TID of the created thread in case of success.
// Return -1 if the given executable file does not exist. (only handled error).
//+b simbadSid 22.01.2016
// ----------------------------------------------------
int do_ForkExec(char *fileName) {
    OpenFile *executable = fileSystem->Open(fileName, READ);
//+b simbadSid 22.01.2016
	if (executable == NULL) return -1;
//+e simbadSid 22.01.2016
	char *threadName = new char();

	int tid = initThreadName(threadName);
	Thread *programThread = new Thread(threadName, tid);
	programThread->space = new AddrSpace(executable);

	int	newThreadStack	= -1;
	int test = programThread->UserThreadCreate(programThread, &newThreadStack);
//+b simbadSid 22.01.2016
	ASSERT(test >= 0);
//+e simbadSid 22.01.2016
	userThreadList->Prepend(programThread->getTID(), programThread);
	ThreadCreationParameter *tcp = new ThreadCreationParameter(0, 0, 0, newThreadStack);
	programThread->Fork(StartUserThread, (int)tcp);

	return tid;
}
// +e FoxTox 19.01.2015
// ----------------------------------------------------
// Auxiliary functions
// ----------------------------------------------------
int initThreadName(char *name)
{
	nbrUserThread++;
	int tid = nbrUserThread;
	name = new char[THREAD_NAME_MAX_SIZE];
	sprintf(name, "User Thread %d", tid);
	return tid;
}

