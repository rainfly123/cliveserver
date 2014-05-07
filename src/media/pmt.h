/** 
* @file pmt.h
* @brief parse pmt table
* @author xie changcai
* @date 2008-09-06
*/
#ifndef __PMT_H__
#define __PMT_H__
#include <stdint.h> 

typedef struct {

uint8_t  stream_type ;
uint16_t es_pid ;

}es_list ;

typedef struct {

uint16_t program_num ;
uint16_t pcr_pid ;
uint8_t  total_es ;
es_list alles[5] ; // video audio teltext,subtitle 最多es数量 

}pmt_list;

/** 
* @brief parset the pmt table .
* 
* @param fd file descriptor 
* @param pmt_pid pmts' pid 
* 
* @return pml_list  
*/

pmt_list pmtparse(int fd,uint16_t pmt_pid) ;

#endif
