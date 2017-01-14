//Edit by cyj 2016-12-30

#ifndef _COMMON_H_
#define _COMMON_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <crypt.h>
#include <shadow.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <net/if.h> 
#include <netinet/in.h> 
#include <net/if_arp.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <linux/capability.h>
#include <sys/syscall.h>
#include <sys/sendfile.h>

//1、因为宏定义要求只能在一行完成，所以用反斜线可以把多行连接成一行
//2、不用担心 do{}while(0) 运行的消耗，编译器会自动优化它——只执行一次就结束了
//3、不需要加';'，因为用户使用 ERR_EXIT(m) 时，会加上 ';'，把 ERR_EXIT(m); 当成语句来使用
#define ERR_EXIT(m) \
  do \
  { \
    perror(m); \
	  exit(EXIT_FAILURE); \
  } \
  while (0)

#define MAX_COMMAND_LINE    1024
#define MAX_COMMAND         32
#define MAX_ARG             1024
#define MINIFTPD_CONF       "miniftpd.conf"

#endif //__COMMON_H__