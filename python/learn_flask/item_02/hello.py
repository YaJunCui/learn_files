#! /usr/bin/env python
# coding=utf-8

from flask import Flask
from flask_script import Manager

app = Flask(__name__)
manager = Manager(app)


@app.route('/')
def index():
    return "<h2>Hello World!</h2>"

@app.route('/user/<name>')
def user(name):
    return '<h1>Hello, %s</h1>' % name

if __name__ == '__main__':
    manager.run()
