# coding:utf-8

"""
test return
"""

def fibs(num):
    """
    fibs
    """
    result = [1, 1]
    for i in range(num-2):
        result.append(result[-2]+result[-1])
    return result

if __name__ == "__main__":
    lst = fibs(10)
    print lst
