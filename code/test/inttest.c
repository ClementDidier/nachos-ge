#include "syscall.h"

int main()
{
	int n;
	GetInt(&n);
	PutInt(n);

	n = GetCharInt();
	PutInt(n);
	return 0;
}