#!/usr/bin/python
import subprocess
import time, os, sys, re

THREADS_PER_STREAM = 3

"""
14423 pts/0    R      1:04 ffmpeg -re -i ../../../LoadTest/FightClub.avi -t 00:10:00 -f flv -ar 11025 rtmp://71.96.94.69/rtmp/testa00000000
14424 pts/0    S      1:05 ffmpeg -re -i ../../../LoadTest/FightClub.avi -t 00:10:00 -f flv -ar 11025 rtmp://71.96.94.69/rtmp/testa00000001


15266 pts/0    S      0:02 ffmpeg -re -i ../../../LoadTest/FightClub.avi -t 00:10:00 -f flv -ar 11025 rtmp://71.96.94.69/rtmp/testa00000000
15267 pts/0    R      0:02 ffmpeg -re -i ../../../LoadTest/FightClub.avi -t 00:10:00 -f flv -ar 11025 rtmp://71.96.94.69/rtmp/testa00000001
15276 pts/0    S      0:00 ffmpeg -re -i rtmp://71.96.94.69/rtmp/testa00000000 -f flv -
15277 pts/0    R      0:00 ffmpeg -re -i rtmp://71.96.94.69/rtmp/testa00000001 -f flv -

"""

send_re = re.compile('rtmp://\d+\.\d+\.\d+\.\d+/rtmp/testa(\d+)\n')
recv_re = re.compile('rtmp://\d+\.\d+\.\d+\.\d+/rtmp/testa(\d+) -f flv -\n')

def get_streams():
    cmd = 'ps ax | grep ffmpeg'
    instances = subprocess.check_output(cmd, shell = True)
    (send, recv) = send_re.findall(instances, re.M|re.S), recv_re.findall(instances, re.M|re.S)
    return (send, recv)
    

def ffthread(stream, index):
    out = file(os.devnull, 'w')
    log = file('recvff-%s-%02d.log'%(stream, index), 'w')
    cmd = 'ffmpeg -re -i rtmp://71.96.94.69/rtmp/testa%s -f flv -'%(stream)
    print cmd
    pid = subprocess.Popen(cmd.split(), stdout=out, stderr=log).pid


def number_of_recv(send_id, recv_streams):
    number_of = 0
    for i in recv_streams:
        if (send_id == i):
            number_of += 1
    return number_of


while(1):
    send_streams, recv_streams = get_streams()
    for send_id in send_streams:
        recv_n = number_of_recv(send_id, recv_streams)
        if (recv_n < THREADS_PER_STREAM):
            for i in range(THREADS_PER_STREAM-recv_n):
                ffthread(send_id, i)
    time.sleep(60)

