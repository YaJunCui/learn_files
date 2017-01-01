//Edit by cyj 2017-1-1

#include "str.h"
#include "common.h"

void str_trim_crlf(char *str)                 //去除字符串尾部的\r\n
{
	char *p = &str[strlen(str)-1];
	while (*p == '\r' || *p == '\n')
	{
		*p-- = '\0';
	}
}

void str_split(const char *str , char *left, char *right, char c)
{
	char *p = strchr(str, c);
	if(p == NULL)                       //没有 c 分隔符(该项目为空格)，则说明该命令没有参数
	{
		strcpy(left, str);
	}
	else
	{
		strncpy(left, str, p-str);
		strcpy(right, p+1);		
	}
}

int str_all_space(const char *str)      //判断所有的字符是否全为空白字符
{
	while (*str)
	{
		if(!isspace(*str))
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
	return 0;	
}

unsigned int str_octal_to_uint(const char *str)
{
	return 0;	
}
