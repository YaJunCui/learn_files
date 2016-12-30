//Edit by cyj 2016-12-30
#ifndef __COMMON_H__
#define __COMMON_H__

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sysutil.h"

#define  ERR_EXIT(m) \
  do \
  { \
    perror(m); \
    exit(EXIT_FAILURE); \
  } \
  while(0)

#endif //__COMMON_H__