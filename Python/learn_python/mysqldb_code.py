# coding:utf-8

import MySQLdb

conn = MySQLdb.connect(host="127.0.0.1",user="root",passwd="cui",db="learn_mysql",port=3306)

cur = conn.cursor()

# cur.execute("insert into tbl(username,password,email) values (%s,%s,%s)", ("python","666666","python@gmail.com"))
# conn.commit()

cur.execute("select * from tbl")
lines = cur.fetchall()

for line in lines:
    print line

cur.close()
conn.close()