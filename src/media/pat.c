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

/** 
* @file pat.c
* @brief get the video pid from the file descriptor 
* @author xie changcai
* @date 2008-09-05
*/

#include "ts.h"
#include "pat.h"
#include "stdafx.h"

/** 
* @brief get the video pid 
* 
* @param fd file handle 
* 
* @return if succeeded return the videos' pid ,else 0  
*/

pat_list patparse(int fd)
{
    TSPacket_t data;
    pat_list programs ;
    uint8_t size = 0 ;
    uint16_t adaption = 0 ;
    uint16_t program_num = 0 ;
    uint16_t program_map_pid = 0 ;
    uint16_t section_length = 0 ;
    uint8_t payload_start = 0;
    uint8_t pointer = 0;
    uint8_t table_id = 0;
    uint8_t *p = 0;
    uint16_t i = 32768;

    memset((void*)&data,0,sizeof(data));
    memset((void*)&programs,0,sizeof(programs));
    while(i--)
    {
    size = read(fd,(char*)&data,sizeof(data));
    if(size != sizeof(data))
        continue ;
    if(data.header[0] != 0x47)
        continue;
    if(TSPACKET_GETPID(data))
        continue ;
    if(TSPACKET_ISPAYLOADUNITSTART(data))
     {
       adaption = TSPACKET_GETADAPTATION(data);
       if(adaption&0x01)
       {
            //printf("pid 0 has payload\n");
            if(adaption&0x02)  //has adaption filed
              {
                payload_start = data.payload[0] + 1;
                pointer = data.payload[payload_start];
                payload_start += pointer ;
                payload_start++;
                //printf("has adaption payload start %d\n",payload_start);
               }
            else {
                pointer = data.payload[payload_start];
                payload_start += pointer ;
                payload_start++; }
                //printf("no adaption payload start %d\n",payload_start);
                table_id = data.payload[payload_start];
                if(table_id)   // not pat table
                    continue ;
                payload_start += 1;  //skip tableid
                p = &data.payload[payload_start];
                section_length = GET2BYTES(p);
                section_length = section_length & 0x03ff ; //10bit section length
                section_length -= 9 ;
                payload_start += 7 ;
                while(section_length){
                p = &data.payload[payload_start];
                program_num = GET2BYTES(p);
                programs.program[programs.total].program_num= program_num ;
                //printf("progam number==== %x \n",program_num);
                payload_start += 2;
                p = &data.payload[payload_start];
                program_map_pid = GET2BYTES(p) & 0x1fff;
                programs.program[programs.total].pmt_pid = program_map_pid ;
                programs.total += 1 ;
                payload_start += 2;
               // printf("program map pid==== %x\n",program_map_pid);
                section_length-=4;
                }
                break ;

       }
       else
        continue ;
     }
    else 
       continue ;

    }
    lseek(fd,0,SEEK_SET);
    return programs;

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
    pat_list a =   patsearch(fd);
    sleep(10);
            int i =0;
            for (i =0 ;i<a.total;i++)
            {
                printf("program number ==== %x\n",a.program[i].program_num);
                printf("program pmt pid ==== %x\n",a.program[i].pmt_pid);
                }


}
#endif

