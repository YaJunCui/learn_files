// Edit by cyj 2017-1-1

#include "privparent.h"
#include "sysutil.h"

void handle_parent(session_t *sess)
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

  char cmd;
  while (1)
  {
    read(sess->parent_fd, &cmd, 1);

    //处理内部命令

    //处理命令
  }
}