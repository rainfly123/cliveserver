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
#include "con.h"
#include "media.h"

enum InputType {
     TS = 1,
     FLV = 2,
     Unknown = -1
};
enum InputProtocol {
    TCP = 1,
    UDP = 2,
    RTMP = 3,
    Unknown = -1
};

enum OutputType {
     TS = 1,
     FLV = 2,
     Unknown = -1
};
enum OutputProtocol {
     HTTP = 1,//flv or ts
     HLS = 2,
     HDS = 3,
     RTMP = 4,
     Unknown = -1
};

extern List_t all_channels;

typedef struct {
    OutputType output_type; //FLV OR TS
    sMedia *media; // write out to protocol's kfifo buffer
                 // protocol can register kfifo buffer to media's pads
}OutputFormat;

typedef struct {
    struct con connection;
    char channel_name[32];
    struct kfifo *buffer; //channel's input buffer
    int input_type;
    int input_protocol;
    
    OutputFormat *outputs[2]; //FLV OR TS
    int total;     //total number of outputs, maximum 2
 
}Channel;

Channel *clive_new_channel(struct event_base *evb, char *url);

int clive_channel_start(Channel * channel);

int clive_channel_stop(Channel *channel);

#endif
