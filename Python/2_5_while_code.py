#! /usr/bin/env python
# coding:utf-8

import random

print "*************************************"
num = raw_input("请输入一个0到9的数字：\n")

if not num.isdigit():
    print "请输入一个整数"
elif int(num) < 0 or int(num) > 9:
    print "请输入一个0-9的数字！"
else:
    pass

num = int(num)
i = 0
while i < 4:
    xnum = random.randint(0,9)

    if num == xnum:
        print "恭喜你！猜中了"
        break
    elif num < xnum:
        print "你猜小了，正确答案是：{}，你还有{}次机会。".format(xnum, 3-i)
    else:
        print "你猜大了，正确答案是：{}，你还有{}次机会。".format(xnum, 3-i)
    
    i += 1
