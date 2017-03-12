#include <unistd.h>
#include <stdio.h>

int main()
{
  int uid = getuid();
  int euid = geteuid();

  printf("userid is %d, effective userid is %d\n", uid, euid);

  return 0;
}