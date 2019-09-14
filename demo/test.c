#include <stdio.h>
#define PI (3.14159265)
int show(void);
int main(){
    printf("hello world!\n");
    double param, result;
    param = 60.0;
    result = cos (param * PI / 180.0);
    printf ("The cosine of %f degrees is %f.\n", param, result);
    show();
    return 0;
}

extern int func();

int show(void){
   printf("show is running...!\n");
   func();
   printf("\n");
   return 0;
}

// 方法1：
 // gcc test.c test2.c -o test.out -lm
// ./test.out

// 方法2：
// gcc -fPIC -shared test2.c -o libtest2.so
// gcc test.c libtest2.so -o test.out -lm
// ./test.out

// 方法3：makefile