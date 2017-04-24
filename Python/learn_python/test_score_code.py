# coding:utf-8

from __future__ import division

def average_score(scores):
    score_values = scores.values()
    sz = len(score_values)
    if sz == 0:
        return False
    else:
        return sum(score_values)/sz

def sort_score(scores):
    score_lst = [(scores[i],i) for i in scores]
    sort_lst = sorted(score_lst, reverse=True)          # 降序排列
    return [(val[1],val[0]) for val in sort_lst]

def max_score(scores):                                  # 返回值最大的所有元素
    lst = sort_score(scores)
    max_value = lst[0][1]
    return [(i[0],i[1]) for i in lst if i[1] == max_value]

def min_score(scores):
    lst = sort_score(scores)
    min_value = lst[len(lst)-1][1]
    return [(i[0],i[1]) for i in lst if i[1] == min_value]

if __name__ == "__main__":
    example_scores = {"goole":98, "facebook":99, "baidu":52, "alibaba":80, "yahoo":49, "IBM":70, "android":76, "apple":99, "amazon":99}

    ave = average_score(example_scores)
    print "the average score is : ", ave

    sor = sort_score(example_scores)
    print "list of the scores : ", sor

    xueba = max_score(example_scores)
    print "Xueba is : ", xueba

    xuezha = min_score(example_scores)
    print "xuezha is : ", xuezha
