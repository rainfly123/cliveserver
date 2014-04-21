/*
  * A generic http implementation
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
#ifndef _SOOONER_HEART_H_
#define _SOOONER_HEART_H_

#include "util.h"
#include "event.h"
#include "timer.h"


struct http {
    struct con connection;
    uint8_t  rbuffer[1024];
    uint32_t rlen;
};

/*
*/
struct http * clive_http_server_new(struct event_base *evb， int port);
int clive_http_server_start(struct http *ctx);
int clive_http_server_stop(struct http *ctx);
#endif
