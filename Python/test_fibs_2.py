# coding:utf-8

meno = {0:0, 1:1}

def fibs(n):
    if n not in meno:
        meno[n] = fibs(n-1) + fibs(n-2)
    return meno[n]

if __name__ == "__main__":
    f = fibs(10)
    print f