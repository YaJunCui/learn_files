# coding:utf-8

from __future__ import division
import math

def quadratic_equation(a,b,c):
    delta = b*b  - 4*a*c
    if delta < 0:
        return False
    elif delta == 0:
        return -b / 2*a
    else:
        x1 = (-b + math.sqrt(delta))/(2*a)
        x2 = (-b - math.sqrt(delta))/(2*a)
        return x1,x2

if __name__ == "__main__":
    print "a quadratic equation: x^2 - 2x - 1 = 0"
    roots = quadratic_equation(1,-2, -1)
    if roots:
        print "the result is : ", roots
    else:
        print "this equation has no solution."
