// Edit by cyj 2016-12-30

#include "sysutil.h"

int tcp_client(unsigned short port)
{
  int ret = 0;
  int sock = socket(PF_INET, SOCK_STREAM, 0);
  if(sock < 0)
  {
    ERR_EXIT("tcp_client");
  }

  if(port > 0)               //只有 PORT 模式才需要绑定，此时 port 不为 0
  {
    int on = 1;
    ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                     (const char *)&on, sizeof(on)); //设置地址重复利用
    if (ret < 0)
    {
      ERR_EXIT("setsockopt");
    }

    char ip[16] = {0};
    getlocalip(ip);                                   //获取本机 IP 地址
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));

    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = inet_addr(ip);

    ret = bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
    if(ret < 0)
    {
      ERR_EXIT("bind");
    }
  }
  
  return sock;
}

/*
 * tcp_server - 启动 tcp 服务器
 * @host:       服务器 IP 地址或者服务器主机名
 * @port：       服务器端口
 * 成功返回监听套接字
 */
int tcp_server(const char *host, unsigned short port)
{
  int ret = 0;
  int listen_fd = socket(PF_INET, SOCK_STREAM, 0); //创建套接字
  if (listen_fd < 0)
    ERR_EXIT("tcp_server");

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  if (host != NULL) //填充 IP 地址
  {
    if (inet_aton(host, &serv_addr.sin_addr) == 0)
    {
      struct hostent *hp;
      hp = gethostbyname(host);
      if (hp == NULL)
      {
        ERR_EXIT("getlocalip");
      }
      serv_addr.sin_addr = *(struct in_addr *)hp->h_addr;
    }
  }
  else
  {
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  }
  serv_addr.sin_port = htons(port); //绑定端口

  int on = 1;
  ret = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)); //设置地址重复利用
  if (ret < 0)
  {
    ERR_EXIT("setsockopt");
  }

  ret = bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); //绑定套接字
  if (ret < 0)
  {
    ERR_EXIT("bind");
  }

  ret = listen(listen_fd, SOMAXCONN);
  if (ret < 0)
  {
    ERR_EXIT("listen");
  }

  return listen_fd;
}

// int getlocalip(char *ip)
// {
//   char host[100] = {0};
//   if (gethostname(host, sizeof(host)) < 0)
//     return -1;
//   struct hostent *hp;
//   if ((hp = gethostbyname(host)) == NULL)
//     return -1;

//   strcpy(ip, inet_ntoa(*(struct in_addr *)hp->h_addr));

//   return 0;
// }

