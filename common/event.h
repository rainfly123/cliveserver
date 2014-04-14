/*
 * twemproxy - A fast and lightweight proxy for memcached protocol.
 * Copyright (C) 2011 Twitter, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _CLIVE_EVENT_H_
#define _CLIVE_EVENT_H_

#define EVENT_SIZE  32

#define EVENT_READ  0x0000ff
#define EVENT_WRITE 0x00ff00
#define EVENT_ERR   0xff0000

#define tHEARTBEAT 'H'
#define tTIMER 'T'

struct con;
typedef int (*conn_recv_t)(struct con*);
typedef int (*conn_send_t)(struct con*);
typedef void (*conn_close_t)(struct con *);
struct con
{
   int skt;
   bool recv_active;
   bool send_active;
   char type;
   unsigned int events;
   bool done;
   bool err;
   void *evb;
   conn_recv_t     recv;          /* recv (read) handler */
   conn_send_t     send;          /* send (write) handler */
   conn_close_t    close;         /*close handler*/
   void *ctx;
};


typedef int (*event_cb_t)(void *, uint32_t);

struct event_base {
    int                ep;      /* epoll descriptor */

    struct epoll_event *event;  /* event[] - events that were triggered */
    int                nevent;  /* # event */

    event_cb_t         cb;      /* event callback */
};

struct event_base *event_base_create(int size, event_cb_t cb);
void event_base_destroy(struct event_base *evb);

int event_add_in(struct event_base *evb, struct con *c);
int event_del_in(struct event_base *evb, struct con *c);
int event_add_out(struct event_base *evb, struct con *c);
int event_del_out(struct event_base *evb, struct con *c);
int event_add_conn(struct event_base *evb, struct con *c);
int event_del_conn(struct event_base *evb, struct con *c);
int event_wait(struct event_base *evb, int timeout);

#endif /* _NC_EVENT_H */
