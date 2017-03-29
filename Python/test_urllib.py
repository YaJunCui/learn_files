# coding:utf-8

import urllib

# urllib.urlretrieve("https://github.com/YaJunCui/notes/blob/master/images/ftp_work_principle.png?raw=true","1.png")

# urllib.urlretrieve("https://github.com/YaJunCui/notes/blob/master/images/ftp_work_principle.png?raw=true")

def go(a, b, c):
    per = 100.0 * a * b / c
    if per > 100:
        per = 100
    print "%.2f%%" % per
url = "https://github.com/YaJunCui/notes/blob/master/images/git_common_commands.jpg?raw=true"
local = "F:\\Git\\learn_files\\Python\\gg.jpg"

urllib.urlretrieve(url, local, go)