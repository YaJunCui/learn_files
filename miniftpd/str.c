//Edit by cyj 2017-1-1

#include "str.h"
#include "common.h"

void str_trim_crlf(char *str) //去除字符串尾部的\r\n
{
  char *p = &str[strlen(str) - 1];
  while (*p == '\r' || *p == '\n')
  {
    *p-- = '\0';
  }
}

void str_split(const char *str, char *left, char *right, char c)
{
  char *p = strchr(str, c);
  if (p == NULL) //没有 c 分隔符(该项目为空格)，则说明该命令没有参数
  {
    strcpy(left, str);
  }
  else
  {
    strncpy(left, str, p - str);
    strcpy(right, p + 1);
  }
}

int str_all_space(const char *str) //判断所有的字符是否全为空白字符
{
  while (*str)
  {
    if (!isspace(*str))
    {
      return 0;
    }
    ++str;
  }
  return 1;
}

void str_upper(char *str)
{
  while (*str)
  {
    *str = toupper(*str);
    ++str;
  }
}

long long str_to_longlong(const char *str)
{
  long long result = 0;
  long long mult = 1;
  unsigned int len = strlen(str);
  unsigned int i;

  if (len > 15)
  {
    return 0;
  }

  for (i = 0; i < len; ++i)
  {
    char ch = str[len - (i + 1)];
    long long val = 0;
    if (ch < '0' || ch > '9')
    {
      return 0;
    }

    val = ch - '0';
    val *= mult;
    result += val;
    mult *= 10;
  }

  return result;
}

unsigned int str_octal_to_uint(const char *str)
{
  unsigned int result = 0;
  unsigned int len = strlen(str);
  unsigned int i = 0;

  for (i = 0; i < len; ++i)
  {
    if(str[i] < '0' || '7' < str[i])
    {
      return 0;
    }

    result <<= 3;
    result += str[i] - '0';
  }

  return result;
}
