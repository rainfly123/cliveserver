/*
 * Copyright (C) 2008 - xie changcai
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __UDP_H__
#define __UDP_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include "util.h"

int32_t sdk_udp_socket(void);

void sdk_udp_close( int32_t skt );

int32_t sdk_udp_bind( int32_t skt, int8_t *ip, uint16_t port);

int32_t sdk_udp_send( int32_t skt, int8_t *ip, uint16_t port, void *buf, uint32_t size);

int32_t sdk_udp_recv( int32_t skt, uint32_t *ip, uint16_t *port, void *buf, uint32_t size);
#ifdef __cplusplus
}
#endif

#endif /*__UDP_H__*/
