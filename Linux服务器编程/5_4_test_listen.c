#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libgen.h>

static int stop = 0;

static void handle_term(int sig) //SIGTERM信号处理函数
{
  stop = 1;
}

int main(int argc, char *argv[])
{
  signal(SIGTERM, handle_term);

  if (argc <= 3)
  {
    printf("Usage: %s ip_address port_number backlog\n",
           basename(argv[0]));
    return -1;
  }

  const char *ip = argv[1];
  int port = atoi(argv[2]);
  int backlog = atoi(argv[3]);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock > 0);

  struct sockaddr_in address;         //创建一个 IPv4 socket 地址
  bzero(&address, sizeof(address));

  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = inet_addr(ip);

  int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);

  ret = listen(sock, backlog);
  assert(ret != -1);

  //循环等待连接，直到SIGTERM信号中断
  while (!stop)
  {
    sleep(1);
  }

  //关闭socket
  close(sock);

  return 0;
}