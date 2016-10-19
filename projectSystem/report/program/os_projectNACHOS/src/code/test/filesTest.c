#include "syscall.h"

// FoxTox 24.01.2015

int main() {
	char name [] = "Alisa";
	Create(name);
	int id = Open(name, 0);
	Close(id);
	return 0;
}
