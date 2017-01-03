//Edit by 2017-1-1

#include "session.h"
#include "sysutil.h"
#include "ftpproto.h"
#include "str.h"

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

    printf("cmdline=[%s]\n", sess->cmdline);

    //去掉字符串尾部的\r\n
    str_trim_crlf(sess->cmdline);
    printf("cmdline=[%s]\n", sess->cmdline);

    //解析 FTP 命令与参数
    str_split(sess->cmdline, sess->cmd, sess->arg, ' ');
    printf("cmd=[%s] arg=[%s]\n", sess->cmd, sess->arg);

    //处理 FTP 命令
  }
}