#! /usr/bin/env python
# coding=utf-8

import tornado.web
import tornado.gen

from base import BaseHandler
import time

class SleepHandler(BaseHandler):
    @tornado.gen.coroutine
    def get(self):
        yield tornado.gen.Task(tornado.ioloop.IOLoop.instance().add_timeout, time.time()+17)
        self.render("sleep.html")

class SeeHandler(BaseHandler):
    def get(self):
        self.render("see.html")