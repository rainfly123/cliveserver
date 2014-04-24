/*
  * A generic http implementation
  *
  * Copyright (C) 2014 rainfly123 <xiechc@gmail.com>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
  *
  */
#include "tcp.h"
#include "log.h"
#include "util.h"
#include "event.h"
#include "list.h""
#include "http.h"
#include "core.h"
#include "kfifo.h"
#include "channel.h"

struct http {
    struct con connection;
    uint8_t  rbuffer[512];
    uint32_t rlen;
};


typedef struct http_task {
    List_t clients; //the clients watching the same channel
    struct kfifo *buffer; //the channels data ()
}HTTP_Task;

static List_t channels;  //store all channel name 

#define INTERVAL 1000
static void conn_close(struct con * conn)
{
    ASSERT(conn != NULL);
    struct http * http = (struct http *) conn->ctx;
    close(conn->skt);
    conn->done = true;
    //relase memeory
    ///releae tiemr
  /*************************************/
   //SIGNAL CLOSE
}

static int conn_recv(struct con *conn)
{
    ssize_t n;

    ASSERT(conn != NULL);
    struct http * http = (struct http *) conn->ctx;

    for (;;) {
        n = clive_read(conn->skt, (http->rbuffer + http->rlen),\
                   (sizeof(http->rbuffer) - http->rlen));
        log_debug(LOG_VERB, "recv on sd %d %zd %s ", conn->skt, n, http->rbuffer);
        if (n > 0) {
            http->rlen += (uint32_t)n;
            log_debug(LOG_VERB, "recv on sd return  ");
            if (http->rlen == sizeof(http->rbuffer)) {
                memset(http->rbuffer, 0, sizeof(http->rbuffer));
                http->rlen = 0;
            }
            return CL_OK;
        }

        if (n == 0) {
            log_debug(LOG_INFO, "recv on sd %d eof rb %d", conn->skt,
                      http->rlen);
            return CL_CLOSE;
        }

        if (errno == EINTR) {
            log_debug(LOG_VERB, "recv on sd %d not ready - eintr", conn->skt);
            continue;
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            log_debug(LOG_VERB, "recv on sd %d not ready - eagain", conn->skt);
            return CL_OK;
        } else {
            conn->err = errno;
            log_error("recv on sd %d failed: %s", conn->skt, strerror(errno));
            return CL_ERROR;
        }
    }

    NOT_REACHED();
    return CL_ERROR;
}

static int conn_send(struct con *conn)
{
}


/*
http://192.168.1.13:80803/onlive
*/
struct http * clive_http_server_new(struct event_base *evb, int port)
{
    struct http *http;
    int skt;

    ASSERT(evb != NULL);
    http = clive_calloc(1, sizeof(struct http));
    ASSERT(http != NULL);
   

    skt = clive_tcp_socket();
    clive_set_nonblocking(skt);
    clive_set_sndbuf(skt, 64*1024);
    clive_set_rcvbuf(skt, 64*1024);
    clive_set_tcpnodelay(skt);
    clive_tcp_bind(skt, "0.0.0.0", 80);
    clive_tcp_listen(skt, 100);

    http->connection.skt = skt;
    http->connection.type = tHTTP;
    http->connection.evb = evb;
    http->connection.send = &conn_send;
    http->connection.recv = &conn_recv;
    http->connection.close = &conn_close;
    http->connection.ctx = http;
    
    event_add_conn(evb, &http->connection);

    return http;
}

static void * Entry(void *p)
{
    HTTP_Task * task;
    ListEntry_t * current = NULL;
   
    do {
        if (current == NULL) {
            current = channels.head ;
            pthread_mutex_unlock(&lock);
            usleep(10 *1000);
            log_debug(LOG_INFO, "all task done");
            continue;
        }
        task = current->data;
        //do something

        pthread_mutex_lock(&lock);
        current = current ? current->next : NULL;
        pthread_mutex_unlock(&lock);
    }while(1);
}

static void * event_Entry(void *p)
{
    struct event_base *evb;
    int nsd;

    evb  = event_base_create(EVENT_SIZE, &clive_core_core);

    while (1) {
        nsd = event_wait(evb, 300);
        if (nsd == 0) {
            log_debug(LOG_INFO, "wait return %d", nsd);
        }
        if (nsd < 0) {
            log_debug(LOG_INFO, "wait error");
            return nsd;
        }
    }
}
/*
  start the repacking task thread
*/

int clive_http_server_start(void)
{
    pthread_t tid;
    pthread_attr_t attr;
    int val;

    pthread_attr_init(&attr); 
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    val = pthread_create(&tid, &attr, Entry, NULL);
    val += pthread_create(&tid, &attr, event_Entry, NULL);
    return val;
}

//no need to stop i think
int clive_http_server_stop(void)
{
}


