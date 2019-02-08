/*
*  文件名称：pro.h
*  文件作用：整个工程一些全局东西的的定义
*  文件用法：无
*/
#ifndef PRO_H_
#define PRO_H_

typedef struct
{
  unsigned int battery_voltage;
  unsigned char hc05_state;
  unsigned char system_state;
} systemstatus_typedef;


//系统状态定义
#define system_state_listen    0u
#define system_state_recieve   1u
#define system_state_transmit  2u
#define system_state_command   3u


#endif

