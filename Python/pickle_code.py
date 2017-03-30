# coding:utf-8

import pickle

integers = [1,2,3,4]
f = open("pickle_test0.dat","wb")
pickle.dump(integers,f)
f.close()

d = {}

integers = range(9999)
d["i"] = integers

# f = open("pickle_test1.dat","wb")
# pickle.dump(d,f)
# f.close()

# f = open("pickle_test2.dat","wb")
# pickle.dump(d,f,True)
# f.close()

integers = pickle.load(open("pickle_test0.dat","rb"))
print integers