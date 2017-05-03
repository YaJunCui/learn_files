#! /usr/bin/env python
# coding=utf-8
import math

# x = 10
# y = x**2 if x > 0 else float('nan')
# print y

# def capitalize_all(t):
#     res = []
#     for s in t:
#         res.append(s.capitalize())
#     return res

# def capitalize_all(t):
#     return [s.capitalize() for s in t]

# str_lst = ['acb','cui','hu','pan']
# print id(str_lst), str_lst
# lst2 = capitalize_all(str_lst)
# print id(lst2), lst2

# def signature(s):
#     """Returns the signature of this string.

#     Signature is a string that contains all of the letters in order.

#     s: string
#     """
#     # TODO: rewrite using sorted()
#     t = list(s)
#     t.sort()
#     t = ''.join(t)
#     return t

# line = "  abpp  ef"
# word = line.strip().lower()
# print word
# t = signature(word)
# print t

from collections import namedtuple
Point = namedtuple('Point', ['x', 'y'])

p = Point(5,8)

print p.x, p.y
