#
struct kfifo {
    unsigned char *buffer;     /* the buffer holding the data */
    unsigned int size;         /* the size of the allocated buffer */
    unsigned int in;           /* data is added at offset (in % size) */
    unsigned int out;          /* data is extracted from off. (out % size) */
    spinlock_t *lock;          /* protects concurrent modifications */
};

 struct kfifo *kfifo_init(unsigned char *buffer, unsigned int size,
                  gfp_t gfp_mask, spinlock_t *lock);

  struct kfifo *kfifo_alloc(unsigned int size, gfp_t gfp_mask,
                   spinlock_t *lock);

  void kfifo_free(struct kfifo *fifo)

unsigned int kfifo_put(struct kfifo *fifo,
                const unsigned char *buffer, unsigned int len)

unsigned int kfifo_put(struct kfifo *fifo,
                const unsigned char *buffer, unsigned int len)

 unsigned int kfifo_len(struct kfifo *fifo)
