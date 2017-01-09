// Edit by cyj 2017-1-6
#include "common.h"
#include "privsock.h"

void priv_sock_init(session_t *sess)
{
  int ret = 0;
  int sockfds[2];
  //创建套接字对，用于父子进程间的通信
  ret = socketpair(PF_UNIX, SOCK_STREAM, 0, sockfds);
  if (ret < 0)
  {
    ERR_EXIT("socketpair");
  }

  sess->parent_fd = sockfds[0];
  sess->child_fd = sockfds[1];
}

void priv_sock_sock_close(session_t *sess)
{
  if (sess->child_fd != -1)       //关闭 sess->child_fd 套接字
  {
    close(sess->child_fd); 
    sess->child_fd = -1;
  }
  if (sess->parent_fd != -1)      //关闭 sess->parent_fd 套接字
  {
    close(sess->parent_fd); 
    sess->parent_fd = -1;
  }
}

void priv_sock_set_parent_context(session_t *sess)
{
  if (sess->child_fd != -1)       //关闭 sess->child_fd 套接字
  {
    close(sess->child_fd); 
    sess->child_fd = -1;
  }
}

void priv_sock_set_child_context(session_t *sess)
{
  if (sess->parent_fd != -1)      //关闭 sess->parent_fd 套接字
  {
    close(sess->parent_fd); 
    sess->parent_fd = -1;
  }
}

void priv_sock_send_cmd(int fd, char cmd)     //发送命令（子->父）
{
  int ret = 0;
  writen()
}

char priv_sock_get_cmd(int fd)                //接收命令（父->子）
{

}

void priv_sock_send_result(int fd, char res)  //发送结果（父->子）
{

}

char priv_sock_get_result(int fd)             //接收结果（子->父）
{

}