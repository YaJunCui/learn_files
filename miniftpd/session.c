//Edit by cyj 2016-12-31

#include "session.h"
#include "sysutil.h"
#include "ftpproto.h"
#include "privparent.h"
#include "privsock.h"

void begin_session(session_t *sess)
{
  priv_sock_init(sess);              //父子进程socket初始化

  pid_t pid;
  pid = fork();
  if (pid < 0)
  {
    ERR_EXIT("fork");
  }
  else if (pid == 0)                //子进程，ftp服务进程
  {
    priv_sock_set_child_context(sess);
    handle_child(sess);
  }
  else                              //父进程，nobody进程
  {
    priv_sock_set_parent_context(sess);
    handle_parent(sess);
  }
}