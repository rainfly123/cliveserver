cliveserver
===========
 This is a open source software, its goal is to remux input stream , as to support playback
on multi-platform, for example , android platform, ios platform, windows platform.
Input and Output
=========
 The input format could be TS over TCP, FLV over TCP, output could be HLS segment, HDS segment
Http+TS, Http+FLV and RTMP.
 If you want to use rtmp , you should install nginx-rtmp-module, cliveserver just demux the input
stream ,then send it out to nginx-rtmp server.
