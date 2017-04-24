# coding:utf-8

# name = raw_input("Input your name : ")
# age = raw_input("How old are you?")

# print "Your name is : ", name
# print "You are " + age + " years old."

# after_ten = int(age) + 10
# print "You will be " + str(after_ten) + " years old after ten years"


a = "xing:cui;ming:jun;"
tmp_a = a[:-1]
lst = tmp_a.split(";")
i = 1

for val in lst:
  s_val = val.split(":")
  for s in s_val:
    print s,
  print