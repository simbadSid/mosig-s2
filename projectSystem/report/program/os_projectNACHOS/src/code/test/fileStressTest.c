#include "syscall.h"
// Stress test

int main() {
	int ids[10];
	int i;
	for (i = 0; i < 10; ++i) {
		char name []= "File0";
		name[4] = i +'0';
		Create(name);
		ids[i] = Open(name, 1);
	}
	for (i = 0; i < 10; ++i) {
		Close(ids[i]);
	}
	return 0;
}

