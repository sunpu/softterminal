#!/usr/bin/python2.7
# -*- coding:utf-8 -*-

import sys
import zmq
import json
import os
import datetime
from time import ctime
from mdwrkapi import MajorDomoWorker
from db import Db

FILE_ROOT_PATH = './sourcefile/'
worker = None
reply = None
context = None
publisher = None
currentPath = ''

def compare(x, y):
    global currentPath
    stat_x = os.stat(currentPath + '/' + x)
    stat_y = os.stat(currentPath + '/' + y)
    if stat_x.st_mtime > stat_y.st_mtime:
        return -1
    elif stat_x.st_mtime < stat_y.st_mtime:
        return 1
    else:
        return 0

def getFolders(path, result):
    print path
    paths = os.listdir(path)
    for i, item in enumerate(paths):
        sub_path = os.path.join(path, item)
        data = {}
        data['name'] = item

        if os.path.isdir(sub_path):
            data['child'] = []
            getFolders(sub_path, data['child'])
            result.append(data)

def readDirectory(path):
    global currentPath
    currentPath = path
    result = []
    dirResult = []
    fileResult = []
    paths = os.listdir(path)
    paths.sort(compare)
    for i, item in enumerate(paths):
        sub_path = os.path.join(path, item)
        data = {}
        data['name'] = item
        timestamp = os.path.getmtime(sub_path)
        date = datetime.datetime.fromtimestamp(timestamp)
        data['time'] = date.strftime('%Y-%m-%d %H:%M:%S')

        if os.path.isdir(sub_path):
            data['type'] = 'folder'
            data['size'] = '-'
            dirResult.append(data)
        else:
            data['type'] = 'file'
            fsize = os.path.getsize(sub_path)
            data['size'] = str(fsize) + 'B'
            if fsize > 1024:
                fsize = fsize / float(1024)
                data['size'] = str(round(fsize,2)) + 'KB'
            if fsize > 1024:
                fsize = fsize / float(1024)
                data['size'] = str(round(fsize,2)) + 'MB'
            if fsize > 1024:
                fsize = fsize / float(1024)
                data['size'] = str(round(fsize,2)) + 'GB'
            fileResult.append(data)
    result = dirResult + fileResult
    json_res = json.dumps(result)
    return json_res

def processMessage(data):
    global publisher
    datas = json.loads(data)
    if datas['type'] == 'course':
        courseID = datas['courseID']
        if datas['action'] == 'new':
            # 插course表、wbitem表
            admin = datas['admin']
            sqlRet = Db.execute('select * from T_COURSE_INFO where COURSE_ID="%s";' % courseID)
            if len(sqlRet) == 0:
                Db.execute('insert into T_COURSE_INFO values ("%s", "%s");' % (courseID, admin))
        elif datas['action'] == 'query':
            # 查询course表
            sqlRet = Db.execute('select ADMIN from T_COURSE_INFO where COURSE_ID="%s";' % courseID)
            if len(sqlRet) == 1:
                return str(sqlRet[0][0])
            else:
                return ''
        elif datas['action'] == 'del':
            # 删course表、wbitem表
            Db.execute('delete from T_COURSE_INFO where COURSE_ID="%s";' % courseID)
            Db.execute('delete from T_COURSE_CONTENT where COURSE_ID="%s";' % courseID)
        elif datas['action'] == 'join':
            # 读取wbitem表，返回历史数据
            sqlRet = Db.execute('select CONTENT from T_COURSE_CONTENT where COURSE_ID="%s";' % courseID)
            for i in range(0, len(sqlRet)):
                content = str(sqlRet[i][0])
                content = content.replace('\\\'', '\"')
                print content
                publisher.send_multipart([str(courseID), content])
        else:
            pass
    elif datas['type'] == 'wbitem':
        courseID = datas['courseID']
        # 实时广播
        publisher.send_multipart([str(courseID), str(data)])
        data = data.replace('\"', '\\\'')
        if datas['subtype'] == 'add':
            id = datas['id']
            # 插wbitem表
            Db.execute('insert into T_COURSE_CONTENT values ("%s", "%s", "%s");' % (id, courseID, data))
        elif datas['subtype'] == 'move':
            id = datas['id']
            # 修改wbitem表
            Db.execute('insert into T_COURSE_CONTENT values ("%s", "%s", "%s");' % (id, courseID, data))
        elif datas['subtype'] == 'del':
            ids = datas['ids']
            # 删除wbitem表
            for i in range(0, len(ids)):
                Db.execute('delete from T_COURSE_CONTENT where ID="%s";' % ids[i])
    elif datas['type'] == 'wbrealtime':
        # 实时广播
        courseID = datas['courseID']
        publisher.send_multipart([str(courseID), str(data)])
    elif datas['type'] == 'file':
        user = datas['user']
        path = FILE_ROOT_PATH + user
        destPath = FILE_ROOT_PATH + user
        if not os.path.exists(path):
            cmd = 'mkdir -p %s' % (path)
            os.system(cmd)
        if datas['action'] == 'list':
            path += datas['data']['path']
        elif datas['action'] == 'new':
            path += datas['data']['path']
            name = datas['data']['name']
            cmd = 'cd %s;mkdir %s;' % (path, name)
            os.system(cmd)
        elif datas['action'] == 'copy':
            path += datas['data']['path']
            name = datas['data']['name']
            destPath += datas['data']['destPath']
            cmd = 'cp -rf %s/%s %s;' % (path, name, destPath)
            os.system(cmd)
        elif datas['action'] == 'move':
            path += datas['data']['path']
            name = datas['data']['name']
            destPath += datas['data']['destPath']
            cmd = 'mv -f %s/%s %s;' % (path, name, destPath)
            os.system(cmd)
        elif datas['action'] == 'del':
            path += datas['data']['path']
            name = datas['data']['name']
            cmd = 'rm -rf %s/%s;' % (path, name)
            os.system(cmd)
        print path
        list = readDirectory(path)
        return 'file_list:' + list
    elif datas['type'] == 'folder' and datas['action'] == 'list':
        user = datas['user']
        path = FILE_ROOT_PATH + user
        if not os.path.exists(path):
            cmd = 'mkdir -p %s' % (path)
            os.system(cmd)
        result = []
        getFolders(path, result)
        print result
        json_res = json.dumps(result)
        return 'folder_list:' + json_res
    return ''

def main():
    global publisher
    worker = MajorDomoWorker('tcp://localhost:5555', 'ST_MESSAGE_SERVER', False)
    reply = None

    context = zmq.Context.instance()
    publisher = context.socket(zmq.PUB)
    publisher.bind('tcp://*:5556')

    while True:
        request = worker.recv(reply)
        print '--------------', request
        if request is None:
            continue
        result = processMessage(request[0])
        print result
        reply = [result]

if __name__ == '__main__':
    main()
