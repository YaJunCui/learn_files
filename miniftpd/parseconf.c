// Edit by cyj 2017-1-3

#include "parseconf.h"
#include "common.h"
#include "tunable.h"
#include "str.h"

static BOOL_CONFIG parseconf_bool_array[] =
    {
        {"pasv_enable", &tunable_pasv_enable},
        {"port_enable", &tunable_port_enable},
        {NULL, NULL}};

static UINT_CONFIG parseconf_uint_array[] =
    {
        {"listen_port", &tunable_listen_port},
        {"max_clients", &tunable_max_clients},
        {"max_per_ip", &tunable_max_per_ip},
        {"accept_timeout", &tunable_accept_timeout},
        {"connect_timeout", &tunable_connect_timeout},
        {"idle_session_timeout", &tunable_idle_session_timeout},
        {"data_connection_timeout", &tunable_data_connection_timeout},
        {"local_umask", &tunable_local_umask},
        {"upload_max_rate", &tunable_upload_max_rate},
        {"download_max_rate", &tunable_download_max_rate},
        {NULL, NULL}};

static STR_CONFIG parseconf_str_array[] =
    {
        {"listen_address", &tunable_listen_address},
        {NULL, NULL}};

void parseconf_load_file(const char *path)
{
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    ERR_EXIT("fopen");
  }

  char setting_line[1024] = {0};
  while (fgets(setting_line, sizeof(setting_line), fp) != NULL)
  {
    if (strlen(setting_line) == 0 //判断配置行是否合法
        || setting_line[0] == '#' || str_all_space(setting_line))
    {
      continue;
    }

    str_trim_crlf(setting_line); //去除尾部的“\r\n”
    parseconf_load_setting(setting_line);
    memset(setting_line, 0, sizeof(setting_line)); //将setting_line清空，进入下一次循环
  }

  fclose(fp);
}

void parseconf_load_setting(const char *setting)
{
  while (isspace(*setting)) //去除左空格
  {
    ++setting;
  }

  char key[128] = {0};
  char value[128] = {0};

  str_split(setting, key, value, '='); //根据‘=’分割键值
  if (strlen(value) == 0)
  {
    fprintf(stderr, "missing value in config file for : %s\n", key);
    exit(EXIT_FAILURE);
  }

  {
    const STR_CONFIG *p_str_setting = parseconf_str_array;
    while (p_str_setting->p_setting_name != NULL)
    {
      if (strcmp(key, p_str_setting->p_setting_name) == 0)
      {
        const char **p_cur_setting = p_str_setting->p_variable;
        if (*p_cur_setting) //如果配置项已有值，释放掉
        {
          free((char*)*p_cur_setting);
        }

        *p_cur_setting = strdup(value); //将字符串拷贝到新建的位置处
        return;
      }
      ++p_str_setting;
    }
  }

  {
    const BOOL_CONFIG *p_bool_setting = parseconf_bool_array;
    while (p_bool_setting->p_setting_name != NULL)
    {
      if (strcmp(key, p_bool_setting->p_setting_name) == 0)
      {
        str_upper(value);
        if (strcmp(value, "YES") == 0 ||
            strcmp(value, "TRUE") == 0 ||
            strcmp(value, "1") == 0)
        {
          *(p_bool_setting->p_variable) = 1;
        }
        else if (strcmp(value, "NO") == 0 ||
                 strcmp(value, "FALSE") == 0 ||
                 strcmp(value, "0") == 0)
        {
          *(p_bool_setting->p_variable) = 0;
        }
        else
        {
          fprintf(stderr, "missing value in config file for : %s\n", key);
          exit(EXIT_FAILURE);
        }

        return;
      }
      ++p_bool_setting;
    }
  }

  {
    const UINT_CONFIG *p_uint_setting = parseconf_uint_array;
    while (p_uint_setting->p_setting_name != NULL)
    {
      if (strcmp(key, p_uint_setting->p_setting_name) == 0)
      {
        if (value[0] == '0') //八进制
        {
          *(p_uint_setting->p_variable) = str_octal_to_uint(value);
        }
        else
        {
          *(p_uint_setting->p_variable) = atoi(value);
        }
        return;
      }
      ++p_uint_setting;
    }
  }
}