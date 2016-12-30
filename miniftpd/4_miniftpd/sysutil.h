// Edit by cyj 2016-12-30
#ifndef __SYS_UTIL_H__
#define __SYS_UTIL_H__

int getlocalip(char* ip);                          //获取本机ip地址

void activate_nonblock(int fd);                    //设置文件描述fd为非阻塞模式                   
void deactivate_nonblock(int fd);                  //去掉文件描述fd的非阻塞模式                     

//int read_timeout(int fd, unsigned int wait_seconds);  
//int write_timeout(int fd, unsigned int wait_seconds);
int accept_timeout(int fd, struct sockaddr_in* addr, unsigned int wait_seconds);      //接受超时函数
int connect_timeout(int fd, struct sockaddr_in* addr, unsigned int wait_seconds);     //连接超时函数

ssize_t readn(int fd, void* buf, size_t count);
ssize_t writen(int fd, const void* buf, size_t count);
//ssize_t recv_peek(int sockfd, void* buf, size_t len);
ssize_t readline(int sockfd, void* buf, size_t maxline);

void send_fd(int sock_fd, int fd);                 //发送网络套接字
int recv_fd(const int sock_fd);                    //接收网络套接字

#endif  //__SYS_UTIL_H__