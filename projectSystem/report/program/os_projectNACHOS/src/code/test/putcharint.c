#include "syscall.h"

//+b FoxTox 08.01.2016
//void print(char c, int n)
void print_chars()
{
	int temp_i;
	temp_i = GetCharInt();
	PutChar((char)temp_i);
	PutChar('\n');
}

int
main()
{
    print_chars();
    return 0;
    // create system call for
}
//+e FoxTox 09.01.2016
