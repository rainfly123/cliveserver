gcc -g -o http -DTEST -I../common/ -I../channel -I../media http.c ../common/core.c ../common/log.c ../common/util.c ../common/epoll.c ../common/tcp.c -lpthread
