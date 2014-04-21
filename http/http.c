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

static List_t channels;  //store channel name , already someone's viewing
                          // cnc_flv, cnc_ts

#define INTERVAL 1000
static void conn_close(struct con * conn)
{
    ASSERT(conn != NULL);
    struct http * beat = (struct http *) conn->ctx;
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
    struct http * beat = (struct http *) conn->ctx;

    for (;;) {
        n = clive_read(conn->skt, (beat->rbuffer + beat->rlen),\
                   (sizeof(beat->rbuffer) - beat->rlen));
        log_debug(LOG_VERB, "recv on sd %d %zd %s ", conn->skt, n, beat->rbuffer);
        if (n > 0) {
            beat->rlen += (uint32_t)n;
            event_del_out(conn->evb, conn);
            log_debug(LOG_VERB, "recv on sd return  ");
            if (beat->rlen == sizeof(beat->rbuffer)) {
                memset(beat->rbuffer, 0, sizeof(beat->rbuffer));
                beat->rlen = 0;
            }
            return CL_OK;
        }

        if (n == 0) {
            log_debug(LOG_INFO, "recv on sd %d eof rb %d", conn->skt,
                      beat->rlen);
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
    ASSERT(conn != NULL);
    ssize_t n;
    struct http * beat = (struct http *) conn->ctx;

    for (;;) 
    {
        n = clive_write(conn->skt, (beat->sbuffer + beat->send_bytes),\
                     (beat->slen - beat->send_bytes));
        log_debug(LOG_VERB, "send on sd %d %zd",
                  conn->skt, n);

        if (n > 0) {
            beat->send_bytes += (size_t)n;
            if (beat->slen == beat->send_bytes)
            {
               // beat->slen = 0;
                beat->send_bytes = 0;
                log_debug(LOG_VERB, "send on sd %d complete slen %d\n", conn->skt, beat->slen);
                return CL_OK;
              //send completed;
            }
        } else {

        if (errno == EINTR) {
            log_debug(LOG_VERB, "send on sd %d not ready - eintr", conn->skt);
            continue;
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            log_debug(LOG_VERB, "send on sd %d not ready - eagain", conn->skt);
            return CL_OK;
        } else {
            conn->err = errno;
            log_error("send on sd %d failed: %s", conn->skt, strerror(errno));
            return CL_ERROR;
        }
        }
    }

    NOT_REACHED();
    return CL_ERROR;
}

static int reconnect(struct con *conn)
{
    int skt;
    ASSERT(conn != NULL);

    skt = clive_tcp_socket();
    clive_set_nonblocking(skt);
    clive_set_sndbuf(skt, 64*1024);
    clive_set_rcvbuf(skt, 64*1024);
    clive_set_tcpnodelay(skt);

    conn->skt = skt;
    conn->err = 0;
    conn->done = 0;
    int status = connect(skt, (struct sockaddr *)&g_conf.admin_addr.addr, g_conf.admin_addr.addrlen);
    if (status != 0) {
        if (errno != EINPROGRESS)
            conn->err = errno;
    }
    event_add_conn(conn->evb, conn);
}

static int timer_handle(struct timer *timer, void *data)
{
    struct con * conn = (struct con *)data;
    log_debug(LOG_DEBUG, "http timer out");
    if ((conn->done == false) && (conn->err == false)) {
        event_add_out(conn->evb, conn);
        log_debug(LOG_INFO, "http resend %d sec later", INTERVAL/1000);
    }
    else
    {
        log_debug(LOG_INFO, "http reconnect %d sec later", INTERVAL/1000);
        reconnect(conn);
    }
    clive_timer_update_time(timer, INTERVAL);
}
/*
http://192.168.1.13:80803/onlive
*/
struct http * clive_http_new(struct event_base *evb, uint32_t camera_id)
{
    struct http *beat;
    int skt;

    ASSERT(evb != NULL);
    beat = clive_calloc(1, sizeof(struct http));
    ASSERT(beat != NULL);
   
    beat->camera_id = camera_id;
    beat->slen = clive_build_http_request(camera_id, beat->sbuffer, sizeof(beat->sbuffer));
    beat->connection.type = tHEARTBEAT;
    beat->connection.evb = evb;

    skt = clive_tcp_socket();
    clive_set_nonblocking(skt);
    clive_set_sndbuf(skt, 64*1024);
    clive_set_rcvbuf(skt, 64*1024);
    clive_set_tcpnodelay(skt);

    beat->connection.skt = skt;
//    beat->connection.err = 0;
 //   beat->connection.done = 0;
    beat->connection.send = &conn_send;
    beat->connection.recv = &conn_recv;
    beat->connection.close = &conn_close;
    beat->connection.ctx = beat;
    int status = connect(skt, (struct sockaddr *)&g_conf.admin_addr.addr, g_conf.admin_addr.addrlen);
    if (status != 0) {
        if (errno != EINPROGRESS)
            beat->connection.err = errno;
    }
    beat->timer = clive_timer_new(evb, INTERVAL, &timer_handle, &beat->connection);
    event_add_conn(evb, &beat->connection);

    beat->hcount = 0;
    return beat;
}
