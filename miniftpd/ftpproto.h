// Edit by cyj 2016-12-31

#ifndef __FTP_PROTO_H__
#define __FTP_PROTO_H__

#include "session.h"

typedef void (*CMD_HANDLER)(session_t *sess);
typedef struct ftpcmd              //命令与命令操作函数的映射
{
	const char *cmd;
	CMD_HANDLER cmd_handler;
	
} ftpcmd_t;

void handle_child(session_t *sess);
int list_common(void);

#endif // __FTP_PROTO_H__