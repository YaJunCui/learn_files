//Edit by cyj 2016-12-30

#include "sysutil.h"
#include "session.h"
#include "str.h"

int main()
{
  // char *str1 = "    ab";
  // char *str2 = "     ";

  // if (str_all_space(str1))
  // {
  //   printf("str1 all space\n");
  // }
  // else
  // {
  //   printf("str1 not all space\n");
  // }

  // if (str_all_space(str2))
  // {
  //   printf("str2 all space\n");
  // }
  // else
  // {
  //   printf("str2 not all space \n");
  // }

  // char str3[] = "abcdDEFgkj";
  // str_upper(str3);
  // printf("%s\n", str3);

  // char *str4 = "9876543219870";
  // long long str4_num = str_to_longlong(str4);
  // printf("%lld\n", str4_num);

  // char *str5 = "0711";
  // unsigned int str5_num = str_octal_to_uint(str5);
  // printf("%d\n", str5_num);

  if (getuid() != 0)
  {
    fprintf(stderr, "miniftpd : must be started as root\n");
    exit(EXIT_FAILURE);
  }

  session_t sess =
      {
          -1, "", "", "", //控制连接
          -1, -1          //父子进程通道
      };

  int listen_fd = tcp_server(NULL, 5188);
  int conn;
  pid_t pid;

  while (1)
  {
    conn = accept_timeout(listen_fd, NULL, 0);
    if (conn == -1)
    {
      ERR_EXIT("accept_timeout");
    }

    pid = fork();
    if (pid == -1)
      ERR_EXIT("fork");

    if (pid == 0)
    {
      close(listen_fd);
      sess.ctrl_fd = conn;
      begin_session(&sess);
    }
    else
    {
      close(conn);
    }
  }

  return 0;
}
