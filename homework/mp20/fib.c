#include <stdio.h>
#include <stdlib.h>

int fib(int n)
{
	if( n == 0 )
		return 0;
	else
		if( n == 1 )
			return 1;
		else
			return fib(n-2) + fib(n-1);
}

int main(int argc, char *argv[])
{
    printf("%d\n", fib(atoi(argv[1])));
    return 0;
}
