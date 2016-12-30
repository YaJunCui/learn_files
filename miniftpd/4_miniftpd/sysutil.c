#include "sysutil.h"

int getlocalip(char *ip)                //获取本机ip地址
{
  if (ip == NULL)
    return -1;

  char hostname[128] = { 0 };
  struct hostent *hent;

  //获取主机名
  if (gethostname(hostname, sizeof(hostname)) < 0)
    ERR_EXIT("gethostname");
  printf("hostname=%s\n", hostname);
  //通过主机名获取ip地址
  if ((hent = gethostbyname(hostname)) < 0)
    return -1;
  //printf("iplen=%d\n",(unsigned int)sizeof(struct in_addr));

  strcpy(ip, inet_ntoa(*(struct in_addr*)hent->h_addr));
  printf("ip=%s\n", inet_ntoa(*(struct in_addr*)hent->h_addr));//输出ip
  return 0;
}

/*
 *activate_nonblock:  设置IO为非阻塞模式
 *@fd:                文件描述符
 */
static void activate_nonblock(int fd)
{
  int ret;
  int flags = fcntl(fd, F_GETFL);
  if (flags == -1)
    ERR_EXIT("fcntl error");

  flags |= O_NONBLOCK;
  ret = fcntl(fd, F_SETFL, flags);
  if (ret == -1)
    ERR_EXIT("fcntl error");
}

/*
 *deactivate_nonblock: 设置IO为阻塞模式
 *@fd:                 文件描述符
 */
static void deactivate_nonblock(int fd)
{
  int ret;
  int flags = fcntl(fd, F_GETFL);
  if (flags == -1)
    ERR_EXIT("fcntl error");

  flags &= ~O_NONBLOCK;
  ret = fcntl(fd, F_SETFL, flags);
  if (ret == -1)
    ERR_EXIT("fcntl error");
}

/*
 *accept_timeout: 接受客户端的连接
 *@fd:            服务器端的套接字
 *@addr:          保存客户端的地址
 *@wait_seconds:  等待超时时间
 */
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
  int ret;
  socklen_t addrlen = sizeof(struct sockaddr_in);

  if (wait_seconds > 0)
  {
    fd_set accept_fdset;
    struct timeval timeout;
    FD_ZERO(&accept_fdset);
    FD_SET(fd, &accept_fdset);
    timeout.tv_sec = wait_seconds;
    timeout.tv_usec = 0;
    do
    {
      ret = select(fd + 1, &accept_fdset, NULL, NULL, &timeout);
    } while (ret < 0 && errno == EINTR);  //当发生中断重新select
    if (ret == -1)
      return -1;
    else if (ret == 0)
    {
      errno = ETIMEDOUT;
      return -1;
    }
  }
  if (addr != NULL)
    ret = accept(fd, (struct sockaddr*)addr, &addrlen);
  else
    ret = accept(fd, NULL, NULL);

  return ret;
}

/*
 *connect_timeout=connect
 *@fd:            套接字
 *@addr:          要连接的对方地址
 *@wait_seconds:  等待超时秒数，如果为0表示正常模式
 *成功(未超时)返回0，失败返回-1，超时返回-1并且errno=ETIMEDOUT
 */
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
  int ret;
  socklen_t addrlen = sizeof(struct sockaddr_in);

  if (wait_seconds > 0)
  {
    //fd设为非阻塞模式
    activate_nonblock(fd);
  }

  ret = connect(fd, (struct sockaddr *)addr, addrlen);
  if (ret < 0 && errno == EINPROGRESS)
  {
    fd_set connect_fdset;
    struct timeval timeout;
    FD_ZERO(&connect_fdset);
    FD_SET(fd, &connect_fdset);

    timeout.tv_sec = wait_seconds;
    timeout.tv_usec = 0;

    do
    {
      /*一旦连接建立，套接字就可写*/
      ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
    } while (ret < 0 && errno == EINTR);

    if (ret == 0)
    {//返回0表示时间超时
      errno = ETIMEDOUT;
      return -1;
    }
    else if (ret < 0) /*返回-1表示出错*/
      return -1;
    else if (ret == 1)
    {
      /*
      ret 返回为1，可能有两种情况，一种是连接建立成功，一种是
      套接字产生错误
      *此时错误信息不会保存至errno变量中（select没有出错），因此需要调用
      getsockopt来获取
      */
      int err;
      socklen_t socklen = sizeof(err);
      int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
      if (sockoptret == -1)
        return -1;
      if (err == 0)
        ret = 0;
      else
      {
        errno = err;
        ret = -1;
      }
    }

  }
  if (wait_seconds > 0)
    deactivate_nonblock(fd);

  return ret;
}

