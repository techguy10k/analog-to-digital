/*
*  文件名称：IAP15W4K32S4_IT.c
*  文件作用：整个工程所有的中断服务函数都在这里写
*  文件用法：无
*/
#include"IAP15W4K32S4_IT.h"
#include"STC15.h"
#include"mt8880.h"
#include"stdlib.h"
#include"user_timer.h"
#include"io.h"
#include"pro.h"
#include"adc.h"
#include"sort.h"
#include"x9c104.h"

#define S3RI  0x01              //S3CON.0
#define S3TI  0x02              //S3CON.1

extern systemstatus_typedef* systemstatus;

unsigned char uart1_rec = 0;
unsigned char uart3_rec = 0;
unsigned char console_rec = 0;

xdata unsigned int mt8880_counter = 0;
xdata unsigned int led_pwr_counter = 0;
xdata unsigned int systick_very_low_tick_counter = 0;
xdata unsigned int short_string_counter = 0;

unsigned char mt8880_rec_raw = 0;
unsigned char mt8880_rec = 0;

bit mt8880_get_one_byte = 0;
bit uart1_transflag = 0, uart3_transflag = 0;
bit console_recflag = 0,mt8880_recflag = 0;

unsigned char mt8880_read_receive_data_it(void);
mt8880_status_typedef mt8880_get_status_it(void);
void delay1us_it(void);
void hc05_cmd_it(bit state);
void timer4_cmd_it(bit state);


void uart1_interrupt(void) interrupt 4
{
  if (RI)
    {
			
			
      RI = 0;
      uart1_rec = SBUF;
#ifdef USE_UART1_AS_COMMAND_CONSOLE
      console_recflag = 1;
      console_rec = SBUF;
#endif
    }
  if (TI)
    {
      TI = 0;
      uart1_transflag = 1;
    }
}

void uart3_interrupt(void) interrupt 17
{
  if (S3CON & S3RI)
    {
      S3CON &= ~S3RI;         //清除S3RI位
      uart3_rec = S3BUF;
#ifdef USE_UART3_AS_COMMAND_CONSOLE
      console_recflag = 1;
      console_rec = S3BUF;
#endif
    }
  if (S3CON & S3TI)
    {
      S3CON &= ~S3TI;         //清除S3TI位
      uart3_transflag = 1;
    }
}

void systick_timer0_interrupt(void) interrupt 1  //1ms per interrupt
{
  mt8880_counter ++;
  led_pwr_counter ++;
  systick_very_low_tick_counter ++;
  short_string_counter ++;

  WDT_CONTR = 0x17;

  if(led_pwr_counter > 5000)
    {
      LED_PWR = 0;
    }
  else
    {
      LED_PWR = 1;
    }

  if(led_pwr_counter > 5200)
    led_pwr_counter = 0;

}


void mt8880_irq_int2_interrupt(void) interrupt 10
{
  mt8880_status_typedef mt8880_status;

  INT_CLKO &= 0xef; //关中断允许位的时候会顺便清除中断标志


  mt8880_status = mt8880_get_status_it();

  if((mt8880_status.mt8880_irq == 0) || (systemstatus->system_state == system_state_transmit) || (systemstatus->system_state == system_state_command))
    {
      INT_CLKO |= 0x10;
      return;
    }

  if(mt8880_status.mt8880_rec_data_full == 1)
    {
      systemstatus->system_state = system_state_recieve;
      mt8880_recflag = 1;
    }

  INT_CLKO |= 0x10; //程序结束再打开中断

}


void int0_interrupt(void) interrupt 0
{
  hc05_cmd_it(set_bit);
}

mt8880_status_typedef mt8880_get_status_it(void)
{
  unsigned char data_read;
  mt8880_status_typedef mt8880_status;

  T4T3M &= 0xef; //关闭时钟

  P2 = 0Xff;
  rs0 = 1;
  rw = 1;
  clk = 0;
  delay1us_it();

  cs = 0;
  clk = 1;
  delay1us_it();
  data_read = P2;
  clk = 0;
  delay1us_it();
  cs = 1;

  data_read = data_read >> 4;

  if(data_read & 0x01)
    mt8880_status.mt8880_irq = 1;
  if(data_read & 0x02)
    mt8880_status.mt8880_trans_data_empty = 1;
  if(data_read & 0x04)
    mt8880_status.mt8880_rec_data_full = 1;
  if(data_read & 0x08)
    mt8880_status.mt8880_delayed_steering = 1;

  T4T3M |= 0x10;

  return mt8880_status;
}

void int1_interrupt(void) interrupt 2 //蓝牙状态脚
{

}


void delay1us_it(void)  //@30Mhz
{
  unsigned char i;

  i = 5;
  while (--i);

}

unsigned char mt8880_read_receive_data_it(void)
{
  unsigned char data_read;

  timer4_cmd_it(reset_bit);

  clk = 0;

  P2 = 0Xff;
  rs0 = 0;
  rw = 1;

  cs = 0;
  clk = 1;
  delay1us_it();
  data_read = P2;
  clk = 0;
  delay1us_it();
  cs = 1;

  data_read = data_read >> 4;

  timer4_cmd_it(set_bit);

  return data_read;
}


void hc05_cmd_it(bit state)
{
  if(state == 1)
    {
      hc05_en = 1;
    }
  else
    {
      hc05_en = 0;
    }
}


void timer4_cmd_it(bit state)
{
  if(state == set_bit)
    {
      T4T3M |= 0x10;
    }
  else
    {
      T4T3M &= 0xef;
    }

}

