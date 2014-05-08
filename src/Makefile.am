MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -D_GNU_SOURCE
AM_CPPFLAGS += -I $(top_srcdir)/src/common
AM_CPPFLAGS += -I $(top_srcdir)/src/media
AM_CPPFLAGS += -I $(top_srcdir)/src/channel
AM_CPPFLAGS += -I $(top_srcdir)/src/http

AM_CFLAGS =
AM_CFLAGS += -Wall -Wshadow
AM_CFLAGS += -Wpointer-arith
AM_CFLAGS += -Winline
AM_CFLAGS += -Wunused-function -Wunused-variable -Wunused-value
AM_CFLAGS += -Wno-unused-parameter -Wno-unused-value
AM_CFLAGS += -Wconversion -Wsign-compare
AM_CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wredundant-decls -Wmissing-declarations

AM_LDFLAGS =
#AM_LDFLAGS += -lrt -lpthread -rdynamic
#cliveserver_LDADD = 

sbin_PROGRAMS = cliveserver

cliveserver_SOURCES =  \
            common/core.c \
            common/epoll.c \
            common/jsmn.c \
            common/kfifo.c \
            common/list.c \
            common/log.c \
            common/tcp.c  \
            common/timer.c \
            common/udp.c \
            common/util.c \
            media/ts2ts.c \
            media/media.c \
            channel/channel.c \
            http/http.c \
            main.c 


