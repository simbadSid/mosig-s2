/*
 * multithreadingTest.c
 *
 *  Created on: Jan 10, 2016
 *      Author: littlegirle
 */

// simbadSid 9.01.16


#include "syscall.h"



// -------------------------------------
// Return the length of the string
// -------------------------------------
int strlen(char *str)
{
    int res = 0;
    char *buffer = str;

    while (*buffer != '\0')
	{
	    buffer ++;
	    res ++;
	}
    return res;
}

// -------------------------------------
// Handler of the thread creation
// -------------------------------------
void threadFunction1(void *arg)
{
    PutChar('\n');
    PutChar('\t');
    PutChar('1');
    PutChar('-');
    PutChar('a');
    PutChar('a');
    PutChar('a');
    PutChar('b');
    PutChar('\n');
    UserThreadExit();		// Not mandatory: function UserThreadExit defined as return function of the user thread creat system call
}
void threadFunction2(void *arg)
{
    PutChar('\t');
    PutChar('2');
    PutChar('-');
    PutChar('z');
    PutChar('z');
    PutChar('z');
    PutChar('y');
    PutChar('\n');
}
void threadFunction3(void *arg)
{
    int arg1 = *((int*) arg);
    //char arg2 = *((int*) (arg+1));
    PutChar('\t');
    PutInt(arg1);
    PutChar('c');
    PutChar('\n');
}
void threadFunction4(void *arg)
{
    int		arg1 = *((int*) arg);
    int		arg2 = *((int*) arg+1);

    PutChar('\t');
    PutInt(arg1);
    PutChar(' ');
    PutChar(arg2);
    PutChar('\n');
}
void threadFunctionJoin(void *arg)
{
    int	tid = *((int*) arg);
    UserThreadJoin(tid);
}

// -------------------------------------
// Function to test a specific part of the code
// -------------------------------------
void testCreation()
{
    int arg3[] = {3};
    int arg4[] = {4, (int)'c'};

    int tid1 = UserThreadCreate(threadFunction1, 0);
    if (tid1 <= 0)
	{
	    PutString("\n****Failed to create thread tid1***\n", 100);
	    return;
	}
    int tid2 = UserThreadCreate(threadFunction2, 0);
    if (tid2 <= 0)
	{
	    PutString("\n****Failed to create thread tid2***\n", 100);
	    return;
	}
    int tid3 = UserThreadCreate(threadFunction3, (void*)arg3);
    if (tid3 <= 0)
	{
	    PutString("\n****Failed to create thread tid3***\n", 100);
	    return;
	}
    int tid4 = UserThreadCreate(threadFunction4, (void*)arg4);
    if (tid4 <= 0)
	{
	    PutString("\n****Failed to create thread tid4***\n", 100);
	    return;
	}

	UserThreadJoin(tid1);
	UserThreadJoin(tid2);
	UserThreadJoin(tid3);
	UserThreadJoin(tid4);
}
void testJoin()
{
    int arg0[]	= {57, (int)'c'};
    int tid0	= UserThreadCreate(threadFunction3,		(void*)arg0);
    int arg1[]	= {tid0};
    int tid1	= UserThreadCreate(threadFunctionJoin,	(void*)arg1);

    UserThreadJoin(tid1);
}
// -------------------------------------
// Main function
// -------------------------------------
int main (int argc, char **argv)
{
	testCreation();
//    testJoin();

//    Halt ();

    /* not reached */
    return 0;
}

