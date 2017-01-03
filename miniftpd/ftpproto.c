//Edit by 2017-1-1

#include "session.h"
#include "sysutil.h"
#include "ftpproto.h"
#include "str.h"

static void do_user(session_t *sess);
static void do_pass(session_t *sess);

void handle_child(session_t *sess)
{
  writen(sess->ctrl_fd, "220 (miniftpd 0.1)\r\n", strlen("220 (miniftpd 0.1)\r\n"));

  int ret = 0;

  while (1)
  {
    memset(sess->cmdline, 0, sizeof(sess->cmdline));
    memset(sess->cmd, 0, sizeof(sess->cmd));
    memset(sess->arg, 0, sizeof(sess->arg));

    ret = readline(sess->ctrl_fd, sess->cmdline, MAX_COMMAND_LINE);
    if (ret == -1)
    {
      ERR_EXIT("readline");
    }
    else if (ret == 0)
    {
      exit(EXIT_SUCCESS);
    }

    // printf("cmdline=[%s]\n", sess->cmdline);

    //去掉字符串尾部的\r\n
    str_trim_crlf(sess->cmdline);
    printf("cmdline=[%s]\n", sess->cmdline);

    //解析 FTP 命令与参数
    str_split(sess->cmdline, sess->cmd, sess->arg, ' ');
    printf("cmd=[%s] arg=[%s]\n", sess->cmd, sess->arg);

    //将命令转为大写
    str_upper(sess->cmd);

    //处理 FTP 命令
    if (strcmp("USER", sess->cmd) == 0)
    {
      do_user(sess);
    }
    else if (strcmp("PASS", sess->cmd) == 0)
    {
      do_pass(sess);
    }
  }
}

void do_user(session_t *sess)
{
  struct passwd *pw = getpwnam(sess->arg); //获取用户信息
  if (pw == NULL)                          //用户不存在
  {
    writen(sess->ctrl_fd, "530 Login incorrect.\r\n", strlen("530 Login incorrect.\r\n"));
  }
  else
  {
  }

  writen(sess->ctrl_fd, "331 Please specify the password.\r\n", strlen("331 Please specify the password.\r\n"));
}

void do_pass(session_t *sess)
{
  writen(sess->ctrl_fd, "230 Login successful.\r\n", strlen("230 Login successful.\r\n"));
}