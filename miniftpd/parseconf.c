// Edit by cyj 2017-1-3

#include "parseconf.h"
#include "common.h"
#include "tunable.h"

static BOOL_CONFIG parseconf_bool_array[] =
{
	{ "pasv_enable", &tunable_pasv_enable },
	{ "port_enable", &tunable_port_enable },
	{ NULL, NULL }
};

static UINT_CONFIG parseconf_uint_array[] =
{
	{ "listen_port", &tunable_listen_port },
	{ "max_clients", &tunable_max_clients },
	{ "max_per_ip", &tunable_max_per_ip },
	{ "accept_timeout", &tunable_accept_timeout },
	{ "connect_timeout", &tunable_connect_timeout },
	{ "idle_session_timeout", &tunable_idle_session_timeout },
	{ "data_connection_timeout", &tunable_data_connection_timeout },
	{ "local_umask", &tunable_local_umask },
	{ "upload_max_rate", &tunable_upload_max_rate },
	{ "download_max_rate", &tunable_download_max_rate },
	{ NULL, NULL }
};

static STR_CONFIG parseconf_str_array[] =
{
	{ "listen_address", &tunable_listen_address },
	{ NULL, NULL }
};

void parseconf_load_file(const char *path)
{
  FILE* fp = fopen(path, "r");
  if(fp == NULL)
  {
    ERR_EXIT("fopen");
  }

  char setting_line[1024] = {0};
  while (fgets(setting_line, sizeof(setting_line), fp) != NULL)
  {
    if(strlen(setting_line) == 0                       //判断配置行是否合法
       || setting_line[0] == '#'
       || str_all_space(setting_line))
    {
      continue;
    }

    str_trim_crlf(setting_line);                      //去除尾部的“\r\n”
    parseconf_load_setting(setting_line);
    memset(setting_line, 0, sizeof(setting_line));    //将setting_line清空，进入下一次循环
  }

  close(fp);
}

void parseconf_load_setting(const char *setting)
{

}