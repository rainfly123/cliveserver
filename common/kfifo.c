#include "util.h"
#include "kfifo.h"

 struct kfifo *kfifo_alloc(unsigned int size)
 {
     uint8_t *buffer;
     struct kfifo *ret;

     if (size % 2 != 0) {
         size += 1;
     }
     ret = (struct kfifo *)sdk_alloc(sizeof(struct kfifo));
     buffer = sdk_alloc(size);
     fifo->buffer = buffer;
     fifo->size = size;
     fifo->in = fifo->out = 0;
     return ret;
 }

static inline uint32_t kfifo_put(struct kfifo *fifo,
                 const uint8_t *buffer, uint32_t len)
 {
     uint32_t ret;
     ret = __kfifo_put(fifo, buffer, len);
     return ret;
 }
 
 static inline unsigned int kfifo_get(struct kfifo *fifo,
                      unsigned char *buffer, unsigned int len)
 {
     uint32_t ret;
     ret = __kfifo_get(fifo, buffer, len);
     if (fifo->in == fifo->out)
         fifo->in = fifo->out = 0;
     return ret;
 }
 
 
 uint32_t __kfifo_put(struct kfifo *fifo,
             const uint8_t *buffer, uint32_t len)
 {
     uint32_t l;

     len = min(len, fifo->size - fifo->in + fifo->out);
     /* first put the data starting from fifo->in to buffer end */
     l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
     memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);
     /* then put the rest (if any) at the beginning of the buffer */
     memcpy(fifo->buffer, buffer + l, len - l);
 
     fifo->in += len;  
     return len;
}
 
uint32_t __kfifo_get(struct kfifo *fifo,
              uint8_t *buffer, uint32_t  len)
{
     uint32_t l;

     len = min(len, fifo->in - fifo->out);
     /* first get the data from fifo->out until the end of the buffer */
     l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
     memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);
     /* then get the rest (if any) from the beginning of the buffer */
     memcpy(buffer + l, fifo->buffer, len - l);
     fifo->out += len; 
     return len;
 }
uint32_t kfifo_len(struct fifo *fifo) {
        return (fifo->in - fifo.out); 
}
