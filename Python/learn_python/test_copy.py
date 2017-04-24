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
print "id(a) = ", id(a)

b = a[:]
print "id(b) = ", id(b)

c = list(a)
print "id(c) = ", id(c)

d = copy.copy(a)
print "id(d) = ", id(d)

e = copy.deepcopy(a)
print "id(e) = ", id(e)

a.append("abc")
foo.value = 88

print "original: %r\nslice:%r\nlist():%r\ncopy():%r\ndeepcopy():%r\n"%(a,b,c,d,e)
