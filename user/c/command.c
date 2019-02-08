#include"STC15.h"
#include"command.h"
#include"delay.h"
#include"uart.h"
#include"eeprom.h"
#include"adc.h"
#include"mt8880.h"
#include"string.h"
#include"stdlib.h"
#include"io.h"
#include"INTRINS.h"
#include"user_timer.h"
#include"pro.h"

extern systemstatus_typedef* systemstatus;

extern unsigned char uart1_rec;
extern unsigned char uart3_rec;
extern unsigned char console_rec;

extern bit console_recflag;

unsigned char *cmd_input = NULL;

extern bit uart1_transflag, uart3_transflag;

extern xdata unsigned int mt8880_send_countup = 0x00;
extern xdata unsigned int systick_very_low_tick_counter;
extern xdata unsigned int short_string_counter;

unsigned char int_to_ascii(unsigned int input);

void command(unsigned char* cmd_input)
{
  unsigned char* mt8880_short_string = NULL;
  xdata unsigned int mt8880_short_string_input_counter = 0;

  if(cmd_input != NULL)
    {

      if((strcmp("reboot", cmd_input)) == 0)
        {
          send_string_to_console("Reboot!\n");
          delayms(1500);
          IAP_CONTR = 0x60;
          free(cmd_input);
          while(1);
        }


      if((strcmp("mt8880 send byte", cmd_input)) == 0)
        {
          send_string_to_console("input\n");
          console_rec = 0x00;
          while(console_recflag == 0);
          console_recflag = 0;
          send_string_to_console("mt8880 sending bytes!\n");

          mt8880_send_sequence_byte(&console_rec, 0x01);

          send_string_to_console("mt8880 tone sent!\n");
          console_rec = 0x00;
          free(cmd_input);
          
          systemstatus->system_state = system_state_listen;
          return;
        }

      if((strcmp("hc05 on", cmd_input)) == 0)
        {
          hc05_cmd(set_bit);
          send_string_to_console("HC50 ON\n");
          free(cmd_input);
          
          systemstatus->system_state = system_state_listen;
          return;
        }


      if((strcmp("hc05 off", cmd_input)) == 0)
        {
          hc05_cmd(reset_bit);
          send_string_to_console("HC50 OFF\n");
          free(cmd_input);
          
          systemstatus->system_state = system_state_listen;
          return;
        }


      if((strcmp("pd mode", cmd_input)) == 0)
        {
          send_string_to_console("powerdown!\n");
          delayms(2000);
          hc05_cmd(reset_bit);
          //关闭ADC电源，关闭蓝牙电源，关闭TL431电源，晶体管进入off状态
          //由于mt8880工作需要tclko4工作 所以实际上此模式不可用
          free(cmd_input);
          PCON = 0x02;
          _nop_();
          _nop_();
          
          systemstatus->system_state = system_state_listen;
          return;
        }



      if((strcmp("idle mode", cmd_input)) == 0)
        {
          send_string_to_console("Idle mode!\n");
          delayms(2000);
          hc05_cmd(reset_bit);
          timer0_cmd(reset_bit);
          vref_vcc = 0;
          //关闭T0中断，关闭蓝牙电源，关闭TL431电源，晶体管进入off状态
          //此模式1mA左右功耗
          free(cmd_input);
          
          delayms(1000);
          
          _nop_();
          _nop_();
          PCON = 0x01;
          _nop_();
          _nop_();

          timer0_cmd(set_bit); //记得开启否则无法喂狗
          vref_vcc = 1;

          send_string_to_console("Idle mode out!\n");
          
          systemstatus->system_state = system_state_listen;
          return;
        }


      /*
        测试用数据发送
      */
      if((strcmp("send", cmd_input)) == 0)
        {
          send_string_to_console("mt8880 send string!\n");

          mt8880_send_string("hhhhhhhhhh");

          send_string_to_console("mt8880 tone sent!\n");

          free(cmd_input);
          
          systemstatus->system_state = system_state_listen;
          return;
        }

        
      /*
        短报文发送 最长512个字节
        可以将字符获取函数与命令获取函数整合
        unsigned char* get_command(unsigned char* input, unsigned char max_lenth, unsigned int wait_time)
      */
      if((strcmp("send_short_string", cmd_input)) == 0)
        {
          console_recflag = 0;
          mt8880_short_string = calloc(515,sizeof(unsigned char));
          if(mt8880_short_string == NULL)
          {
            send_string_to_console("fuck you 233\n");
            while(1);
          }
                    
          
          send_string_to_console("mt8880 input:\n");
          
          console_rec = 0;
          console_recflag = 0;
          
          while(console_recflag == 0); //等待用户输入第一个字符
          short_string_counter = 0;
          
          while((console_rec != '\0') && (short_string_counter < 1000))
          {
            mt8880_short_string[mt8880_short_string_input_counter] = console_rec;
            console_recflag = 0;
            mt8880_short_string_input_counter++;

            if(short_string_counter < 1000)
            {
              short_string_counter = 0x00;
            }
          
            
          if(mt8880_short_string_input_counter > 512)
            {
              send_string_to_console("Over length !\n");
              free(cmd_input);
              free(mt8880_short_string);
              console_rec = 0;
              console_recflag = 0;
							systemstatus->system_state = system_state_listen;
              return;
            }
          
            while((console_recflag == 0) && (short_string_counter < 1000));
          
          }
          
          send_string_to_console("sending...\n");
          
          mt8880_send_string(mt8880_short_string);
          
          send_string_to_console("mt8880 tone sent!\n");

          free(cmd_input);
          free(mt8880_short_string);
          console_rec = 0;
          console_recflag = 0;          
					systemstatus->system_state = system_state_listen;
          return;
        }
         
        

      if((strcmp("bat", cmd_input)) == 0)
        {
          send_string_to_console("BAT voltage is:");

          send_byte_to_console(int_to_ascii(systemstatus->battery_voltage / 1000));
          send_byte_to_console(int_to_ascii((systemstatus->battery_voltage / 100) % 10));
          send_byte_to_console(int_to_ascii((systemstatus->battery_voltage % 100) / 10));
          send_byte_to_console(int_to_ascii(systemstatus->battery_voltage % 10));

          send_string_to_console("mv\n");
          free(cmd_input);
					systemstatus->system_state = system_state_listen;
          return;
        }


 
      send_string_to_console("Command incorrect !\n");
      delayms(10);
      console_rec = 0x00;
      free(cmd_input);
			systemstatus->system_state = system_state_listen;
			
    }
}



