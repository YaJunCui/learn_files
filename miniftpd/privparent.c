// Edit by cyj 2017-1-1

#include "privparent.h"
#include "sysutil.h"

void handle_parent(session_t *sess)
{
	char cmd;
	while (1)
	{
		read(sess->parent_fd, &cmd, 1);

		//处理内部命令

		//处理命令

	}
}