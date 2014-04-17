#ifndef _CLIVE_KFIFO_H_
#define _CLIVE_KFIFO_H_
#include <stdint.h>

struct kfifo {
    unsigned char *buffer;     /* the buffer holding the data */
    unsigned int size;         /* the size of the allocated buffer */
    unsigned int in;           /* data is added at offset (in % size) */
    unsigned int out;          /* data is extracted from off. (out % size) */
};

struct kfifo *kfifo_alloc(uint32_t size);

void kfifo_free(struct kfifo *fifo);

uint32_t  kfifo_put(struct kfifo *fifo, const uint8_t *buffer, uint32_t len);

uint32_t  kfifo_get(struct kfifo *fifo, uint32_t *buffer, uint32_t len);
uint32_t  kfifo_len(struct kfifo *fifo);


#endif
