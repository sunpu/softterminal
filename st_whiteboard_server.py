#!/usr/bin/python2.7
# -*- coding:utf-8 -*-

import SocketServer
from time import ctime
import json

courseDict = {}

class Course:
    def __init__(self, courseID):
        self.courseID = courseID
        self.clientDict = {}
        self.finishList = []

HOST = ''
PORT = 10001
ADDR = (HOST, PORT)

ROOT_PATH = './file/'

class Client(SocketServer.BaseRequestHandler):
    role = ''
    courseID = 0
    bigData = ''

    def readDirectory(self, path):
        result = []
        paths = os.listdir(path)
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
            else:
                data['type'] = 'file'
                fsize = os.path.getsize(sub_path)
                fsize = fsize / float(1024)
                data['size'] = str(round(fsize,2)) + 'KB'
            result.append(data)
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
        #print '--------------', data
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
                cmd = 'cd %s;cp -rf %s %s;' % (path, name, destPath)
                os.system(cmd)
            elif datas['action'] == 'move':
                path += datas['data']['path']
                name = datas['data']['name']
                destPath += datas['data']['destPath']
                cmd = 'cd %s;mv -rf %s %s;' % (path, name, destPath)
                os.system(cmd)
            elif datas['action'] == 'del':
                path += datas['data']['path']
                name = datas['data']['name']
                cmd = 'cd %s;rm -rf %s;' % (path, name)
                os.system(cmd)
            list = self.readDirectory(path)
            self.sendMessage(list)
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

tcpServ = SocketServer.ThreadingTCPServer(ADDR, Client)
print 'waiting for connection...'
tcpServ.serve_forever()