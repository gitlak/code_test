#include<stdio.h>

int Callback_1(int x) // Callback Function 1
{
	printf("Hello, this is Callback_1: x = %d \n", x);
	return 0;
}

int Callback_2(int x) // Callback Function 2
{
	printf("Hello, this is Callback_2: x = %d \n", x);
	return 0;
}

int Callback_3(int x) // Callback Function 3
{
	printf("Hello, this is Callback_3: x = %d \n", x);
	return 0;
}

int Handle(int y, int (*Callback)(int))  //函数指针，它指向的函数参数为int，返回值为int
{
	printf("Entering Handle Function. \n");
	Callback(y);
	printf("Leaving Handle Function. \n\n");
}

int main()
{
	int a = 2;
	int b = 4;
	int c = 6;

	printf("Entering Main Function. \n\n");

	Handle(a, Callback_1);
	Handle(b, Callback_2);
	Handle(c, Callback_3);

	printf("Leaving Main Function. \n");
	return 0;
}

// 运行结果：

// Entering Main Function.

// Entering Handle Function.
// Hello, this is Callback_1: x = 2
// Leaving Handle Function.

// Entering Handle Function.
// Hello, this is Callback_2: x = 4
// Leaving Handle Function.

// Entering Handle Function.
// Hello, this is Callback_3: x = 6
// Leaving Handle Function.

// Leaving Main Function.