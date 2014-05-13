#include "media.h"
#include "kfifo.h"

int do_pack_ts2ts(sMedia *media, uint8_t *buffer, uint32_t len)
{

    ListIterator_t iterator;
    struct kfifo *buf;
    
    if ((media == NULL) || (buffer == NULL))
        return -1;

    ListIterator_Init(iterator, &media->output_pads); 
    for ( ; ListIterator_MoreEntries(iterator); ListIterator_Next(iterator))
    {
         buf = ListIterator_Current(iterator);
         kfifo_put(buf, "hello", 5);
    }
}
