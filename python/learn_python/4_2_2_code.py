# coding:utf-8

__metaclass__ = type
class Person:
    x = [1,2,3]
    def __init__(self, name):
        self.name = name
        self.email = "644583255@qq.com"

    def get_name(self):
        return self.name

    def color(self, color):
         print "%s is %s" % (self.name, color)

cui = Person("cuiyajun")
cui.x.append("cui")

zhang = Person("zhang")
print zhang.name
print zhang.email
print zhang.x

print cui.name
print cui.x