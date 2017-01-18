#include "syscall.h"

int main()
{
	ForkExec("./userpage0");
	ForkExec("./userpage1");
	ForkExec("./userpage0");
	ForkExec("./userpage1");
	ForkExec("./userpage0");
	ForkExec("./userpage1");
	ForkExec("./userpage0");
	ForkExec("./userpage1");
	ForkExec("./userpage0");
	ForkExec("./userpage1");	
	return 0;
}