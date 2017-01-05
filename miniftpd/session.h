// Edit by cyj 2016-12-31
#ifndef __SESSION_H__
#define __SESSION_H__

#include "common.h"

typedef struct session
{
  //控制连接
  uid_t uid;         
  int ctrl_fd;          //控制连接套接字
  char cmdline[MAX_COMMAND_LINE];
  char cmd[MAX_COMMAND];
  char arg[MAX_ARG];

  //父子进程通道
  int parent_fd;
  int child_fd;

  //FTP协议状态
  int is_ascii;
} session_t;

void begin_session(session_t *sess);

#endif //__SESSION_H__