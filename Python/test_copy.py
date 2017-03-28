#!/usr/bin/env python
# coding=utf-8

import copy

class MyCopy(object):
    def __init__(self, value):
        self.value = value

    def __repr__(self):
        return str(self.value)

foo = MyCopy(7)

a = ["foo", foo]
print id(a)

b = a[:]
i
c = list(a)
d = copy.copy(a)
e = copy.deepcopy(a)

a.append("abc")
foo.value = 88

print "original: %r\nslice:%r\nlist():%r\ncopy():%r\ndeepcopy():%r\n"%(a,b,c,d,e)
