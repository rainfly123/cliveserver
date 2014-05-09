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
  
#ifndef _CLIVE_CHANNEL_H_
#define _CLIVE_CHANNEL_H_

#include "util.h"
#include "list.h"
#include "event.h"
#include "media.h"

#define Unknown (-1)

enum MediaType {
     TS = 0,
     FLV = 1,
};
enum Protocol {
    UDP = 0,
    TCP = 1,
    HTTP = 2,
    RTMP = 3,
    HLS = 1,
    HDS = 2,
};


extern List_t all_channels;

typedef struct {
    struct con connection;
    struct event_base *evb;
    char channel_name[32];
    int input_protocol;
    int input_media_type;

    struct kfifo *buffer; //channel's input buffer
    sMedia *ts_media; // write out to output protocol's kfifo buffer
                     // protocol can register kfifo buffer to media's pads
    sMedia *flv_media;
 
}Channel;

void clive_init_channel(void);

Channel *clive_new_channel(struct event_base *evb, char *url, char *name);

int clive_channel_add_output(Channel * channel, char *url);

int clive_channel_start(Channel * channel);

int clive_channel_stop(Channel *channel);

bool clive_channel_is_existed(const char *name);

Channel * clive_channel_find(const char *name);

bool clive_channel_add(Channel *channel);

#endif
