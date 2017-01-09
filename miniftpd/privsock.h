// Edit by cyj 2017-1-6
#ifndef __PRIV_SOCK_H__
#define __PRIV_SOCK_H__
#include "session.h"

// 内部进程自定义协议
// 用于 FTP 服务进程与 nobody 进程进行通信

// FTP 服务进程向 nobody 进程请求的命令
#define PRIV_SOCK_GET_DATA_SOCK 1
#define PRIV_SOCK_PASV_ACTIVE   2
#define PRIV_SOCK_PASV_LISTEN   3
#define PRIV_SOCK_PASV_ACCEPT   4

// nobody 进程对 FTP 服务进程的应答
#define PRIV_SOCK_RESULT_OK     1
#define PRIV_SOCK_RESULT_BAD    2

void priv_sock_init(session_t *sess);                   //初始化内部进程间通道
void priv_sock_close(session_t *sess);                  //关闭内部进程间通信通道
void priv_sock_set_parent_context(session_t *sess);     //设置父进程环境
void priv_sock_set_child_context(session_t *sess);      //设置子进程环境


void priv_sock_send_cmd(int fd, char cmd);              //发送命令（子->父）
char priv_sock_get_cmd(int fd);                         //接收命令（父->子）
void priv_sock_send_result(int fd, char res);           //发送结果（父->子）
char priv_sock_get_result(int fd);                      //接收结果（子->父）

#endif //  __PRIV_SOCK_H__