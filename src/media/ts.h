/*
Copyright (C) 2008 xie changcai

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

ts.h

Transport stream processing and filter management.

*/

#ifndef _CLIVE_TS_H_
#define _CLIVE_TS_H_

#include <stdint.h> 

#define GET2BYTES(start) ((start)[0]<<8 | (start[1]))
#define GET3BYTES(start) (0<<24 |(start)[0]<<16 | (start)[1]<<8 | (start[2]))
#define GETWORD(start) ((start)[0]<<24 | (start)[1]<<16 | (start)[2]<<8 | (start[3]))

/**
* @defgroup TSPacket Transport Stream Packet Access
*
* Constant for the size of a transport packet with out hamming code.
*/
#define TSPACKET_SIZE (188)

/*H.220 suggestion paper*/
typedef enum 
{
    Media_Reserved    = 0x00,
    Media_11172_Video = 0x01,
    Media_13818_Video = 0x02,
    Media_11172_Audio = 0x03,
    Media_13818_Audio = 0x04,
    Media_H220_Private = 0x05,
    Media_AAC_Audio =  0x0f,
    Media_H264_Video = 0x1b
}MediaType;


/**
 * Structure representing an MPEG2 Transport Stream packet
 * with out hamming codes.
 */

typedef struct TSPacket_t
{
    uint8_t header[4];                  /**< Packet Header fields */
    uint8_t payload[TSPACKET_SIZE - 4]; /**< Data contained in the packet */
}
TSPacket_t;

/**
 * Retrieves the PID of packet from the packet header
 * @param packet The packet to extract the PID from.
 * @return The PID of the packet as a 16bit integer.
 */
#define TSPACKET_GETPID(packet) \
    (((((packet).header[1] & 0x1f) << 8) | ((packet).header[2] & 0xff)))

/**
 * Sets the PID of the packet in the packet header.
 * @param packet The packet to update.
 * @param pid    The new PID to set.
 */
#define TSPACKET_SETPID(packet, pid) \
    do{ \
        (packet).header[1] = ((packet).header[1] & 0xe0) | ((pid >> 8) & 0x1f); \
        (packet).header[2] = pid & 0xff; \
    }while(0)
/**
 * Retrieves the packet sequence count.
 * @param packet The packet to extract the count from.
 * @return The packet sequence count as a 4 bit integer.
 */
#define TSPACKET_GETCOUNT(packet) \
    ((packet).header[3] & 0x0f)

/**
 * Sets the packet sequence count.
 * @param packet The packet to update.
 * @param count  The new sequence count to set.
 */
#define TSPACKET_SETCOUNT(packet, count) \
    ((packet).header[3] = ((packet).header[3] & 0xf0) | ((count) & 0x0f))

/**
 * Boolean test to determine whether this packet is the start of a payload.
 * @param packet The packet to check.
 */
#define TSPACKET_ISPAYLOADUNITSTART(packet) \
    (((packet).header[1] & 0x40) == 0x40)
/**
 * Boolean test to determine whether this packet is valid, transport_error_indicator 
 * is not set.
 * @param packet The packet to check.
 * @return True if the packet is valid, false otherwise.
 */
#define TSPACKET_ISVALID(packet) \
    (((packet).header[1] & 0x80) == 0x00)

/**
 * Retrieves the priority field of the packet.
 * @param packet The packet to check.
 * @return The packet priority.
 */
#define TSPACKET_GETPRIORITY(packet) \
    (((packet).header[1] & 0x20) >> 4)

/**
 * Set the priority field of the packet.
 * @param packet The packet to update.
 * @param priority Either 1 or 0 to indicate that this is a priority packet.
 */
#define TSPACKET_SETPRIORITY(packet, priority) \
    ((packet).header[1] = ((packet).header[1] & 0xdf) | (((priority) << 4) & 0x20))

/**
 * Retrieve whether the packet has an adaptation field.
 * @param packet The packet to check.
 * @return The adapatation field control flags.
 */
#define TSPACKET_GETADAPTATION(packet) \
    (((packet).header[3] & 0x30) >> 4)

/**
 * whether the packet has an pcr .
 * @param packet The packet to check.
 * @return True if pcr flags is set,false otherwise.
 */
#define TSPACKET_ISPCRFLAGSET(packet) \
    (((packet).payload[1] & 0x10) == 0x10)
/**
 * Retrieve an pcr base field.
 * @param packet The packet to check.
 * @return The pcr field base.
 */
#define TSPACKET_GETPCRBASE(packet) \
    ((uint64_t)((packet).payload[2]) << 25)|(((packet).payload[3]) <<17)|(((packet).payload[4]) <<9)|(((packet).payload[5]) <<1)|(((packet).payload[6] & 0x80) >> 7) 


#define TSPACKET_GETPCR_32BIT(packet) \
    ((((packet).payload[2]) << 24)|(((packet).payload[3]) <<16)|(((packet).payload[4]) <<8)|((packet).payload[5])) 

/**
 * Retrieve an pcr extension field.
 * @param packet The packet to check.
 * @return The pcr field base.
 */
#define TSPACKET_GETPCREXT(packet) \
    ((packet).payload[7]) | (((packet).payload[6] & 0x01)<<8)
/**
 * Set whether the packet has an adaptation field.
 * @param packet The packet to update.
 * @param adaptation The new adaptation field flags.
 */
#define TSPACKET_SETADAPTATION(packet, adaptation) \
    ((packet).header[3] = ((packet).header[3] & 0xcf) | (((adaptation) << 4) & 0x30))

/**
 * Retrieves the adaptation field length.
 * @param packet The packet to check.
 * @return The length of the adaptation field.
 */
#define TSPACKET_GETADAPTATION_LEN(packet) \
    ((packet).payload[0])
#endif

#endif
