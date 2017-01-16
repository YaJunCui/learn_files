//Edit by cyj 2016-12-30

#include "common.h"
#include "sysutil.h"
#include "session.h"
#include "str.h"
#include "tunable.h"
#include "parseconf.h"
#include "ftpproto.h"
#include "ftpcodes.h"

session_t *g_sess;
static unsigned int s_children;

void check_limits(session_t *sess);
void handle_sigchld(int sig);

int main()
{
  //list_common();
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

  parseconf_load_file(MINIFTPD_CONF);                        //加载配置项

  // printf("tunable_pasv_enable = %d\n", tunable_pasv_enable);
  // printf("tunable_port_enable = %d\n", tunable_port_enable);
  // printf("tunable_listen_port = %d\n", tunable_listen_port);
  // printf("tunable_max_clients = %d\n", tunable_max_clients);
  // printf("tunable_max_per_ip = %d\n", tunable_max_per_ip);
  // printf("tunable_accept_timeout = %d\n", tunable_accept_timeout);
  // printf("tunable_connect_timeout = %d\n", tunable_connect_timeout);
  // printf("tunable_idle_session_timeout = %d\n", tunable_idle_session_timeout);
  // printf("tunable_data_connection_timeout = %d\n", tunable_data_connection_timeout);
  // printf("tunable_local_umask = %o\n", tunable_local_umask);
  // printf("tunable_upload_max_rate = %d\n", tunable_upload_max_rate);
  // printf("tunable_download_max_rate = %d\n", tunable_download_max_rate);

  // if(tunable_listen_address == NULL)
  // {
  //   printf("tunable_listen_address = NULL\n");
  // }
  // else
  // {
  //    printf("tunable_listen_address = %s\n", tunable_listen_address);
  // }

  if (getuid() != 0)
  {
    fprintf(stderr, "miniftpd : must be started as root\n");
    exit(EXIT_FAILURE);
  }

  session_t sess =
  {
      0, -1, "", "", "",                  //控制连接
      NULL, -1, -1, 0,                    //数据连接
      0, 0, 0, 0,                         //限速
      -1, -1,                             //父子进程通道
      0 ,0, NULL, 0,                      //FTP 状态
      0                                   //连接数的限制
  };
  g_sess = &sess;

  sess.bw_upload_rate_max = tunable_upload_max_rate;
  sess.bw_download_rate_max = tunable_download_max_rate;

  signal(SIGCHLD, handle_sigchld);
  
  int listen_fd = tcp_server(NULL, tunable_listen_port);
  int conn;
  pid_t pid;

  while (1)
  {
    conn = accept_timeout(listen_fd, NULL, 0);
    if (conn == -1)
    {
      ERR_EXIT("accept_timeout");
    }

    ++s_children;
    sess.num_clients = s_children;

    pid = fork();
    if (pid == -1)
    {
      --s_children;
      ERR_EXIT("fork");
    }
    else if (pid == 0)
    {
      close(listen_fd);
      sess.ctrl_fd = conn;
      check_limits(&sess);
      signal(SIGCHLD, SIG_IGN);          
      begin_session(&sess);
    }
    else
    {
      close(conn);
    }
  }

  return 0;
}

void check_limits(session_t *sess)
{
  if(tunable_max_clients > 0 && sess->num_clients > tunable_max_clients)
  {
    ftp_reply(sess, FTP_TOO_MANY_USERS,
              "There are too many connected users. Please try later.");
    exit(EXIT_FAILURE);
  }
}

void handle_sigchld(int sig)
{
  pid_t pid;
  while((pid = waitpid(-1, NULL, WNOHANG)) < 0);    //必须检测到有子进程结束

  --s_children;
}