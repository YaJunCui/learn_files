#! /usr/bin/env python
# coding:utf-8

print "请输入一个整型数字："

num = int(raw_input())     # 通过raw_input()传入的参数是字符串，需要进行转化

if num == 10:
    print "你输入的数字是 %d" % num
    print "you are smart."
elif num > 10:
    print "你输入的数字是 %d。" % num
    print "This number is more than 10."
elif num < 10:
    print "你输入的数字是 %d。" % num
    print "This number is less than 10."
else:
    print "Are you human?"