#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

int main()
{
  const char* my_ip = "192.168.131.45";

  int inp1 = inet_addr(my_ip);
  printf("inet_addr ip = %d\n",inp1);

  int inp2;
  printf("inet_aton ip = %d\n", inp2);
  inet_aton(my_ip,(struct in_addr*)&inp2);
  printf("inet_aton ip = %d\n", inp2);

  //struct in_addr addr1;
  //memcpy(&addr1, &inp1,4);
  char* str_value1 = inet_ntoa(*(struct in_addr*)&inp1);
  //char* str_value1 = inet_ntoa(addr1);
  printf("inet_ntoa str ip = %s\n", str_value1);

  return 0;
}