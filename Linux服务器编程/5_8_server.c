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
#include <errno.h>
#include <libgen.h>

#define BUF_SIZE 1024

static int stop = 0;

static void handle_term(int sig) //SIGTERM信号处理函数
{
  stop = 1;
}

int main(int argc, char *argv[])
{
  signal(SIGTERM, handle_term);

  if (argc <= 2)
  {
    printf("Usage: %s ip_address port_number\n",
           basename(argv[0]));
    return -1;
  }

  const char *ip = argv[1];
  int port = atoi(argv[2]);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock > 0);

  struct sockaddr_in address;         //创建一个 IPv4 socket 地址
  bzero(&address, sizeof(address));

  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = inet_addr(ip);

  int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);

  ret = listen(sock, 5);
  assert(ret != -1);

  //sleep(20);

  struct sockaddr_in client;
  socklen_t client_length = sizeof(client);

  int conn_fd = accept(sock, (struct sockaddr*)&client, &client_length);

  if(conn_fd == -1)
  {
    printf("errno is: %d\n", errno);
  }
  else
  {
    char buffer[BUF_SIZE];

    memset(buffer, '\0', BUF_SIZE);
    ret = recv(conn_fd, buffer, BUF_SIZE, 0);
    printf("get %d bytes of normal data '%s'\n", ret, buffer);

    memset(buffer, '\0', BUF_SIZE);
    ret = recv(conn_fd, buffer, BUF_SIZE, MSG_OOB);
    printf("get %d bytes of normal data '%s'\n", ret, buffer);

    memset(buffer, '\0', BUF_SIZE);
    ret = recv(conn_fd, buffer, BUF_SIZE, 0);
    printf("get %d bytes of normal data '%s'\n", ret, buffer);

    close(conn_fd);
  }

  //关闭socket
  close(sock);

  return 0;
}