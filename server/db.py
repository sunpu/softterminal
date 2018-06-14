#!/usr/bin/python2.7
# -*- coding:utf-8 -*-

import sqlite3

class Db:
    db_file = './db.db'
    def __init__(self):
        pass
    @staticmethod
    def execute(sql):
        ret = ''
        cx = sqlite3.connect(Db.db_file)
        cx.text_factory = str
        cu = cx.cursor()
        cu.execute(sql)
        cx.commit()
        ret = cu.fetchall()
        cu.close()
        cx.close()
        return ret
