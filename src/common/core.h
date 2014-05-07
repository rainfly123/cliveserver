#ifndef _CLIVE_CORE_H_
#define _CLIVE_CORE_H

#include "event.h"

int clive_core_close(struct con *conn);
int clive_core_recv(struct con *conn);
int clive_core_send(struct con *conn);
int clive_core_core(void *arg, uint32_t events);
#endif
