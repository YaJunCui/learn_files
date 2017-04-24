# coding:utf-8

import xml.etree.cElementTree as ET

tree = ET.ElementTree(file="xml_code.xml")

root = tree.getroot()

print root.tag
print root.attrib

for child in root:
    print child.tag, child.attrib

print ""

for ele in tree.iter(tag="book"):
    print ele.tag,ele.attrib

print ""

for ele in tree.iter(tag="title"):
    print ele.tag, ele.attrib, ele.text

print ""