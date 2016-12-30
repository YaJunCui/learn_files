//Edit by cyj 2016-12-30
#include "common.h"
#include "sysutil.h"

int main()
{
  if(getuid()!=0)
  {
    fprintf(stderr,"miniftpd : must be started as root\n");
    exit(EXIT_FAILURE);
  }

  return 0;
}