int getlocalip(char *ip)
{

  int sock_get_ip;

  struct sockaddr_in *sin;
  struct ifreq ifr_ip;            // ifreq 用来保存某个接口的信息

  if ((sock_get_ip = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    printf("socket create fail...GetLocalIp!\n");
    return 0;
  }

  memset(&ifr_ip, 0, sizeof(ifr_ip));
  strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);
  // SIOCGIFADDR 获得接口的地址
  if (ioctl(sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0)
  {
    return 0;
  }
  sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
  strcpy(ip, inet_ntoa(sin->sin_addr));

  close(sock_get_ip);

  return 1;
}

/*
 * activate_noblock - 设置I/O为非阻塞模式
 * @fd: 文件描符
 */
void activate_nonblock(int fd)
{
  int ret;
  int flags = fcntl(fd, F_GETFL);
  if (flags == -1)
    ERR_EXIT("fcntl");

  flags |= O_NONBLOCK;
  ret = fcntl(fd, F_SETFL, flags);
  if (ret == -1)
    ERR_EXIT("fcntl");
}

/*
 * deactivate_nonblock - 设置I/O为阻塞模
 * @fd: 文件描符
 */
void deactivate_nonblock(int fd)
{
  int ret;
  int flags = fcntl(fd, F_GETFL);
  if (flags == -1)
    ERR_EXIT("fcntl");

  flags &= ~O_NONBLOCK;
  ret = fcntl(fd, F_SETFL, flags);
  if (ret == -1)
    ERR_EXIT("fcntl");
}

/**
 * read_timeout - 读超时检测函数，不含读操作
 * @fd: 文件描述符
 * @wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
 */
int read_timeout(int fd, unsigned int wait_seconds)
{
  int ret = 0;
  if (wait_seconds > 0)
  {
    fd_set read_fdset;
    struct timeval timeout;

    FD_ZERO(&read_fdset);
    FD_SET(fd, &read_fdset);

    timeout.tv_sec = wait_seconds;
    timeout.tv_usec = 0;
    do
    {
      ret = select(fd + 1, &read_fdset, NULL, NULL, &timeout);
    } while (ret < 0 && errno == EINTR);

    if (ret == 0)
    {
      ret = -1;
      errno = ETIMEDOUT;
    }
    else if (ret == 1)
      ret = 0;
  }

  return ret;
}

/**
 * write_timeout - 读超时检测函数，不含写操作
 * @fd: 文件描述符
 * @wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
 */
int write_timeout(int fd, unsigned int wait_seconds)
{
  int ret = 0;
  if (wait_seconds > 0)
  {
    fd_set write_fdset;
    struct timeval timeout;

    FD_ZERO(&write_fdset);
    FD_SET(fd, &write_fdset);

    timeout.tv_sec = wait_seconds;
    timeout.tv_usec = 0;
    do
    {
      ret = select(fd + 1, NULL, NULL, &write_fdset, &timeout);
    } while (ret < 0 && errno == EINTR);

    if (ret == 0)
    {
      ret = -1;
      errno = ETIMEDOUT;
    }
    else if (ret == 1)
      ret = 0;
  }

  return ret;
}

/**
 * accept_timeout - 带超时的accept
 * @fd:           套接字
 * @addr:         输出参数，返回对方地址
 * @wait_seconds: 等待超时秒数，如果为0表示正常模式
 * 成功（未超时）返回已连接套接字，超时返回-1并且errno = ETIMEDOUT
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
    } while (ret < 0 && errno == EINTR);
    if (ret == -1)
      return -1;
    else if (ret == 0)
    {
      errno = ETIMEDOUT;
      return -1;
    }
  }

  if (addr != NULL)
    ret = accept(fd, (struct sockaddr *)addr, &addrlen);
  else
    ret = accept(fd, NULL, NULL);
  /*	if (ret == -1)
		ERR_EXIT("accept");
		*/

  return ret;
}

/**
 * connect_timeout - connect
 * @fd: 套接字
 * @addr: 要连接的对方地址
 * @wait_seconds: 等待超时秒数，如果为0表示正常模式
 * 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
 */
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
  int ret;
  socklen_t addrlen = sizeof(struct sockaddr_in);

  if (wait_seconds > 0)
    activate_nonblock(fd);

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
      /* 一量连接建立，套接字就可写 */
      ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
    } while (ret < 0 && errno == EINTR);
    if (ret == 0)
    {
      ret = -1;
      errno = ETIMEDOUT;
    }
    else if (ret < 0)
      return -1;
    else if (ret == 1)
    {
      /* ret返回为1，可能有两种情况，一种是连接建立成功，一种是套接字产生错误，*/
      /* 此时错误信息不会保存至errno变量中，因此，需要调用getsockopt来获取。 */
      int err;
      socklen_t socklen = sizeof(err);
      int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
      if (sockoptret == -1)
      {
        return -1;
      }
      if (err == 0)
      {
        ret = 0;
      }
      else
      {
        errno = err;
        ret = -1;
      }
    }
  }
  if (wait_seconds > 0)
  {
    deactivate_nonblock(fd);
  }
  return ret;
}

/**
 * readn - 读取固定字节数
 * @fd:       文件描述符
 * @buf:      接收缓冲区
 * @count:    要读取的字节数
 * 成功返回count，失败返回-1，读到EOF返回<count
 */
ssize_t readn(int fd, void *buf, size_t count)
{
  size_t nleft = count;
  ssize_t nread;
  char *bufp = (char *)buf;

  while (nleft > 0)
  {
    if ((nread = read(fd, bufp, nleft)) < 0)
    {
      if (errno == EINTR)
        continue;
      return -1;
    }
    else if (nread == 0)
      return count - nleft;

    bufp += nread;
    nleft -= nread;
  }

  return count;
}

