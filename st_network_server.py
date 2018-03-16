#!/usr/bin/python2.7
# -*- coding:utf-8 -*-

import SocketServer
from time import ctime
import json
import os
import datetime

courseDict = {}
currentPath = ''

class Course:
    def __init__(self, courseID):
        self.courseID = courseID
        self.clientDict = {}
        self.finishList = []

def compare(x, y):
    stat_x = os.stat(currentPath + "/" + x)
    stat_y = os.stat(currentPath + "/" + y)
    if stat_x.st_mtime > stat_y.st_mtime:
        return -1
    elif stat_x.st_mtime < stat_y.st_mtime:
        return 1
    else:
        return 0

HOST = ''
PORT = 10001
ADDR = (HOST, PORT)

ROOT_PATH = './file'

class Client(SocketServer.BaseRequestHandler):
    role = ''
    courseID = 0
    bigData = ''

    def getFolders(self, path, result):
        print path
        paths = os.listdir(path)
        for i, item in enumerate(paths):
            sub_path = os.path.join(path, item)
            data = {}
            data['name'] = item

            if os.path.isdir(sub_path):
                data['child'] = []
                self.getFolders(sub_path, data['child'])
                result.append(data)

    def readDirectory(self, path):
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

    def sendMessage(self, data):
        self.request.sendall('#*#' + data + '@%@')

    def sendHistoryMessage(self):
        #print courseDict[self.courseID].finishList
        finishList = courseDict[self.courseID].finishList
        for index in range(0, len(finishList)):
            self.request.sendall('#*#' + finishList[index] + '@%@')

    def boardcastMessage(self, data):
        #print courseDict
        clientDict = courseDict[self.courseID].clientDict
        #print clientDict
        for k in clientDict.keys():
            if k == self.client_address:
                continue
            #print k, clientDict[k]
            #print '---', data
            clientDict[k].sendall('#*#' + data + '@%@')

    def processData(self, data):
        print '--------------', data
        datas = json.loads(data)
        if datas['type'] == 'createClient':
            self.role = datas['data']['role']
        elif datas['type'] == 'createCourse':
            self.courseID = datas['data']['courseID']
            if courseDict.has_key(self.courseID):
                return
            course = Course(self.courseID)
            courseDict[self.courseID] = course
        elif datas['type'] == 'joinCourse':
            self.courseID = datas['data']['courseID']
            course = courseDict[self.courseID]
            course.clientDict[self.client_address] = self.request
            self.sendHistoryMessage()
        elif datas['type'] == 'setClientAuthority' or datas['type'] == 'finish':
            self.boardcastMessage(data)
            course = courseDict[self.courseID]
            for index in range(0, len(course.finishList)):
                #print '---', course.finishList[index]
                historyDatas = json.loads(course.finishList[index])
                #print 'historyDatas---', historyDatas
                #print 'datas---', datas
                if historyDatas.has_key('itemID') and datas.has_key('itemID') and historyDatas['itemID'] == datas['itemID'] and historyDatas['subtype'] == datas['subtype']:
                    course.finishList.remove(course.finishList[index])
                    break
            course.finishList.append(data)
        elif datas['type'] == 'realtime':
            self.boardcastMessage(data)
        elif datas['type'] == 'file':
            path = ROOT_PATH
            destPath = ROOT_PATH
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
            list = self.readDirectory(path)
            self.sendMessage('file_list:' + list)
        elif datas['type'] == 'folder' and datas['action'] == 'list':
            path = ROOT_PATH
            result = []
            self.getFolders(path, result)
            json_res = json.dumps(result)
            self.sendMessage('folder_list:' + json_res)
#{"type":"folder","action":"list"}
#{"type":"file","action":"upload","data":{"path":"/1/2","name":"xxx"}}
#{"type":"file","action":"list","data":{"path":"/1/2"}}
#{"type":"file","action":"new","data":{"path":"/1/2","name":"xxx"}}
#{"type":"file","action":"copy","data":{"path":"/2","name":"xxx","destPath":"/3"}}
#{"type":"file","action":"move","data":{"path":"/2","name":"xxx","destPath":"/3"}}
#{"type":"file","action":"del","data":{"path":"/2","name":"xxx"}}

    def handle(self):
        # 客户端登入后，记住
        print '...connected from:', self.client_address
        while True: 
            data = self.request.recv(1024 * 1024 * 10)
            #print data, 'from', self.client_address
            if len(data) == 0:
                course = courseDict[self.courseID]
                course.clientDict.pop(self.client_address)
                break
            if data.endswith('@%@'):
                if len(self.bigData) > 0:
                    data = self.bigData + data
                    self.bigData = ''
                data = data.replace('@%@', '')
                dataList = data.split('#*#')
                for index in range(0, len(dataList)):
                    if dataList[index]:
                        self.processData(dataList[index])
            else:
                self.bigData = self.bigData + data

if not os.path.exists(ROOT_PATH):
    cmd = 'mkdir -p %s' % (ROOT_PATH)
    os.system(cmd)

tcpServ = SocketServer.ThreadingTCPServer(ADDR, Client)
print 'waiting for connection...'
tcpServ.serve_forever()