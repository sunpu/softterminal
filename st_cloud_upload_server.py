#!/usr/bin/python2.7
# -*- coding:utf-8 -*-

import SocketServer

HOST = ''
PORT = 10002
ADDR = (HOST, PORT)

ROOT_PATH = './file'

class STCloudUploadServer(SocketServer.BaseRequestHandler):
    filePath = ''
    desFile = None
    total = 0

    def processData(self, data):
        #print '--------------', data
        if len(self.filePath) == 0:
            data = data.replace('#*#', '')
            data = data.replace('@%@', '')
            self.filePath = ROOT_PATH + data
            self.desFile = open(self.filePath, 'wb+')
        elif data.endswith('#*#finish@%@') and self.desFile is not None:
            print '----------------------end'
            data = data.replace('#*#finish@%@', '')
            self.desFile.write(data)
            self.desFile.close()
            self.filePath = ''
        else:
            self.total += len(data)
            print self.total
            self.desFile.write(data)

    def handle(self):
        # 客户端登入后，记住
        print '...connected from:', self.client_address
        while True: 
            data = self.request.recv(512 * 1024)
            #print data, 'from', self.client_address
            if len(data) == 0:
                break
            self.processData(data)

tcpServ = SocketServer.ThreadingTCPServer(ADDR, STCloudUploadServer)
print 'waiting for connection...'
tcpServ.serve_forever()