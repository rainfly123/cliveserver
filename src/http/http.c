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
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include "tcp.h"
#include "log.h"
#include "util.h"
#include "event.h"
#include "list.h"
#include "http.h"
#include "core.h"
#include "kfifo.h"
#include "channel.h"
#include "media.h"

/*global definition*/
const char * flv_head = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: video/x-flv\r\nServer: cliveserver/0.1\r\n\r\n";
const char *ts_head = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: video/MP2T\r\nServer: cliveserver/0.1\r\n\r\n";
const char *error_head = "HTTP/1.1 404 NOT FOUND\r\nContent-Type:text/html\r\nContent-Length:56\r\nConnection:Keep-Alive\r\n\r\n<html><body><center>404 Not Found</center></body></html>";
#define HTTP_PORT 8080

struct http * clive_http_server_new(struct event_base *evb, unsigned short port);

struct http {
    struct con connection;
};

struct http_client {
    struct con connection;
    char rbuffer[512];
    uint32_t rlen;
};

typedef struct http_task {
    List_t clients; //the clients watching the same channel
    struct kfifo *buffer; //the channels data ()
    char channel_name[48]; //channel name with _ts or _flv suffix
}HTTP_Task;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static List_t channels;  //store all channel, in the form of http task 

static void http_add_task(char *name)
{
    HTTP_Task *task = clive_calloc(1, sizeof(HTTP_Task));
    strcpy(task->channel_name, name);
    task->buffer = kfifo_alloc(128 * 1024);
}

static void http_add_client(char *name, int fd)
{
    ListIterator_t iterator;
    HTTP_Task *p;
    int * data = clive_calloc(1, sizeof(int));
    *data = fd;
    int val;

    ListIterator_Init(iterator, &channels); 
    for ( ; ListIterator_MoreEntries(iterator); ListIterator_Next(iterator))
    {
        p = ListIterator_Current(iterator);
        val = strncmp(p->channel_name, name, strlen(p->channel_name));
        if (val == 0) {
            ListAdd(&p->clients, data);
        }
    }
}

static bool http_task_is_existed(const char *name)
{
    ListIterator_t iterator;
    HTTP_Task *p;
    int val;

    if (name == NULL) {
        return false;
    }

    ListIterator_Init(iterator, &channels); 

    for ( ; ListIterator_MoreEntries(iterator); ListIterator_Next(iterator))
    {
        p = ListIterator_Current(iterator);
        val = strncmp(p->channel_name, name, strlen(p->channel_name));
        if (val == 0)
            return true;
    }
    return false;
}

static HTTP_Task * http_task_find(const char *name)
{
    ListIterator_t iterator;
    HTTP_Task *p;
    int val;

    if (name == NULL) {
        return false;
    }

    ListIterator_Init(iterator, &channels); 

    for ( ; ListIterator_MoreEntries(iterator); ListIterator_Next(iterator))
    {
        p = ListIterator_Current(iterator);
        val = strncmp(p->channel_name, name, strlen(p->channel_name));
        if (val == 0)
            return p;
    }
    return NULL;
}

static void conn_close(struct con * conn)
{
    ASSERT(conn != NULL);
    close(conn->skt);
    conn->done = true;
}

