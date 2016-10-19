/*
 * ioTest.c
 *
 *  Created on: Jan 8, 2016
 *      Author: littlegirle
 */


// simbadSid 9.01.16


#include "syscall.h"


#define LONG_STRING_SIZE	128		// Size of the writing buffer of the system funvtion PutString
#define LONG_STRING_CHAR	'-'
#define LONG_STRING_END		"|||\n\0"



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
// Initialize the string with the same char
// Write "###" at the end of the string
// -------------------------------------
void initString(char *str, int size)
{
	int i=0, j;

	for (i=0; i<size-strlen(LONG_STRING_END); i++)
	{
		str[i] = LONG_STRING_CHAR;
	}
	for (j=0; j<strlen(LONG_STRING_END); j++)
	{
		str[i+j] = LONG_STRING_END[j];
	}
	str[i+j] = '\0';
}
int main ()
{
	char *str;

	str= "qwerty\n";							// Test small string write
	PutString(str, strlen(str));

	str= "qwer\0ty";
	PutString(str, strlen(str)+2);

	str = "\nIT WORKS...\n";
	PutString(str, strlen(str));

	char longStr[LONG_STRING_SIZE];				// Test long string write: bigger than the writing buffer of the system funvtion PutString
	initString(longStr, LONG_STRING_SIZE);
	PutString(longStr, strlen(longStr));

	char longStr2[3*LONG_STRING_SIZE+10];
	initString(longStr2, 3*LONG_STRING_SIZE+10);
	PutString(longStr2, strlen(longStr2));

//TODO uncomment
//	Halt ();

    /* not reached */
    return 0;
}
