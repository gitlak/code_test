#include<stdio.h>
#include<stdlib.h>

float func(int a, int b, int (* pun)(int, int)) {
	// return (*pun)(a, b);
	return pun(a, b);

}

int multi(int a, int b) {
	return a * b;
}

int main(int argc, char const *argv[])
{
	/* code */
	int a,b;
	float rv;
	if (argc < 3)
	{
		/* code */
		printf("too few arguments, please input again!\n");
		return 0;
	}
	printf("current process is %s\n", argv[0]);
	a = atoi(argv[1]);
	b = atoi(argv[2]);
	rv = func(a, b, multi);
	printf("rv ==== %f\n", rv);
	return 0;
}

// gcc hello.c -o hello
// ./hello 10 20
