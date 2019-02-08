#include"STC15.h"
#include"INTRINS.h"
#include"user_timer.h"


void timer0_init(void)		//1毫秒@30.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x3C;		//设置定时初值
	TH0 = 0xF6;		//设置定时初值
	TF0 = 0;		//清除TF0标志
  
  EA = 1;
  ET0 = 1;
  TR0 = 1;		//定时器0开始计时
}

void timer0_cmd(bit state)
{
  if(state == set_bit)
    {
      ET0 = 1;
      TR0 = 1;
    }
  else
    {
      ET0 = 0;
      TR0 = 0;
    }
}
 

void timer3_init(void) // 0.5u @30Mhz
{
  T4T3M |= 0x02;		//定时器时钟1T模式
  T3L = 0xFC;		//设置定时初值
  T3H = 0xFF;		//设置定时初值
  T4T3M |= 0x08;		//定时器3开始计时
  T4T3M &= 0xfe;  //关闭定时器3对外时钟输出

  IE2 &= 0xdf; //关闭定时器3的中断
}

void timer3_cmd(bit state)
{
  if(state == set_bit)
    {
      T4T3M |= 0x01;
    }
  else
    {
      T4T3M &= 0xfe;
    }
}

void timer4_init(void)
{
  T4T3M |= 0x20;		//定时器时钟1T模式
  T4L = 0xFC;		//设置定时初值
  T4H = 0xFF;		//设置定时初值
  T4T3M |= 0x80;		//定时器4开始计时
  T4T3M &= 0xef;  //关闭定时器4对外时钟输出

  IE2 &= 0xbf; //关闭定时器4的中断
}

void timer4_cmd(bit state)
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

void timer1_init(void) //100us @30.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x48;		//设置定时初值
	TH1 = 0xF4;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 0;		//定时器1停止计时
  
  EA = 1; //开启总中断
  ET1 = 0; //关闭定时器1中断
}

void timer1_cmd(bit state)
{
  if(state == set_bit)
  {
    TR1 = 1;
    ET1 = 1;
  }
  else
  {
		timer1_init();
  }
}

