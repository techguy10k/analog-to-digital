/*
*  文件名称：pro.c
*  文件作用：整个工程的主c文件
*  文件用法：无
*/
#include"STC15.h"
#include"INTRINS.h"
#include"string.h"
#include"stdio.h"

#include"pro.h"
#include"uart.h"
#include"delay.h"
#include"eeprom.h"
#include"io.h"
#include"adc.h"
#include"user_timer.h"
#include"command.h"
#include"stdlib.h"
#include"mt8880.h"
#include"exint.h"
#include"sort.h"
#include"x9c104.h"
#include"IAP15W4K32S4_IT.h"

//内存池的建立
unsigned char xdata malloc_mempool[2048];


//系统状态表
systemstatus_typedef* systemstatus = NULL;



//void passwd(unsigned int passwd_location);

void system_init(void)
{
  io_init();
  systemstatus = calloc(1, sizeof(systemstatus_typedef));

  //定时器2、3初始化不要调用 因为已经被串口用于产生时钟
  timer0_init(); //systick 1ms
  timer4_init(); //MT8880时钟输出
  
  exint2_init();
  exint0_init();
  exint1_init();


#ifndef USE_HARDWARE_SIMULATOR
  uart1_init();
#endif
  uart3_init();
  delayms(100);
  mt8880_bootup_setup();
  delayms(300);

  hc05_cmd(reset_bit);

  send_string_to_console("Boot up!\n");
  delayms(10);
//  send_string_to_console("Enter passwd(ASCII):\n");
//  passwd();
}

void main(void)
{		
  system_init();
  mt8880_listen();	
		
	for(;;)
	{
		command(get_command());
		service();
	}
}
