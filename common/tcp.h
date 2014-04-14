/*
 Copyright (C) 2009 - xiecc
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
*/

#ifndef _CLIVE_TCP_H_
#define _CLIVE_TCP_H_

#include <sys/select.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @brief create tcp socket
 * @return socket fd if succeeded ,-1 esle
 */
int sdk_tcp_socket();

/** 
 * @brief close socket
 * @param skt socket fd
 */
void sdk_tcp_close( int skt );

/** 
 * @brief connect remote server
 * @param skt socket fd
 * @param ip remote server ip (network byte order)
 * @param port remoter server port (network byte order)
 * @return -1 failed
 */
int sdk_tcp_connect( int skt, unsigned long ip, unsigned short port );

/** 
 * @brief bind local socket
 * @param skt socket fd
 * @param ip  local ip address (network byte order)
 * @param port local port (network byte order)
 * @return -1 failed
 */
int sdk_tcp_bind( int skt, unsigned long ip, unsigned short port );

/*  
 * @brief listen local 
 * @param skt 
 * @param max 5
 * @return
 */
int sdk_tcp_listen( int skt, int max );

/** 
 * @brief 
 * @param skt 
 * @param ip 
 * @param port 
 * @return 
 */
int sdk_tcp_accept( int skt, unsigned long *ip, unsigned short *port );

int sdk_tcp_select( int skt, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, int timeout );

#ifdef __cplusplus
}
#endif

#endif /*__SWtcp_H__*/
