#ifndef _CLIVE_MEDIA_H_
#define _CLIVE_MEDIA_H_

#include "kfifo.h"
#include "list.h"

enum {
    TS2TS = 1,
    TS2FLV = 2,
    FLV2FLV = 3,
    FLV2TS = 4
};

#define MEDIA \
    int media_type; \
    struct kfifo *input_buffer; \
    //read from the input_buffer
    List_t output_buffers ;
    //write out to the ouput_buffers
    

void * clive_media_create(int media_type);

int clive_media_start(void *media);

int clive_media_stop(void *media);

#endif
