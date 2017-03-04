#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libgen.h>

int main(int argc, char* argv[])
{
  if(argc <= 2)
  {
    printf("Usage: %s ip_address port_number\n", basename(argv[0]));
    return -1;
  }

  const char* ip = argv[1];
  int port = atoi(argv[2]);

  int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock_fd >= 0);

  struct sockaddr_in server_addr;
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  int ret = connect(sock_fd, (struct sockaddr*)&server_addr,
                    sizeof(server_addr));
  if(ret == -1)
  {
    printf("connection failed\n");
  }
  else
  {
    const char* oob_data = "abc";
    const char* normal_data = "123";

    send(sock_fd, normal_data, strlen(normal_data), 0);
    send(sock_fd, oob_data, strlen(oob_data), MSG_OOB);
    send(sock_fd, normal_data, strlen(normal_data), 0);
  }

  close(sock_fd);

  return 0;
}