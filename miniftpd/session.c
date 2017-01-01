//Edit by cyj 2016-12-31

#include "session.h"

void begin_session(session_t *sess)
{
	int ret = 0;
	int sockfds[2];
	ret = socketpair(PF_UNIX, SOCK_STREAM, 0, sockfds);	 //创建套接字对，用于父子进程间的通信
	if(ret < 0)
	{
		ERR_EXIT("socketpair");
	}

	pid_t pid;
	pid = fork();
	if(pid < 0)
	{
		ERR_EXIT("fork");
	}
	
	if(pid == 0)           //ftp服务进程
	{
		close(sockfds[0]);   //ftp服务进程使用 sockfds[1] 与 nobody 进程进行通信
		sess->child_fd = sockfds[1];
		handle_child(sess);
	}
	else                   //nobody进程
	{
		close(sockfds[1]);   //nobody进程使用 sockfds[0] 与ftp服务进程进行通信
		sess->parent_fd = sockfds[0];
		handle_parent(sess);
	}
}