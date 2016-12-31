//Edit by cyj 2016-12-30

#include "sysutil.h"
#include "session.h"

int main()
{
  if(getuid()!=0)
  {
    fprintf(stderr,"miniftpd : must be started as root\n");
    exit(EXIT_FAILURE);
  }

  int listen_fd = tcp_server(NULL,5188);
  int conn;
  pid_t pid;

  while (1)
  {
    conn = accept_timeout(listen_fd, NULL, 0);
    if(conn == -1)
    {
      ERR_EXIT("accept_timeout");
    }

    pid = fork();
    if(pid == -1)
      ERR_EXIT("fork");

    if(pid == 0)
    {
      close(listen_fd);
      begin_session(conn);
    }
    else
    {
      close(conn);
    }
  }

  return 0;
}
