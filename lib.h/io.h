#ifndef IO_H_

#define IO_H_

#define hc05_en P04
#define KEY1 P32
#define KEY2 P35
#define KEY3 P34
#define KEY4 P54

#define cs P20
#define rw P21
#define rs0 P22
#define irq P36
#define clk P06
#define ptt P03

#define LED_RX P44
#define LED_TX P42
#define LED_PWR P41

#define BAT P15
#define vref_vcc P16
#define t4clko P06

#define x9c104_cs  P32
#define x9c104_ud  P33
#define x9c104_inc P34


void io_init(void);
void mt8880_ptt_cmd(bit state);
void hc05_cmd(bit state);
#endif

