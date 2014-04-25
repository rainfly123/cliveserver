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

#include "ts.h"
#include "pmt.h"
#include "stdafx.h"

pmt_list pmtparse(int fd,uint16_t pmt_pid)
{

    TSPacket_t data;
    pmt_list pmt ;
    uint8_t size = 0 ;
    uint16_t adaption = 0 ;
    uint16_t section_length = 0 ;
    uint16_t program_info_length = 0 ;
    uint16_t es_info_length = 0 ;
    uint8_t payload_start = 0;
    uint8_t pointer = 0;
    uint8_t table_id = 0;
    uint8_t *p = 0;
    uint16_t i = 32768;

    memset((void*)&data,0,sizeof(data));
    memset((void*)&pmt,0,sizeof(pmt));
    while(i--)
    {
    size = read(fd,(char*)&data,sizeof(data));
    if(size != sizeof(data))
        continue ;
    if(data.header[0] != 0x47)
        continue;
    if(TSPACKET_GETPID(data) != pmt_pid)
        continue ;
    if(TSPACKET_ISPAYLOADUNITSTART(data))
    {
       adaption = TSPACKET_GETADAPTATION(data);
       if(adaption&0x01)
       {
            if(adaption&0x02)  //has adaption filed
              {
                payload_start = data.payload[0] + 1;
                pointer = data.payload[payload_start];
                payload_start += pointer ;
                payload_start++;
                //printf("has adaption payload start %d\n",payload_start);
               }
               else{
                pointer = data.payload[payload_start];
                payload_start += pointer ;
                payload_start++;}
               // printf("no adaption payload start %d\n",payload_start);
                table_id = data.payload[payload_start];
                if(table_id != 0x02)
                    return pmt ;
                payload_start += 1;  //skip tableid
                p = &data.payload[payload_start];
                section_length = GET2BYTES(p);
                section_length = section_length & 0x03ff ; //10bit section length
               // printf("section length %x\n",section_length);
                payload_start += 2 ;
                p = &data.payload[payload_start];
                pmt.program_num = GET2BYTES(p);
               // printf("program num %x\n",pmt.program_num);
                payload_start += 5 ;
                p = &data.payload[payload_start];
                pmt.pcr_pid = GET2BYTES(p) &0x1fff;
               // printf("pcr pid %x\n",pmt.pcr_pid);
                payload_start += 2 ;
                p = &data.payload[payload_start];
                program_info_length = GET2BYTES(p) &0x0fff;
               // printf("program info length %x\n",program_info_length);
                payload_start += program_info_length ;
                payload_start += 2;

                section_length-=13;
                section_length-=program_info_length;
               // printf("now section length %x\n",section_length);
                while (section_length) {
                pmt.alles[pmt.total_es].stream_type = data.payload[payload_start];
               // printf("stream_type %x\n",pmt.alles[pmt.total_es].stream_type);
                payload_start += 1;
                p = &data.payload[payload_start];
                pmt.alles[pmt.total_es].es_pid = GET2BYTES(p) &0x1fff;
               // printf("es_pid %x\n",pmt.alles[pmt.total_es].es_pid);
                pmt.total_es++;
                payload_start += 2;
                p = &data.payload[payload_start];
                es_info_length = GET2BYTES(p) &0x0fff;
               // printf("es_info_length %x\n",es_info_length);
                payload_start += 2;
                payload_start += es_info_length; 
                section_length -= 5 ;
                section_length -= es_info_length; }
                break;
         
        }
        else continue;

    } //end payload start 

    else
        continue; 
    } //end while 

    lseek(fd,0,SEEK_SET);
    return pmt;
}
#if 0
int fd;
int main(int argc ,char *argv[])
{
    fd = open(argv[1],O_RDONLY);
    if(fd == -1)
    {
        printf("open file error\n");
        return -1;
        }
    pmt_list b =   pmtparse(fd,0x404);
    printf("%x %x %x %x %x",b.total_es,b.pcr_pid,b.program_num,b.alles[0].stream_type,b.alles[0].es_pid);

 
 }
 #endif 

