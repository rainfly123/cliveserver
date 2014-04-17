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

enum InputType {
     tcp_ts = 1,
     tcp_flv = 2,
     udp_ts = 3,
     udp_flv = 4,
     rtmp = 5,
     unknown = -1
};
enum OutputType {
     http_ts = 1,
     http_flv = 2,
     hls = 3,
     hds = 4,
     rtmp = 5,
     unknown = -1
};
extern List_t all_channels;

typedef struct {
    OutputType output_type;
    void *media;
    void *protocol;
}OutputFormat;

typedef struct {
    struct con connection;
    struct kfifo *buffer;
    int input_type;
    OutputFormat *outputs;
    int total;     //total number of outputs
 
}channel;

#endif