/**
 * writen - 发送固定字节数
 * @fd:       文件描述符
 * @buf:      发送缓冲区
 * @count:    要读取的字节数
 * 成功返回count，失败返回-1
 */
ssize_t writen(int fd, const void *buf, size_t count)
{
  size_t nleft = count;
  ssize_t nwritten;
  char *bufp = (char *)buf;

  while (nleft > 0)
  {
    if ((nwritten = write(fd, bufp, nleft)) < 0)
    {
      if (errno == EINTR)
        continue;
      return -1;
    }
    else if (nwritten == 0)
      continue;

    bufp += nwritten;
    nleft -= nwritten;
  }

  return count;
}

/**
 * recv_peek - 仅仅查看套接字缓冲区数据，但不移除数据
 * @sockfd: 套接字
 * @buf: 接收缓冲区
 * @len: 长度
 * 成功返回>=0，失败返回-1
 */
ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
  while (1)
  {
    int ret = recv(sockfd, buf, len, MSG_PEEK);
    if (ret == -1 && errno == EINTR)
      continue;
    return ret;
  }
}

/**
 * readline - 按行读取数据
 * @sockfd: 套接字
 * @buf: 接收缓冲区
 * @maxline: 每行最大长度
 * 成功返回>=0，失败返回-1
 */
ssize_t readline(int sockfd, void *buf, size_t maxline)
{
  int ret;
  int nread;
  char *bufp = buf;
  int nleft = maxline;
  int count = 0;
  while (1)
  {
    ret = recv_peek(sockfd, bufp, nleft);
    if (ret < 0)
      return ret; // 返回小于0表示失败
    else if (ret == 0)
      return ret; //返回0表示对方关闭连接了

    nread = ret;
    int i;
    for (i = 0; i < nread; i++)
    {
      if (bufp[i] == '\n')
      {
        ret = readn(sockfd, bufp, i + 1);
        if (ret != i + 1)
          exit(EXIT_FAILURE);

        bufp[i + 1] = '\0';
        return ret + count;
      }
    }
    if (nread > nleft)
      exit(EXIT_FAILURE);
    nleft -= nread;
    ret = readn(sockfd, bufp, nread);
    if (ret != nread)
      exit(EXIT_FAILURE);

    bufp += nread;
    count += nread;
  }

  return -1;
}

void send_fd(int sock_fd, int fd)
{
  int ret;
  struct msghdr msg;
  struct cmsghdr *p_cmsg;
  struct iovec vec;
  char cmsgbuf[CMSG_SPACE(sizeof(fd))];
  int *p_fds;
  char sendchar = 0;
  msg.msg_control = cmsgbuf;
  msg.msg_controllen = sizeof(cmsgbuf);
  p_cmsg = CMSG_FIRSTHDR(&msg);
  p_cmsg->cmsg_level = SOL_SOCKET;
  p_cmsg->cmsg_type = SCM_RIGHTS;
  p_cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
  p_fds = (int *)CMSG_DATA(p_cmsg);
  *p_fds = fd;

  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;
  msg.msg_flags = 0;

  vec.iov_base = &sendchar;
  vec.iov_len = sizeof(sendchar);
  ret = sendmsg(sock_fd, &msg, 0);
  if (ret != 1)
    ERR_EXIT("sendmsg");
}

int recv_fd(const int sock_fd)
{
  int ret;
  struct msghdr msg;
  char recvchar;
  struct iovec vec;
  int recv_fd;
  char cmsgbuf[CMSG_SPACE(sizeof(recv_fd))];
  struct cmsghdr *p_cmsg;
  int *p_fd;
  vec.iov_base = &recvchar;
  vec.iov_len = sizeof(recvchar);
  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;
  msg.msg_control = cmsgbuf;
  msg.msg_controllen = sizeof(cmsgbuf);
  msg.msg_flags = 0;

  p_fd = (int *)CMSG_DATA(CMSG_FIRSTHDR(&msg));
  *p_fd = -1;
  ret = recvmsg(sock_fd, &msg, 0);
  if (ret != 1)
    ERR_EXIT("recvmsg");

  p_cmsg = CMSG_FIRSTHDR(&msg);
  if (p_cmsg == NULL)
    ERR_EXIT("no passed fd");

  p_fd = (int *)CMSG_DATA(p_cmsg);
  recv_fd = *p_fd;
  if (recv_fd == -1)
    ERR_EXIT("no passed fd");

  return recv_fd;
}

