#include "hash.h"
#include "common.h"

typedef struct stu
{
  char sno[5];
  char name[32];
  int age;
}stu_t;

int main()
{
  printf("main.\n");

  stu_t stu_arr[] = 
  {
    {"1234","AAAA", 20},
    {"4568", "BBBB", 23},
    {"6729", "AAAA", 19}
  };


  return 0;
}