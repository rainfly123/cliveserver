/***********************************************88
**************************************************
**************************************************
*/
#ifndef _SOOONER_HEART_H_
#define _SOOONER_HEART_H_

#include "util.h"
#include "event.h"
#include "timer.h"


struct http{
    struct con connection;
    uint8_t  rbuffer[1024];
    uint32_t rlen;
    uint8_t sbuffer[256];
    uint32_t slen;
    uint32_t send_bytes;
    struct timer * timer;
};

/*
*/
struct http * clive_http_new(struct event_base *evb);
#endif
