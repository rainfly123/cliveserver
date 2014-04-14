#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "udp.h"


int32_t sdk_udp_socket(void)
{
    return socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP );
}

/* Ïú»Ùsocket */
void sdk_udp_close( int32_t skt )
{
    if (skt != -1 )
        close( skt );
}

int32_t sdk_udp_bind( int32_t skt, int8_t *ip, uint16_t port )
{
    struct sockaddr_in local;
    memset( &local, 0, sizeof(local) );
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = inet_addr(ip);
    local.sin_port = htons(port);
    if (bind(skt, (struct sockaddr *)&local,	sizeof(local)) < 0)
        return -1;
    return 0;
}

int32_t sdk_udp_send( int32_t skt, int8_t *ip, uint16_t port, void *buf, uint32_t size )
{
    struct sockaddr_in sn;
    uint32_t slen = sizeof(sn);

    memset(&sn, 0, sizeof(sn));
    slen = sizeof(sn);
    sn.sin_family = AF_INET;
    sn.sin_addr.s_addr = inet_addr(ip);
    sn.sin_port =  htons(port);

    return sendto(skt, buf, size, 0, (struct sockaddr *)&sn, slen);
}

int32_t sdk_udp_recv( int32_t skt, uint32_t *ip, uint16_t *port, void *buf, uint32_t size )
{
    struct sockaddr_in from;
    uint32_t slen = sizeof(from);

    memset(&from, 0, sizeof(from));
    slen = recvfrom(skt, buf, size, 0, (struct sockaddr *)&from, &slen);

    if (ip)
        *ip = from.sin_addr.s_addr;
    if (port)
        *port = from.sin_port;

    return slen;
}

