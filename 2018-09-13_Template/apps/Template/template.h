/*
 * template.h
 *
 * Created: 2018-11-28 12:26:28
 *  Author: rolj1802
 */ 

#ifndef TEMPLATE_H_
#define TEMPLATE_H_

/*- Prototypes -------------------------------------------------------------*/
// Put your function prototypes here
void SYS_Init(void);
void init_UART(void);
char Lis_UART(void);
void Ecris_UART(char data);
void Ecris_UART_string(char const * data, ...);
void init_timer1(void);
void lectureEtatStation();



#endif /* template_H_ */