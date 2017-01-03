// Edit by cyj 2017-1-3

#include "parseconf.h"
#include "common.h"
#include "tunable.h"

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