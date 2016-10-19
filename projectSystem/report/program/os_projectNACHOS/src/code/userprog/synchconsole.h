#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "console.h"
#include "synch.h"

// FoxTox 09.01.2016

#define MAX_INT_DIGITS 20


class SynchConsole
{
	public:
		SynchConsole(char *readFile, char *writeFile);	// initialize the hardware console device
		~SynchConsole();								// clean up console emulation
		void SynchPutChar(const char ch);				// Unix putchar(3S)
		char SynchGetChar();							// Unix getchar(3S)
		void SynchPutString(const char *s);				// Unix puts(3S)
		//+b FoxTox 09.01.2016
		void SynchGetString(char *s, int n);			// Unix fgets(3S)
		void SynchGetInt(int *n);
		void SynchPutInt(int n);
		//+e FoxTox 09.01.2016

		int SynchGetCharInt();   //+ TooFo 11012016
	private:
		Console *console;
};





#endif // SYNCHCONSOLE_H
