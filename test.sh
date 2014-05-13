ffmpeg -f video4linux2 -i /dev/video0 -f alsa -i hw:0 -ac 1 -vcodec libx264 -bf 0 -s 640x480 -acodec libfaac -f mpegts udp://127.0.0.1:9090
