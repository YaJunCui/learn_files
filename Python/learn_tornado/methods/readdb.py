#! /usr/bin/env python
# coding=utf-8

from db import *

def select_table(table, column, condition, value):
    sql = "select "+column+" from "+table+" where "+condition+"='"+value+"'"
    cur.execute(sql)                   # 执行sql语句
    lines = cur.fetchall()             # 返回所有查询的结果
    return lines

def select_columns(table, column):
    sql = "select "+column+" from "+table
    cur.execute(sql)
    lines = cur.fetchall()
    return lines
