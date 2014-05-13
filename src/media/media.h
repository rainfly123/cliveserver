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

#include <pthread.h>
#include "kfifo.h"
#include "list.h"

enum Pack_Type {
    TS2TS = 1,
    TS2FLV = 2,
    FLV2FLV = 3,
    FLV2TS = 4,
    Unknown = -1
};

typedef struct {
    int pack_type; 

    int input_media_type;

    /*protect output_pads*/
    pthread_mutex_t olock;

    /*write out to the ouput_buffers*/
    List_t output_pads; 

    /*store PSI OF TS OR FLV HEAD, SEQUENCE tag,  META TAG*/
    void *extra_data; 
    int elen;

    void * private_data;
    int plen;
}sMedia;
    
/*
create a media repack task
*/
sMedia * clive_media_create(int pack_type, int input_media_type);

int clive_media_setype(sMedia * media, int pack_type, int input_media_type);
/*
   DO NOT call it manually,
   chanel core will cat it automaticly
*/
int clive_media_attach(sMedia *flv_media, sMedia *ts_media,
                      struct kfifo *buffer);

/*
   stop the media repacking task
*/
int clive_media_stop(sMedia *media);

/*
   release the media
*/
int clive_media_release(sMedia *media);

int clive_media_add_output(sMedia *media, struct kfifo *buffer);
/*
  start the repacking task thread
*/
int clive_media_task_thread_start(void);

int clive_media_consume_data(sMedia *media, uint8_t *data, uint32_t len);
#endif
