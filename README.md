cliveserver
===========
 This is a open source software, its objective is to remux input stream , as to support playback
on multi-platform, for example , android platform, ios platform, windows platform.
 Its functionalities are similar to crtmpserver , but you will find cliveserver is very useful in reality,
besides, cliveserver is for free completely. you could use it in your commerical software without any worry.
Input and Output
=========
  The input format could be TS over TCP, FLV over TCP, output could be HLS segment, HDS segment
Http+TS, Http+FLV and RTMP.
 If you want to use rtmp out, you should install nginx-rtmp-module, cliveserver just demux the input
stream ,then send it out to nginx-rtmp server.
 If you want to user rtmp input, you should install rtmpdump firstly. 
About Config File
==================
It's json format, but not strict compliance with the json's definition,
 {
   channels: [
     {
       channel_name: "cnc", 
       input_url: "tcp://127.0.0.1:9090",
       output:["http://127.0.0.1:80/cnc_ts", 
              "http://127.0.0.1:80/cnc_flv"]
      },
      {
       channel_name: "tvb",
       input_url: "tcp://127.0.0.1:9090",
       output:["http://127.0.0.1:80/tvb_ts", 
              "http://127.0.0.1:80/tvb_flv",
              "http://127.0.0.1:80/tvb.m3u8"]
      }
    
    ]
  }
  channels: is a list containing all of the channel,
  channel_name: is the channel's unique name , 
  input_url: channel's input address , if protocol's tcp or udp , the cliveserver is the server side
  else cliveserver is the client side.
  output: channels's output protocol.
