# coding:utf-8

"""
learning
"""

import random

xnum = random.randint(0,9)

i = 0
while i < 4:
    print "*************************************"
    num = raw_input("请输入一个0到9的数字：\n")

    if not num.isdigit():
        print "请输入一个整数"
        continue
    elif int(num) < 0 or int(num) > 9:
        print "请输入一个0-9的数字！"
        continue
    else:
        pass

    if int(num) == xnum:
        print "恭喜你！猜中了"
        break
    elif int(num) < xnum:
        print "你猜小了，你还有{}次机会。".format(xnum, 3-i)
    else:
        print "你猜大了，你还有{}次机会。".format(xnum, 3-i)

    i += 1
