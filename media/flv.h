#ifndef _CLIVE_FLV_H_
#define _CLIVE_FLV_H_

#include <stdint.h>

typedef struct {
    uint8_t tag_type;
    uint32_t data_size ;//////*human readable, 
    uint32_t time_stamp ;  ///human readable
    uint32_t streamID ;
    void *data;
} sTag;

#define IS_VIDEO_TAG(Tag) ((Tag.tag_type)  == 9 )
#define IS_AUDIO_TAG(Tag) ((Tag.tag_type)  == 8 )
#define IS_KEY_FRAME(Tag) ((Tag.data[0] & 0xf0) == 0x10)
#define IS_AVC_SEQ_HEAD(Tag) (Tag.data[1] == 0x00)
#endif
