// Edit by cyj 2017-1-1

#include "privparent.h"
#include "sysutil.h"
#include "privsock.h"
#include "tunable.h"

static void privop_pasv_get_data_sock(session_t *sess);
static void privop_pasv_active(session_t *sess);
static void privop_pasv_listen(session_t *sess);
static void privop_pasv_accept(session_t *sess);

int capset(cap_user_header_t hdrp, const cap_user_data_t datap)
{
  return syscall(__NR_capset, hdrp, datap);            //调用内核接口
}

void minimize_privilege(void)                          //使进程获得最小的特权
{
    struct passwd *pw = getpwnam("nobody");
  if (pw == NULL)
  {
    return;
  }

  if (setegid(pw->pw_gid) < 0) //将当前进程的有效组id设置为父进程的组id
  {
    ERR_EXIT("setegid");
  }

  if (seteuid(pw->pw_uid) < 0) //将当前进程的有效用户id设置为父进程的用户id
  {
    ERR_EXIT("seteuid");
  }

  struct __user_cap_header_struct cap_header;
  struct __user_cap_data_struct cap_data;
  memset(&cap_header, 0, sizeof(cap_header));
  memset(&cap_data, 0, sizeof(cap_data));

  cap_header.version = _LINUX_CAPABILITY_VERSION_2;
  cap_header.pid = 0;

  __u32 cap_mask = 0;
  cap_mask |= (1 << CAP_NET_BIND_SERVICE);

  cap_data.effective = cap_mask;
  cap_data.permitted = cap_mask;
  cap_data.inheritable = 0;

  capset(&cap_header, &cap_data);
}

void handle_parent(session_t *sess)
{
  minimize_privilege();

  char cmd;
  while (1)
  {
    cmd = priv_sock_get_cmd(sess->parent_fd);
    //处理内部命令
    //处理命令
    switch (cmd)
    {
    case PRIV_SOCK_GET_DATA_SOCK:
      privop_pasv_get_data_sock(sess);
      break;

    case PRIV_SOCK_PASV_ACTIVE:
      privop_pasv_active(sess);
      break;

    case PRIV_SOCK_PASV_LISTEN:
      privop_pasv_listen(sess);
      break;

    case PRIV_SOCK_PASV_ACCEPT:
      privop_pasv_accept(sess);
      break;

    default:
      break;
    }
  }
}

static void privop_pasv_get_data_sock(session_t *sess)
{
  unsigned short port = (unsigned short)priv_sock_get_int(sess->parent_fd);
  char ip[16] = {0};
  priv_sock_recv_buf(sess->parent_fd, ip, sizeof(ip));
 
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);

  int fd = tcp_client(20);     //创建套接字
  if(fd == -1)
  {
    priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_BAD);
    return;
  }

  int ret = connect_timeout(fd, &addr, tunable_connect_timeout);
  if (ret < 0) //连接失败，关闭套接字，然后返回 0
  {
    close(fd);
    priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_BAD);
    return;
  }

  priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_OK);
  priv_sock_send_fd(sess->parent_fd, fd);
  close(fd); 
}

static void privop_pasv_active(session_t *sess)
{

}

static void privop_pasv_listen(session_t *sess)
{

}

static void privop_pasv_accept(session_t *sess)
{
  
}