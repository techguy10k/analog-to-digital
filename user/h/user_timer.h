#ifndef USER_TIMER_H_

#define USER_TIMER_H_

#ifndef set_bit
#define set_bit 1
#endif

#ifndef reset_bit 
#define reset_bit 0
#endif

#ifndef T3CLKO
#define T3CLKO set_bit
#endif

#ifndef T4CLKO
#define T4CLKO set_bit
#endif

void timer0_init(void);
void timer0_cmd(bit state);

void timer1_init(void);
void timer1_cmd(bit state);

void timer3_init(void);
void timer3_cmd(bit state);

void timer4_init(void);
void timer4_cmd(bit state);


#endif
