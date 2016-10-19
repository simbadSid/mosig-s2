#include "syscall.h"

void threadFunction1() {
	char name [] = "File0";
	Create(name);
	int id = Open(name, 0);
	Close(id);
}

void threadFunction2() {
	char name [] = "File1";
	Create(name);
	int id = Open(name, 0);
	Close(id);
}

int main() {
	int tid1 = UserThreadCreate(threadFunction1, 0);
	int tid2 = UserThreadCreate(threadFunction2, 0);
	UserThreadJoin(tid1);
	UserThreadJoin(tid2);
	return 0;
}
