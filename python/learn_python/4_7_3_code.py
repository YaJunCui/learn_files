# coding:utf-8

# class Rectangle(object):
#     def __init__(self, *args):
#         super(Rectangle, self).__init__(*args)
#         self.width = 0
#         self.lenght = 0

#     def set_size(self, size):
#         self.width, self.lenght = size

#     def get_size(self):
#         return self.width, self.lenght

#     size = property(get_size, set_size)

class NewRectangle(object):
    def __init__(self):
        self.width = 0
        self.lenght = 0

    def __setattr__(self, name, value):
        if name == "size":
            self.width, self.lenght = value
        else:
            self.__dict__[name] = value
    
    def __getattr__(self, name):
        if name == "size":
            return self.width, self.lenght
        else:
            raise AttributeError

if __name__ == "__main__":
    r = NewRectangle()
    r.width = 3
    r.lenght = 4
    print r.size

    r.size = 30, 40
    print r.width
    print r.lenght
