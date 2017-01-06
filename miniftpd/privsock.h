// Edit by cyj 2017-1-6
#ifndef __PRIV_SOCK_H__
#define __PRIV_SOCK_H__

void priv_sock_init(session_t *sess);                      //初始化内部进程间通道
void priv_sock_close(session_t *sess);                     //
void priv_sock_set_parent_context(session_t *sess);
void priv_sock_set_child_context(session_t *sess);

#endif //  __PRIV_SOCK_H__