unsigned char* get_command(void)
{
  unsigned char input_counter = 0x00;

#ifdef USE_UART1_AS_COMMAND_CONSOLE

  if(console_recflag != 0)
    {
      systemstatus->system_state = system_state_command;
      console_recflag = 0;
      cmd_input = calloc(52, sizeof(char));
      if(cmd_input == NULL)
        {
          uart1_sendstring("No mems for input command !\n");
          free(cmd_input);
					systemstatus->system_state = system_state_listen;
          return NULL;
        }

      while(uart1_rec != '\0')
        {
          cmd_input[input_counter] = uart1_rec;
          console_recflag = 0;
          input_counter ++;
          if(input_counter > 50)
            {
              uart1_sendstring("Command over length !\n");
              free(cmd_input);
							systemstatus->system_state = system_state_listen;
              return NULL;
            }
          while(console_recflag == 0);
        }

      uart1_rec = 0x00;
      console_rec = 0;
      console_recflag = 0;
      return cmd_input;

    }

#endif


#ifdef USE_UART3_AS_COMMAND_CONSOLE

  if(console_recflag != 0)
    {
      systemstatus->system_state = system_state_command;
      console_recflag = 0;
      cmd_input = calloc(52, sizeof(char));
      if(cmd_input == NULL)
        {
          uart3_sendstring("No mems for input command !\n");
          free(cmd_input);
					systemstatus->system_state = system_state_listen;
          return NULL;
        }

      while(uart3_rec != '\0')
        {
          cmd_input[input_counter] = uart3_rec;
          console_recflag = 0;
          input_counter ++;
          if(input_counter > 50)
            {
              uart3_sendstring("Command over length !\n");
              free(cmd_input);
							systemstatus->system_state = system_state_listen;
              return NULL;
            }
          while(console_recflag == 0);
        }

      uart3_rec = 0x00;
      console_rec = 0;
      console_recflag = 0;
      return cmd_input;

    }


#endif


  return NULL;
}

void send_string_to_console(unsigned  char* p)
{
#ifdef USE_UART1_AS_COMMAND_CONSOLE

  uart1_sendstring(p);

#endif

#ifdef USE_UART3_AS_COMMAND_CONSOLE

  uart3_sendstring(p);

#endif

}

void send_byte_to_console(unsigned char data_input)
{
#ifdef USE_UART1_AS_COMMAND_CONSOLE

  uart1_sendbyte(data_input);

#endif

#ifdef USE_UART3_AS_COMMAND_CONSOLE

  uart3_sendbyte(data_input);

#endif

}

void service(void)
{
  mt8880_receive();
  system_1000ms_tick_mission();
}

unsigned char int_to_ascii(unsigned int input)
{
  switch(input)
    {
    case 0:
      return '0';
    case 1:
      return '1';
    case 2:
      return '2';
    case 3:
      return '3';
    case 4:
      return '4';
    case 5:
      return '5';
    case 6:
      return '6';
    case 7:
      return '7';
    case 8:
      return '8';
    case 9:
      return '9';
    }
  return 'E';
}

void system_1000ms_tick_mission(void)
{
  xdata adc_cmdtypedef* adc_cmd = NULL;
	
	systemstatus->system_state = system_state_command;

  if(systick_very_low_tick_counter >= 1000)
    {
      
      systick_very_low_tick_counter = 0;
      
      adc_cmd = calloc(1,sizeof(adc_cmdtypedef));
      
      if(adc_cmd == NULL)
        {
          send_string_to_console("Error code 2013\n");
          while(1);
        }

      adc_cmd->speed = adc_speed_veryslow;
      adc_cmd->cal = set_bit;
      adc_cmd->vcc = 2500;
      adc_cmd->accuracy = adc_accuracy_10bit;
      adc_cmd->channel = 7;
      systemstatus->battery_voltage = adc_on(adc_cmd);

      adc_cmd->cal = reset_bit;
      adc_cmd->channel = 5;
      adc_cmd->vcc = systemstatus->battery_voltage;
      systemstatus->battery_voltage = adc_on(adc_cmd);
        
      free(adc_cmd);

      systemstatus->system_state = system_state_listen;
    }

}
