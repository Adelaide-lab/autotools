#include <stdio.h>
#include <string.h>

typedef struct
{
    int math;
    int chemistry;
    int physics;
    char *name;
} Student;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Paramters are not qualified\n");
        return -1;
    }

    FILE *fp;
    Student student;

    fp = fopen(argv[1], "r");

    if (fp == NULL)
    {
        perror("FILE is not found");
    }
    char string[1024];
    int ret;
    while (fgets(string, sizeof(string), fp) != NULL)
    {
        ret = sscanf(string, "%d %d %d %s", &student.math, &student.chemistry, &student.physics, student.name);
        if (ret != 4)
        {
            // fprintf(stderr, "Input line is not valid\n");
        }
        else
        {
            fprintf(stderr, "Name:%s Math:%d, Chemistry:%d, Physics:%d\n", student.name, student.math, student.chemistry, student.physics);
        }
    }
    fclose(fp);
    return 0;
}