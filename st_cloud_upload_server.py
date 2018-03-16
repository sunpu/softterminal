#!/usr/bin/python2.7
# -*- coding:utf-8 -*-

import SocketServer
import threading
import os
import zipfile
import time

HOST = ''
PORT = 10002
ADDR = (HOST, PORT)

ROOT_PATH = './file'
CONVERT_PATH = './convert'

class STCloudUploadServer(SocketServer.BaseRequestHandler):
    filePath = ''
    convertPath = ''
    desFile = None
    total = 0
    convertThread = None
    downloadThread = None
    supportExt = ['.pptx', '.ppt', '.pptm', '.docx', '.doc', '.pdf']

    def downloadFileProcess(self, destFilePath):
        f = open(destFilePath, 'rb')
        total = 0
        while True:
            data=f.read(512 * 1024)
            if not data:
                break
            self.request.sendall(data)
            #while len(data)>0:
                #sent = self.request.sendall(data)
                #print sent
                #data=data[sent:]
                #total = total + sent
                #print '---', total
                #time.sleep(0.1)
        time.sleep(1)
        self.request.sendall('#*#finish@%@')
        print 'finish'
        f.close()

    def makeZip(self, source_dir, output_filename):
        zipf = zipfile.ZipFile(output_filename, 'w')
        pics = os.listdir(source_dir)
        for i, item in enumerate(pics):
            if item.endswith(".png"):
                picPath = os.path.join(source_dir, item)
                zipf.write(picPath, item)
        zipf.close()

    def convertFileProcess(self):
        (filepath, tempfilename) = os.path.split(self.filePath)
        (shortname, extension) = os.path.splitext(tempfilename)
        if extension.lower() in self.supportExt:
            cmd = 'mkdir -p %s' % (self.convertPath)
            print cmd
            os.system(cmd)
            if extension.lower() == '.pdf':
                cmd = 'cp -rf %s %s' % (self.filePath, self.convertPath)
                print cmd
                os.system(cmd)
            else:
                cmd = 'soffice --headless --convert-to pdf %s --outdir %s' % (self.filePath, self.convertPath)
                print cmd
                os.system(cmd)
            cmd = 'convert -density 200  %s/%s.pdf %s/%s.png' % (self.convertPath, shortname, self.convertPath, shortname)
            print cmd
            os.system(cmd)
            pics = os.listdir(self.convertPath)
            for i, item in enumerate(pics):
                if item.endswith(".png"):
                    cmd = 'convert -background white -flatten %s/%s %s/%s' % (self.convertPath, item, self.convertPath, item)
                    print cmd
                    os.system(cmd)
            cmd = 'rm -f %s/%s.pdf' % (self.convertPath, shortname)
            print cmd
            os.system(cmd)
            self.makeZip(self.convertPath, self.convertPath + '/' + tempfilename + '.zip')
            cmd = 'rm -f %s/*.png' % (self.convertPath)
            print cmd
            os.system(cmd)
        self.filePath = ''

    def processData(self, data):
        #print '--------------', data
        if data.startswith('#*#download#') and data.endswith('@%@'):
            # '#*#download#/path/xxx.ppt@%@'
            path = data.replace('#*#download#', '')
            path = path.replace('@%@', '')
            print path
            (filepath, tempfilename) = os.path.split(path)
            (shortname, extension) = os.path.splitext(tempfilename)
            print shortname, extension
            destFilePath = ''
            if extension.lower() in self.supportExt:
                destFilePath = os.path.join(CONVERT_PATH + path, tempfilename + '.zip')
            else:
                destFilePath = ROOT_PATH + path
            # 循环写文件
            print destFilePath
            self.downloadThread = threading.Thread(target = self.downloadFileProcess, args=(destFilePath,))
            self.downloadThread.start()
        elif len(self.filePath) == 0:
            data = data.replace('#*#', '')
            data = data.replace('@%@', '')
            self.filePath = ROOT_PATH + data
            self.convertPath = CONVERT_PATH + data
            print '--------------', self.filePath
            print '--------------', self.convertPath
            self.desFile = open(self.filePath, 'wb+')
        elif data.endswith('#*#finish@%@') and self.desFile is not None:
            print '----------------------end'
            data = data.replace('#*#finish@%@', '')
            self.desFile.write(data)
            self.desFile.close()
            self.convertThread = threading.Thread(target = self.convertFileProcess)
            self.convertThread.start()
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

if not os.path.exists(CONVERT_PATH):
    cmd = 'mkdir -p %s' % (CONVERT_PATH)
    os.system(cmd)

tcpServ = SocketServer.ThreadingTCPServer(ADDR, STCloudUploadServer)
print 'waiting for connection...'
tcpServ.serve_forever()