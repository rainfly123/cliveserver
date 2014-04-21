/***********************************************88
**************************************************
**************************************************
*/
#ifndef _SOOONER_HEART_H_
#define _SOOONER_HEART_H_

#include "util.h"
#include "event.h"
#include "timer.h"


struct http {
    struct con connection;
    uint8_t  rbuffer[1024];
    uint32_t rlen;
};

/*
*/
struct http * clive_http_server_new(struct event_base *evb， int port);
int clive_http_server_start(struct http *ctx);
int clive_http_server_stop(struct http *ctx);
#endif