static int conn_recv(struct con *conn)
{
    ssize_t n;
    char *channel_name = NULL;
    char *token;
    char *slash;
    int media_type = FLV;

    ASSERT(conn != NULL);
    struct http_client * http = (struct http_client *) conn->ctx;

    for (;;) {
        n = clive_read(conn->skt, (http->rbuffer + http->rlen),\
                   (sizeof(http->rbuffer) - http->rlen));
        if (n > 0) {
            //parse channel name
            http->rlen += (uint32_t)n;
            token = strstr(http->rbuffer, "\r\n");
            if (token != NULL) {
                //got one line
                slash = strchr(http->rbuffer, '/');
                if (slash == NULL)
                    return CL_ERROR;
                slash++;
                channel_name = slash;
                do {
                    if (*slash == ' '){
                        *slash = '\0';
                        break;
                    }
                }while(*slash++ != '\r');
                if (strstr(channel_name, "_ts") != NULL)
                    media_type = TS;
                log_debug(LOG_DEBUG, "channel name:%s", channel_name);
            }
            //find specific channel
            if (clive_channel_is_existed(channel_name) == false) {
                send(conn->skt, error_head, strlen(flv_head), 0);
                return CL_CLOSE;
            }

            //add a output
            if (http_task_is_existed(channel_name)) {
                http_add_client(channel_name, conn->skt);
            }
            else {
                http_add_task(channel_name);
            }
            HTTP_Task *task_temp = http_task_find(channel_name);
            Channel *temp = clive_channel_find(channel_name);
            if ((temp != NULL) && (task_temp != NULL)) {
                if (media_type == FLV)
                    clive_media_add_output(temp->flv_media, task_temp->buffer);
                else
                    clive_media_add_output(temp->ts_media, task_temp->buffer);
            }

            if (media_type == FLV)
                send(conn->skt, flv_head, strlen(flv_head), 0);
            else
                send(conn->skt, ts_head, strlen(flv_head), 0);
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


static void http_close(struct con * conn)
{
    ASSERT(conn != NULL);
    close(conn->skt);
    conn->done = true;
}

static int http_recv(struct con * conn)
{
    ASSERT(conn != NULL);
    int client;
    struct sockaddr_in from;
    uint32_t  slen = sizeof(from);
    struct http_client *new;
    struct event_base *evb = conn->evb;

    client = accept(conn->skt, (struct sockaddr *)&from, &slen);
    if (client > 0) {
        clive_set_nonblocking(client);
        clive_set_sndbuf(client, 512*1024);
        new = clive_calloc(1, sizeof(struct http_client));
        new->connection.skt = client;
        new->connection.type = tHTTP;
        new->connection.evb = evb;
        new->connection.ctx = new;

        new->connection.recv = &conn_recv;
        new->connection.close = &conn_close;
        event_add_conn(evb, &new->connection);
        event_del_out(evb, &new->connection);
    }
    return CL_OK;
}


/*
http://192.168.1.13:80803/onlive
*/
struct http * clive_http_server_new(struct event_base *evb, unsigned short port)
{
    struct http *http;
    int skt;

    ASSERT(evb != NULL);
    http = clive_calloc(1, sizeof(struct http));
    ASSERT(http != NULL);
   

    skt = clive_tcp_socket();
    clive_set_reuseaddr(skt);
    clive_set_nonblocking(skt);
    clive_set_sndbuf(skt, 64*1024);
    clive_set_rcvbuf(skt, 64*1024);
    clive_set_tcpnodelay(skt);
    clive_tcp_bind(skt, "0.0.0.0", port);
    clive_tcp_listen(skt, 100);

    http->connection.skt = skt;
    http->connection.type = tHTTP;
    http->connection.evb = evb;
    //http->connection.send = &http_send;
    http->connection.recv = &http_recv;
    http->connection.close = &http_close;
    http->connection.ctx = http;
    
    event_add_conn(evb, &http->connection);
    event_del_out(evb, &http->connection);

    return http;
}

static void * Entry(void *p)
{
    HTTP_Task * task;
    ListEntry_t * current = NULL;
    uint8_t *buffer;
    uint32_t len;
    buffer = clive_calloc(1, 1024); 
    ListIterator_t iterator;
    int *skt;
    int ret;
   
    do {
        if (current == NULL) {
            pthread_mutex_lock(&lock);
            current = channels.head ;
            pthread_mutex_unlock(&lock);
            sched_yield();
            continue;
        }
        task = current->data;
        //do something
        if (kfifo_len(task->buffer) > 0) {
           len = kfifo_get(task->buffer, buffer, 1024);
           log_debug(LOG_INFO, "http task got %d data from its buffer", len);
           ListIterator_Init(iterator, &task->clients); 
           for ( ; ListIterator_MoreEntries(iterator); ListIterator_Next(iterator))
           {
               skt = ListIterator_Current(iterator);
               ret = clive_write(*skt, buffer, len);
               if ((ret < 0) && (errno != EAGAIN))
               {
                   close(*skt);
                   clive_free(skt);
                   ListRemoveCurrent(&iterator);
               }
           }
        }
        pthread_mutex_lock(&lock);
        current = current ? current->next : NULL;
        pthread_mutex_unlock(&lock);
    }while(1);
    return (void *)0;
}

static void * Event_Entry(void *p)
{
    struct event_base *evb;
    struct http *http;
    int nsd;
    evb  = event_base_create(EVENT_SIZE, &clive_core_core);
    http = clive_http_server_new(evb, HTTP_PORT);

    while (1) {
        nsd = event_wait(evb, 500);
        if (nsd < 0) {
            log_debug(LOG_INFO, "wait error");
            return (void *)0;
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
    val += pthread_create(&tid, &attr, Event_Entry, NULL);
    return val;
}

//no need to stop i think
int clive_http_server_stop(void)
{
    return 0;
}
//used to close HTTP_TASK, called by channel or media module
//HTTP_TASK.buffer 
int clive_http_close_media(struct kfifo *buffer)
{
    return 0;
}

#ifdef TEST
int main(int argc, char **argv)
{
    log_init(LOG_PVERB,NULL);
    clive_http_server_start();
    sched_yield();
    while (1) sleep(1);
}
#endif