const char *statbuf_get_perms(struct stat *sbuf)  //获取文件的权限状态
{
  static char perms[] = "----------"; //权限位
  perms[0] = '?';

  mode_t mode = sbuf->st_mode;
  switch (mode & S_IFMT)
  {
  case S_IFREG: //普通文件
    perms[0] = '-';
    break;

  case S_IFDIR: //文件夹
    perms[0] = 'd';
    break;

  case S_IFLNK: //符号连接文件
    perms[0] = 'l';
    break;

  case S_IFIFO: //管道文件
    perms[0] = 'p';
    break;

  case S_IFSOCK: //socket
    perms[0] = 's';
    break;

  case S_IFCHR: //字符设备文件
    perms[0] = 'c';
    break;

  case S_IFBLK: //块设备文件
    perms[0] = 'b';
    break;

  default:
    break;
  }

  if (mode & S_IRUSR) //当前用户的读、写、执行权限
  {
    perms[1] = 'r';
  }
  if (mode & S_IWUSR)
  {
    perms[2] = 'w';
  }
  if (mode & S_IXUSR)
  {
    perms[3] = 'x';
  }

  if (mode & S_IRGRP) //组用户的读、写、执行权限
  {
    perms[4] = 'r';
  }
  if (mode & S_IWGRP)
  {
    perms[5] = 'w';
  }
  if (mode & S_IXGRP)
  {
    perms[6] = 'x';
  }

  if (mode & S_IROTH) //其他用户的读、写、执行权限
  {
    perms[7] = 'r';
  }
  if (mode & S_IWOTH)
  {
    perms[8] = 'w';
  }
  if (mode & S_IXOTH)
  {
    perms[9] = 'x';
  }

  if (mode & S_ISUID) //特殊权限的处理
  {
    perms[3] = (perms[3] == 'x') ? 's' : 'S';
  }
  if (mode & S_ISGID)
  {
    perms[6] = (perms[6] == 'x') ? 's' : 'S';
  }
  if (mode & S_ISVTX)
  {
    perms[9] = (perms[9] == 'x') ? 't' : 'T';
  }

  return perms;
}

const char *statbuf_get_date(struct stat *sbuf)   //获取文件的时间
{
  static char date_buf[64] = {0};        //文件的日期
  const char *p_date_format = "%b %e %H:%M";
  struct timeval tv;
  gettimeofday(&tv, NULL);
  time_t local_time = tv.tv_sec;

  if (sbuf->st_mtime > local_time || 
      (local_time - sbuf->st_mtime) > 182 * 24 * 60 * 60)
  {
    p_date_format = "%b %e  %Y";
  }

  struct tm *p_tm = localtime(&sbuf->st_mtime);
  strftime(date_buf, sizeof(date_buf), p_date_format, p_tm);

  return date_buf;
}

static int lock_internal(int fd, int lock_type)
{
  int ret = 0;
  struct flock the_lock;
  memset(&the_lock, 0, sizeof(the_lock));
  the_lock.l_type = lock_type;
  the_lock.l_whence = SEEK_SET;
  the_lock.l_start = 0;
  the_lock.l_len = 0;

  do
  {
    ret = fcntl(fd, F_SETLKW, &the_lock);
  } while (ret < 0 && errno == EINTR);

  return ret;
}

int lock_file_read(int fd)                         //加读文件锁
{
  return lock_internal(fd, F_RDLCK);
}

int lock_file_write(int fd)                        //加写文件锁
{
  return lock_internal(fd, F_WRLCK);
}

int unlock_file(int fd)                            //解除文件锁，包括读/写锁
{
  int ret = 0;
  struct flock the_lock;
  memset(&the_lock, 0, sizeof(the_lock));
  the_lock.l_type = F_UNLCK;
  the_lock.l_whence = SEEK_SET;
  the_lock.l_start = 0;
  the_lock.l_len = 0;

  ret = fcntl(fd, F_SETLK, &the_lock);

  return ret;
}