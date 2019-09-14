#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct student_s
{
	int num;
	int age;
	char name[30];
	int grade;
} student_t;

int main(int argc, char const *argv[])
{
	/* code */
	student_t stu[3], *p;

	int i = 0;
	char ch[10];   
	// char *ch;  //no???
	for (i = 0; i < 3; ++i)
	{
		/* code */
		// ch = itoa(i, ch, 5);
		sprintf(ch, "%3d", i+1);
		printf("ch == \"%s\"\n", ch);
		stu[i].num = i+9;
		stu[i].age = i+20;
		sprintf(stu[i].name, "zhangsan%s", ch);
		stu[i].grade = i+90;
	}

	p = stu;
	// for (i = 0; i < 3; ++i)
	// {
	// 	/* code */
	// 	printf("num %d  age %d  name %s  grade %d\n", p[i].num, 
	// 		p[i].age, p[i].name, p[i].grade);

	// }

	for (; p < stu+3; ++p)
	{
		/* code */
		printf("num %2d  age %d  name %s  grade %d\n", p->num, 
			p->age, p->name, p->grade);
	}

	for (; ; )
	{
		/* code */
		printf("666\n");
		exit(0);
	}

	return 0;
}