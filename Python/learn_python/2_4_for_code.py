#! /usr/bin/env python
# coding:utf-8

# aliquot = []

# for val in range(1,100):
#     if val % 3 == 0:
#         aliquot.append(val)

# print aliquot

# a_dict = {"name":"cui","age":25,"address":"shenyang"}

# for k in a_dict.keys():
#     print k, a_dict[k]

# a = [1,2,3,4,5]
# b = [9,8,7,6,5]
# c = []

# for i in range(len(a)):
#     c.append(a[i]+b[i])

# print c

# d = []

# for x, y in zip(a,b):
#     d.append(x+y)

# print d

# yinfo = {"name":"cui","age":25,"address":"shenyang"}
# info = {}

# for k,v in yinfo.iteritems():
#     info[v] = k

# print info

# print dict(zip(yinfo.values(), yinfo.keys()))

# raw = "DO you love Java? Java is a goog tool."

# raw_list = raw.split()

# for i, s in enumerate(raw_list):
#     if "Java" in s:
#         raw_list[i] = "PHP"

# print " ".join(raw_list)

mybag = ["  cui  ", " ya", "jun"]
lst = [val.strip() for val in mybag]

print lst

