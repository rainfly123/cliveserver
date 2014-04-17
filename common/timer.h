/*
  * A generic task thread implementation
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
