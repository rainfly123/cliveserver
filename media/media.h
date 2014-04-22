/*
  * A generic media implementation
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
#ifndef _CLIVE_MEDIA_H_
#define _CLIVE_MEDIA_H_

#include "kfifo.h"
#include "list.h"

enum Media_Type {
    TS2TS = 1,
    TS2FLV = 2,
    FLV2FLV = 3,
    FLV2TS = 4
};

typedef struct {
    int media_type; 

    /*read from the input_buffer*/
    struct kfifo *input_buffer; 

    /*write out to the ouput_buffers*/
    List_t output_pads; 

    /*store PSI OF TS OR FLV HEAD, SEQUENCE tag,  META TAG*/
    void *extra_data; 
    int elen;

    void * private_data;
    int plen;
}sMedia;
    

void * clive_media_create(int media_type, struct kfifo *in_buffer);

int clive_media_start(void *media);

int clive_media_stop(void *media);

int clive_media_release(void *media);

int clive_task_thread_start(void);

#endif
