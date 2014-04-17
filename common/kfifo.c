 struct kfifo *kfifo_init(unsigned char *buffer, unsigned int size,
 2              gfp_t gfp_mask, spinlock_t *lock)
 3 {
 4     struct kfifo *fifo;
 6     /* size must be a power of 2 */
 7     BUG_ON(!is_power_of_2(size));
 9     fifo = kmalloc(sizeof(struct kfifo), gfp_mask);
10     if (!fifo)
11         return ERR_PTR(-ENOMEM);
13     fifo->buffer = buffer;
14     fifo->size = size;
15     fifo->in = fifo->out = 0;
16     fifo->lock = lock;
17 
18     return fifo;
19 }
20 struct kfifo *kfifo_alloc(unsigned int size, gfp_t gfp_mask, spinlock_t *lock)
21 {
22     unsigned char *buffer;
23     struct kfifo *ret;
29     if (!is_power_of_2(size)) {
30         BUG_ON(size > 0x80000000);
31         size = roundup_pow_of_two(size);
32     }
34     buffer = kmalloc(size, gfp_mask);
35     if (!buffer)
36         return ERR_PTR(-ENOMEM);
38     ret = kfifo_init(buffer, size, gfp_mask, lock);
39 
40     if (IS_ERR(ret))
41         kfree(buffer);
43     return ret;
44 }
static inline unsigned int kfifo_put(struct kfifo *fifo,
 2                 const unsigned char *buffer, unsigned int len)
 3 {
 4     unsigned long flags;
 5     unsigned int ret;
 6     spin_lock_irqsave(fifo->lock, flags);
 7     ret = __kfifo_put(fifo, buffer, len);
 8     spin_unlock_irqrestore(fifo->lock, flags);
 9     return ret;
10 }
11 
12 static inline unsigned int kfifo_get(struct kfifo *fifo,
13                      unsigned char *buffer, unsigned int len)
14 {
15     unsigned long flags;
16     unsigned int ret;
17     spin_lock_irqsave(fifo->lock, flags);
18     ret = __kfifo_get(fifo, buffer, len);
19         //当fifo->in == fifo->out时，buufer为空
20     if (fifo->in == fifo->out)
21         fifo->in = fifo->out = 0;
22     spin_unlock_irqrestore(fifo->lock, flags);
23     return ret;
24 }
25 
26 
27 unsigned int __kfifo_put(struct kfifo *fifo,
28             const unsigned char *buffer, unsigned int len)
29 {
30     unsigned int l;
31        //buffer中空的长度
32     len = min(len, fifo->size - fifo->in + fifo->out);
34     /*
35      * Ensure that we sample the fifo->out index -before- we
36      * start putting bytes into the kfifo.
37      */
39     smp_mb();
41     /* first put the data starting from fifo->in to buffer end */
42     l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
43     memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);
45     /* then put the rest (if any) at the beginning of the buffer */
46     memcpy(fifo->buffer, buffer + l, len - l);
47 
48     /*
49      * Ensure that we add the bytes to the kfifo -before-
50      * we update the fifo->in index.
51      */
53     smp_wmb();
55     fifo->in += len;  //每次累加，到达最大值后溢出，自动转为0
57     return len;
58 }
59 
60 unsigned int __kfifo_get(struct kfifo *fifo,
61              unsigned char *buffer, unsigned int len)
62 {
63     unsigned int l;
64         //有数据的缓冲区的长度
65     len = min(len, fifo->in - fifo->out);
67     /*
68      * Ensure that we sample the fifo->in index -before- we
69      * start removing bytes from the kfifo.
70      */
72     smp_rmb();
74     /* first get the data from fifo->out until the end of the buffer */
75     l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
76     memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);
78     /* then get the rest (if any) from the beginning of the buffer */
79     memcpy(buffer + l, fifo->buffer, len - l);
81     /*
82      * Ensure that we remove the bytes from the kfifo -before-
83      * we update the fifo->out index.
84      */
86     smp_mb();
88     fifo->out += len; //每次累加，到达最大值后溢出，自动转为0
90     return len;
91 }
