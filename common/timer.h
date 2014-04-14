#include "util.h"
#include "event.h"


#ifndef _TIME_H_
#define _TIME_H_
struct tevents;
struct timer{
    struct con connection;
    uint64_t rbuffer;
    uint64_t sbuffer;
    struct tevents * handle;
};
typedef int (*tevent_handler)(struct timer * time, void *data);
struct tevents{
    tevent_handler handle;
    void *data;
};

struct timer *clive_timer_new(struct event_base *evb, uint32_t msecs, tevent_handler handle,\
                            void *data);
int clive_timer_update_time(struct timer *time, uint32_t msecs);
int clive_timer_update_handle(struct timer *time, tevent_handler handle, void *data);

#endif
