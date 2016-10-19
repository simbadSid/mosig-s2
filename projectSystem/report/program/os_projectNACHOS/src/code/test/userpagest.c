// +b FoxTox 19.01.2015
#include "syscall.h"


int main() {
	int tid0 = ForkExec("userpages1");
	int tid1 = ForkExec("userpages0");

// +b simbadSid x20.01.2016
	tid0++;
	tid1++;
// +e simbadSid 20.01.2016
	return 0;
}
// +e FoxTox 19.01.2015
