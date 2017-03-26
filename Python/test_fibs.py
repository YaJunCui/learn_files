# coding:utf-8

def fibs(n):
    if n == 0:
        return 0
    elif n == 1:
        return 1
    else:
        return fibs(n-1)+fibs(n-2)

if __name__ == "__main__":
    f = fibs(10)
    print f