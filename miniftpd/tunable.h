// Edit by cyj 2017-1-3
#ifndef __TUNABLE_H__
#define __TUNABLE_H__

// 以下变量用于保存配置文件中对应的配置项
extern int tunable_pasv_enable;
extern int tunable_port_enable;
extern unsigned int tunable_listen_port;
extern unsigned int tunable_max_clients;
extern unsigned int tunable_max_per_ip;
extern unsigned int tunable_accept_timeout;
extern unsigned int tunable_connect_timeout;
extern unsigned int tunable_idle_session_timeout;
extern unsigned int tunable_data_connection_timeout;
extern unsigned int tunable_local_umask;
extern unsigned int tunable_upload_max_rate;
extern unsigned int tunable_download_max_rate;
extern const char *tunable_listen_address;

#endif // __TUNABLE_H__
