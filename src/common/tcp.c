/*
 Copyright (C) 2009 - xiecc
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcp.h"

/** 
 * @brief 
 * 
 * @return 
 */
int clive_tcp_socket(void)
{
    return socket( AF_INET, SOCK_STREAM, 0);
}

/** 
 * @brief 
 * 
 * @param skt 
 */
void clive_tcp_close( int skt )
{
    if ( skt > 0)
        close( skt );
}

/** 
 * @brief 
 * 
 * @param skt 
 * @param ip 
 * @param port 
 * 
 * @return 
 */
int clive_tcp_connect( int skt, unsigned long ip, unsigned short port )
{
    struct sockaddr_in addr;

    memset( &addr, 0, sizeof(addr) );
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ip;
    addr.sin_port = port;

    return connect( skt, (struct sockaddr *)&addr, sizeof(addr) );
}

/** 
 * @brief 
 * 
 * @param skt 
 * @param ip 
 * @param port 
 * 
 * @return 
 */
int clive_tcp_bind( int skt, char *ip, unsigned short port )
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    return bind( skt, (struct sockaddr*)&addr, sizeof(addr) );
}

/** 
 * @brief 
 * 
 * @param skt 
 * @param max 
 * 
 * @return 
 */
int clive_tcp_listen( int skt, int max )
{
    return listen( skt, max );
}

/** 
 * @brief 
 * 
 * @param skt 
 * @param ip 
 * @param port 
 * 
 * @return 
 */
int clive_tcp_accept( int skt, unsigned long *ip, unsigned short *port )
{
    struct sockaddr_in from;
    unsigned int slen = sizeof(from);

    skt = accept(skt, (struct sockaddr *)&from, &slen);
    if (0 <= skt)
    {
        if (ip)
            *ip = from.sin_addr.s_addr;
	if (port)
            *port = from.sin_port;
    }
    return skt;
}

int clive_tcp_select( int skt, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, int timeout )
{
    struct timeval tv;

    if ( readfds )
    {
        FD_ZERO( readfds );
        FD_SET( (unsigned int)skt, readfds );
     }
    if ( writefds )
    {
        FD_ZERO( writefds );
        FD_SET(  (unsigned int)skt, writefds );
    }
    if ( exceptfds )
    {
        FD_ZERO( exceptfds );
	FD_SET( (unsigned int)skt, exceptfds );
    }
    if ( 0 <= timeout )
    {
        tv.tv_sec = timeout/1000;
	tv.tv_usec = (timeout%1000)*1000;
	return select( skt+1, readfds, writefds, exceptfds, &tv );
    }
    else
        return select( skt+1, readfds, writefds, exceptfds, NULL );
}

