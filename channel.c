/*
  * A generic channel implementation
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
#include <string.h>
#include "channel.h"
#include "util.h"
#include "log.h"
#include "kfifo.h"

/*
    create a new channel
    url: could be:
         1) http://x.x.x.x:80/xx //cliveserver is client
         2) tcp://x.x.x.x:9090  //cliveserver is tcp server
         3) udp://x.x.x.x:8080  //cliveserver is udp server
         4) rtmp://x.x.x.x:1935/live/cnv //cliveserver is client
*/
static const char *protocol[4] = {
    "udp://",
    "tcp://", 
    "http://", 
    "rtmp://"
};
Channel * clive_new_channel(struct event_base *evb, char *url)
{
    Channel *channel;
    int val;
    int status;
    char *port;
    char *ip;
    char *loc;
    char ip_addr[64];
    int sport = 80;
    char location[32];
    int input_type = -1;
    int i = 0;
    struct sockinfo sock;
    int skt;



    ASSERT(evb != NULL);
    ASSERT(url != NULL);
    memset(ip_addr, 0, sizeof(ip_addr));
    memset(location, 0, sizeof(location));

    channel = clive_calloc(1, sizeof(Channel));
    ASSERT(channel != NULL);


    for (; i < sizeof(protocol) / sizeof(char *); i++)
    {
        if (!strncmp(url, protocol[i], strlen(protocol[i])))
        {
            input_type = i;
            break;
        }
    }
    switch (input_type)
    {
        case HTTP:
        {
            ip += strlen(protocol[input_type]);
            port = strchr(ip, ':');
            if (port != NULL) {
                strncpy(ip_addr, ip, port - ip);
                port += 1;
                sport = clive_atoi(port, 5);
            } else {
                port = strchr(ip, '/');
                strncpy(ip_addr, ip, port - ip);
            }
            loc = strchr(port, '/');
            if (loc != NULL) {
                strncpy(location, loc, sizeof(location));
            }
            val = clive_resolve(ip_addr, sport, &sock);
            break;
        }
        case UDP:
        case TCP:
        {
            ip += strlen(protocol[input_type]);
            port = strchr(ip, ':');
            if (port != NULL) {
                strncpy(ip_addr, ip, port - ip);
                port += 1;
                sport = clive_atoi(port, 5);
            } else {
                strcpy(ip_addr, ip);
                sport = 9090; //default value
            }
            break;
        }
    }
    if (input_type == TCP) {
        skt = clive_tcp_socket();
        clive_set_tcpnodelay(skt);
        clive_set_nonblocking(skt);
        clive_set_sndbuf(skt, 64*1024);
        clive_set_rcvbuf(skt, 64*1024);
        clive_tcp_bind(skt, ip_addr, sport);
        clive_tcp_listen(skt, 3);
    }
    if (input_type == UDP) {
        skt = clive_udp_socket();
        clive_set_nonblocking(skt);
        clive_set_sndbuf(skt, 64*1024);
        clive_set_rcvbuf(skt, 64*1024);
        clive_udp_bind(skt, ip_addr, sport);
    }
    if (input_type == HTTP) {
    }
    channel->input_type = input_type;
    channel->connection.skt = skt;
    channel->connection.send = &conn_send;
    channel->connection.recv = &conn_recv;
    channel->connection.close = &conn_close;
    channel->connection.ctx = channel;
    log_debug(LOG_INFO, "clive_new_channel ip:%s port:%d location:%s", ip_addr, sport, loc);

    return channel;
}

int clive_channel_start(Channel * channel)
{
    int val;

    ASSERT(channel != NULL);

    val = event_add_conn(evb, &channel->connection);
    if (val != 0) {
        return val;
    }

    if ((channel->input_type == TCP) || (channel->input_type == UDP))
        val = event_del_out(evb, &channel->connection);

    return val; 
}

int clive_channel_stop(Channel *channel)
{
    ASSERT(channel != NULL);
    return event_del_conn(evb, &timer->connection);
}

