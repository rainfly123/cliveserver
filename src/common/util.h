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
#ifndef _CLIVE_UTIL_H_
#define _CLIVE_UTIL_H_

#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#define true 1
#define false 0
#define LF                  (uint8_t) 10
#define CR                  (uint8_t) 13
#define CRLF                "\x0d\x0a"
#define CRLF_LEN            (sizeof("\x0d\x0a") - 1)
#define CL_OK        0
#define CL_ERROR    -1
#define CL_EAGAIN   -2
#define CL_ENOMEM   -3
#define CL_CLOSE    -4

#define NELEMS(a)           ((sizeof(a)) / sizeof((a)[0]))

#define MIN(a, b)           ((a) < (b) ? (a) : (b))
#define MAX(a, b)           ((a) > (b) ? (a) : (b))

#define SQUARE(d)           ((d) * (d))
#define VAR(s, s2, n)       (((n) < 2) ? 0.0 : ((s2) - SQUARE(s)/(n)) / ((n) - 1))
#define STDDEV(s, s2, n)    (((n) < 2) ? 0.0 : sqrt(VAR((s), (s2), (n))))

#define CLIVE_INET4_ADDRSTRLEN (sizeof("255.255.255.255") - 1)
#define CLIVE_MAXHOSTNAMELEN   256

/*
 * Wrapper to workaround well known, safe, implicit type conversion when
 * invoking system calls.
 */
#define clive_gethostname(_name, _len) \
    gethostname((char *)_name, (size_t)_len)

#define clive_atoi(_line, _n)          \
    _clive_atoi((uint8_t *)_line, (size_t)_n)

typedef unsigned int bool;

int clive_set_blocking(int sd);
int clive_set_nonblocking(int sd);
int clive_set_reuseaddr(int sd);
int clive_set_tcpnodelay(int sd);
int clive_set_linger(int sd, int timeout);
int clive_set_sndbuf(int sd, int size);
int clive_set_rcvbuf(int sd, int size);
int clive_get_soerror(int sd);
int clive_get_sndbuf(int sd);
int clive_get_rcvbuf(int sd);

int _clive_atoi(uint8_t *line, size_t n);
bool clive_valid_port(int n);

/*
 * Memory allocation and free wrappers.
 *
 * These wrappers enables us to loosely detect double free, dangling
 * pointer access and zero-byte alloc.
 */
#define clive_alloc(_s)                    \
    _clive_alloc((size_t)(_s), __FILE__, __LINE__)

#define clive_zalloc(_s)                   \
    _clive_zalloc((size_t)(_s), __FILE__, __LINE__)

#define clive_calloc(_n, _s)               \
    _clive_calloc((size_t)(_n), (size_t)(_s), __FILE__, __LINE__)

#define clive_realloc(_p, _s)              \
    _clive_realloc(_p, (size_t)(_s), __FILE__, __LINE__)

#define clive_free(_p) do {                \
    _clive_free(_p, __FILE__, __LINE__);   \
    (_p) = NULL;                        \
} while (0)

void *_clive_alloc(size_t size, const char *name, int line);
void *_clive_zalloc(size_t size, const char *name, int line);
void *_clive_calloc(size_t nmemb, size_t size, const char *name, int line);
void *_clive_realloc(void *ptr, size_t size, const char *name, int line);
void _clive_free(void *ptr, const char *name, int line);

/*
 * Wrappers to send or receive n byte message on a blocking
 * socket descriptor.
 */
#define clive_sendn(_s, _b, _n)    \
    _clive_sendn(_s, _b, (size_t)(_n))

#define clive_recvn(_s, _b, _n)    \
    _clive_recvn(_s, _b, (size_t)(_n))

/*
 * Wrappers to read or write data to/from (multiple) buffers
 * to a file or socket descriptor.
 */
#define clive_read(_d, _b, _n)     \
    read(_d, _b, (size_t)(_n))

#define clive_readv(_d, _b, _n)    \
    readv(_d, _b, (int)(_n))

#define clive_write(_d, _b, _n)    \
    write(_d, _b, (size_t)(_n))

#define clive_writev(_d, _b, _n)   \
    writev(_d, _b, (int)(_n))

ssize_t _clive_sendn(int sd, const void *vptr, size_t n);
ssize_t _clive_recvn(int sd, void *vptr, size_t n);


#define ASSERT(_x) do {                         \
    if (!(_x)) {                                \
        clive_assert(#_x, __FILE__, __LINE__, 1);  \
    }                                           \
} while (0)

#define NOT_REACHED() ASSERT(0)


void clive_assert(const char *cond, const char *file, int line, int panic);

int64_t clive_usec_now(void);
int64_t clive_msec_now(void);

/*
 * Address resolution for internet (ipv4 and ipv6) and unix domain
 * socket address.
 */

struct sockinfo {
    int       family;              /* socket address family */
    socklen_t addrlen;             /* socket address length */
    union {
        struct sockaddr_in  in;    /* ipv4 socket address */
        struct sockaddr_in6 in6;   /* ipv6 socket address */
        struct sockaddr_un  un;    /* unix domain address */
    } addr;
};

struct msg {
    void *data;
    uint32_t len; 
};
int clive_resolve(char *name, int port, struct sockinfo *si);
char *clive_unresolve_addr(struct sockaddr *addr, socklen_t addrlen);
char *clive_unresolve_peer_desc(int sd);
char *clive_unresolve_desc(int sd);

#endif
