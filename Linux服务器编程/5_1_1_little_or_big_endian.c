#include <stdio.h>

void byte_order()
{
  union
  {
    short value;
    char union_byte[sizeof(short)];
  }test;

  test.value = 0x0102;

  if((test.union_byte[0]==0x01) && (test.union_byte[1]==0x02))
  {
    printf("Big endian\n");
  }
  else if((test.union_byte[0]==0x02) && (test.union_byte[1]==0x01))
  {
    printf("Little endian\n");
  }
  else
  {
    printf("Unknown...\n");
  }
}

int main()
{
  byte_order();

  return 0;
}