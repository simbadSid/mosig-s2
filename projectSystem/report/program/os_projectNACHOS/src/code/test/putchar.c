#include "syscall.h"


// FoxTox 08.01.2016
// FoxTox 09.01.2016


//+b FoxTox 08.01.2016
void print(char c, int n)
{	
//    int i;
//    for (i = 0; i < 20; i++) {
//    	char temp = GetChar();
//     	PutChar(temp);
//    }
	char line[11];
	GetString(line, 11);
	PutString(line, 10);
    PutChar('\n');
    int numb;
    GetInt(&numb);
    PutInt(numb);
    //Halt();
}

//+e FoxTox 08.01.2016


//+b FoxTox 09.01.2016
int
main()
{
    print('a',2);
    Halt();
    // create system call for
}
//+e FoxTox 09.01.2016
