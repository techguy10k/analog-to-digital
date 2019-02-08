#ifndef COMMAND_H_

#define COMMAND_H_

#ifndef set_bit
#define set_bit 1
#endif

#ifndef reset_bit
#define reset_bit 0
#endif


void command(unsigned char*cmd_input);
unsigned char* get_command(void);
void send_string_to_console(unsigned  char* p);
void send_byte_to_console(unsigned char data_input);
void service(void);
void system_1000ms_tick_mission(void);
unsigned char int_to_ascii(unsigned int input);

#endif

