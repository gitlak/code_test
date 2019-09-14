
/*
   file test 
   jet   2019.7.7   16.30

*/
#include<stdio.h>
 
int main ()
{
   FILE *fp = NULL;
   char str[] = "This is runoob.com----\n";
 
   fp = fopen( "file.txt" , "w" );
   fwrite(str, sizeof(str) , 1, fp );
 
   fclose(fp);
  
   return 0;
}
