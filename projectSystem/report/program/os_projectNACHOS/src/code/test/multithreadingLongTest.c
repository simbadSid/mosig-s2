/*
 * multithreadingLongTest.c
 *
 *  Created on: Jan 15, 2016
 *      Author: littlegirle
 */



// simbadSid 14.01.16


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
void threadFunction0(void *arg)
{
	PutChar('\n');
	PutChar('\t');
	PutChar('a');
	PutChar('a');
	PutChar('a');
	PutChar('a');
	PutChar('a');
	PutChar('b');
	PutChar('\n');
	UserThreadExit();		// Not mandatory: function UserThreadExit defined as return function of the user thread creat system call
}
void threadFunction1(void *arg)
{
	char *str = "\tFunction 1: Simple thread function without parameters\n\0";
	PutString(str, strlen(str));
}
void threadFunction2(void *arg)
{
	int arg1 = *((int*) arg);
	char *str = "\tFunction 2: Simple thread function with int parameter: \0";

	PutString(str, strlen(str));
	PutInt(arg1);
	str = "\n\0";
	PutString(str, strlen(str));
}
void threadFunction3(void *arg)
{
	int		arg1 = *((int*) arg);
	int		arg2 = *((int*) arg+1);
	char	*str = "\tFunction 3: Simple thread function with an int parameter: \0";

	PutString(str, strlen(str));
	PutInt(arg1);
	str = " and a char parameter: \0";
	PutString(str, strlen(str));
	PutChar((char)arg2);
	str = "\n\0";
	PutString(str, strlen(str));
}
// -------------------------------------
// Main function
// -------------------------------------
int main (int argc, char **argv)
{
	int arg2[] = {76};
	int arg3[] = {57, (int)'c'};

	int tid0 = UserThreadCreate(threadFunction0, 0);
	int tid1 = UserThreadCreate(threadFunction1, 0);
	int tid2 = UserThreadCreate(threadFunction2, (void*)arg2);
	int tid3 = UserThreadCreate(threadFunction3, (void*)arg3);

	UserThreadJoin(tid0);
	UserThreadJoin(tid1);
	UserThreadJoin(tid2);
	UserThreadJoin(tid3);

	Halt ();

	/* not reached */
	return 0;
}
