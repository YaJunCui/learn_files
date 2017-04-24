# coding:utf-8

s = "I am, writing\npython\tbook on line"

s1 = s.split()

s2 = []

for val in s1:
    if "," in val:
        s2.append(val[:-1])
    else:
        s2.append(val)

s2 = " ".join(s2)

print s2