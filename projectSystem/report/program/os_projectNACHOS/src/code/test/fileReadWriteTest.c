#include "syscall.h"

// FoxTox 24.01.2015


// Stress test - should not work, it is normal.
int main() {
	int id0, id1;
	char name []= "FileAlisa";
	Create(name);
	id1 = Open(name, 1);
	id0 = Open(name, 0);
	Close(id1);
//+b simbadSid 27.01.2016
id0++;
//+e simbadSid 27.01.2016
	return 0;
}

