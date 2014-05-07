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

#ifndef __PAT_H__
#define __PAT_H__
#include <stdint.h> 

typedef struct 
{
uint16_t program_num ;
uint16_t pmt_pid;
}program_list ;


typedef struct 
{
uint8_t  total;
program_list program[20] ;  //the max stream in one ts
}pat_list ;

/** 
* @brief search tht ts file ,to get the video pid
* 
* @param fd the file handle
* 
* @return videos'pid number
*/

pat_list patparse(int fd) ;
//typedef struct pat
//{
//  uint8_t table_id ;
//  uint16_t section_length ;
//  uint16_t transform_id ;
//  uint8_t version;
//  uint8_t current;
//  uint8_t section_number ; 
//  uint8_t last_section_number; 
//}pattable ;

#endif
