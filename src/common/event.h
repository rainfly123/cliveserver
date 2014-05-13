/*
  * A generic event loop implementation
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

#ifndef _CLIVE_EVENT_H_
#define _CLIVE_EVENT_H_

#define EVENT_SIZE  32

#define EVENT_READ  0x0000ff
#define EVENT_WRITE 0x00ff00
#define EVENT_ERR   0xff0000

#define tHTTP 'H'
#define tTIMER 'T'
#define tTCP 'C'
#define tUDP 'U'


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
   int err;
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
