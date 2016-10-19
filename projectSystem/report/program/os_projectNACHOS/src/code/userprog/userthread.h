/*
 * userthread.h
 *
 *  Created on: Jan 10, 2016
 *      Author: littlegirle
 */

// simbadSid 10.01.2015
// FoxTox 19.01.2015


#ifndef USERTHREAD_H_
#define USERTHREAD_H_

#include "thread.h"
#include "system.h"


// ------------------------------------------
// Class used to communicate data to the delayed thread management handlers
// ------------------------------------------
class ThreadCreationParameter
{
	public:
		ThreadCreationParameter(int FUNC, int ARG,  int EXIT_FUNC, int STACK_POINTER);
		~ThreadCreationParameter();

		int func;
		int arg;
		int exitFunc;
		int	stackPointer;

};

// ------------------------------------------
// User Thread system functions
// ------------------------------------------
extern int	do_UserThreadCreate	(int f, int arg, int exitFunc);		// +b simbadSid 13.01.2015
extern void	do_UserThreadExit	();									// +b simbadSid 13.01.2015
extern int	do_ForkExec			(char *fileName);					// +b FoxTox 19.01.2015



// ----------------------------------------------------
// Auxiliary functions
// ----------------------------------------------------
extern int initThreadName(char *name);


#endif
