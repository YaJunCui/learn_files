#! /usr/bin/env python
# coding=utf-8

import tornado.escape
import methods.readdb as mrd
from base import BaseHandler

class IndexHandler(BaseHandler):                # 继承 base.py 中的类 BaseHandler
    def get(self):
        usernames = mrd.select_columns(table="users", column="username")
        one_user = usernames[0][0]
        self.render("index.html",user=one_user)

    def post(self):
        username = self.get_argument("username")
        password = self.get_argument("password")
        user_infos = mrd.select_table(table="users",column="*",condition="username",value=username)
        if user_infos:
            db_pwd = user_infos[0][2]
            if db_pwd == password:
                self.set_current_user(username) # 将当前用户名写入cookie
                self.write(username)
            else:
                self.write("-1")
        else:
            self.write("-1")
    
    def set_current_user(self, user):
        if user:
            # 注意：这里使用了tornado.escape.json_encode()方法
            self.set_secure_cookie('user', tornado.escape.json_encode(user))
        else:
            self.clear_cookie("user")

class ErrorHandler(BaseHandler):
    def get(self):
        self.render("error.html")