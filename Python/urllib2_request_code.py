# coding:utf-8

import urllib2

req = urllib2.Request("https://github.com/YaJunCui/notes/tree/master/images")

response = urllib2.urlopen(req)

page = response.read()

print page