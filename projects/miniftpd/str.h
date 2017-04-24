//Edit by cyj 2017-1-1

#ifndef __STR_H__
#define __STR_H__

void str_trim_crlf(char *str); //去除字符串尾部的\r\n
void str_split(const char *str, char *left, char *right, char c);
int str_all_space(const char *str); //判断所有的字符是否全为空白字符
void str_upper(char *str);
long long str_to_longlong(const char *str);
unsigned int str_octal_to_uint(const char *str);

#endif // __STR_H__
