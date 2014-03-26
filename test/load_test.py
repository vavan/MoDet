#!/usr/bin/python
import subprocess
import time, os, sys

THREADS = 10


def get_streams():
    cmd = 'ps ax | grep ffmpeg | wc -l'
    number = subprocess.check_output(cmd, shell = True)
    return int(number) - 1


def ffthread(video, index):
    out = file('fftest%08d.log'%index, 'w')
    cmd = 'ffmpeg -i %s -t 00:10:00 -f flv -ar 11025 rtmp://71.96.94.69/rtmp/testa%08d'%(video, index)
    print cmd
    pid = subprocess.Popen(cmd.split(), stderr=out).pid



video = sys.argv[1]
index = 0
while(1):
    streams = get_streams()
    if (streams < THREADS/2):
        for i in range(THREADS-streams):
            ffthread(video, index)
            index += 1
    time.sleep(60)
    #print "Done!"
