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

  //数据连接
  struct sockaddr_in *port_addr;       //主动连接地址
  int pasv_listen_fd;
  int data_fd;
  int data_process;

  //限速
  unsigned int bw_upload_rate_max;
  unsigned int bw_download_rate_max;
  long bw_transfer_start_sec;
  long bw_transfer_start_usec;

  //父子进程通道
  int parent_fd;
  int child_fd;

  //FTP协议状态
  int is_ascii;
  long long restart_pos;
  char *rnfr_name;
  int abor_received;
} session_t;

void begin_session(session_t *sess);

#endif //__SESSION_H__