ssize_t readn(int fd, void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nread;
  char *ptr;
  ptr = vptr;
  nleft = n;
  while (nleft > 0)
  {
    if ((nread = read(fd, ptr, nleft)) < 0)
    {
      if (errno == EINTR)
        nread = 0;
      else
        return -1;
    }
    else if (nread == 0)
      break;
    nleft -= nread;
    ptr += nread;
  }
  return n - nleft;
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;

  ptr = vptr;
  nleft = n;
  while (nleft > 0)
  {
    if ((nwritten = write(fd, ptr, nleft)) <= 0)
    {
      if (nwritten < 0 && errno == EINTR)
        nwritten = 0;
      else
        return -1;
    }
    nleft -= nwritten;
    ptr += nwritten;
  }
  return n;
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
  ssize_t n, rc;
  char c, *ptr;
  ptr = vptr;
  for (n = 1; n < maxlen; n++) {
    if ((rc = my_read(fd, &c)) == 1) {
      *ptr++ = c;
      if (c == '\n')
        break;
    }
    else if (rc == 0) {
      *ptr = 0;
      return n - 1;
    }
    else
      return -1;
  }
  *ptr = 0;
  return n;
}

void  send_fd(int  sock_fd, int  send_fd)       //发送网络套接字
{
  int  ret;
  struct  msghdr msg;
  struct  cmsghdr *p_cmsg;
  struct  iovec vec;
  char  cmsgbuf[CMSG_SPACE(sizeof(send_fd))];
  int  *p_fds;
  char  sendchar = 0;
  msg.msg_control = cmsgbuf;
  msg.msg_controllen = sizeof(cmsgbuf);
  p_cmsg = CMSG_FIRSTHDR(&msg);
  p_cmsg->cmsg_level = SOL_SOCKET;
  p_cmsg->cmsg_type = SCM_RIGHTS;
  p_cmsg->cmsg_len = CMSG_LEN(sizeof(send_fd));
  p_fds = (int  *)CMSG_DATA(p_cmsg);
  *p_fds = send_fd;  // 通过传递辅助数据的方式传递文件描述符 

  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;  //主要目的不是传递数据，故只传1个字符 
  msg.msg_flags = 0;

  vec.iov_base = &sendchar;
  vec.iov_len = sizeof(sendchar);
  ret = sendmsg(sock_fd, &msg, 0);
  if (ret != 1)
    ERR_EXIT("sendmsg");
}

int  recv_fd(const   int  sock_fd)                //接收网络套接字
{
  int  ret;
  struct  msghdr msg;
  char  recvchar;
  struct  iovec vec;
  int  recv_fd;
  char  cmsgbuf[CMSG_SPACE(sizeof(recv_fd))];
  struct  cmsghdr *p_cmsg;
  int  *p_fd;
  vec.iov_base = &recvchar;
  vec.iov_len = sizeof(recvchar);
  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;
  msg.msg_control = cmsgbuf;
  msg.msg_controllen = sizeof(cmsgbuf);
  msg.msg_flags = 0;

  p_fd = (int  *)CMSG_DATA(CMSG_FIRSTHDR(&msg));
  *p_fd = -1;
  ret = recvmsg(sock_fd, &msg, 0);
  if (ret != 1)
    ERR_EXIT("recvmsg");

  p_cmsg = CMSG_FIRSTHDR(&msg);
  if (p_cmsg == NULL)
    ERR_EXIT("no passed fd");

  p_fd = (int  *)CMSG_DATA(p_cmsg);
  recv_fd = *p_fd;
  if (recv_fd == -1)
    ERR_EXIT("no passed fd");

  return  recv_fd;
}