/*
 * uart.h
 *
 * Created: 2018-12-03 11:01:50
 *  Author: rolj1802
 */ 




#ifndef UART_H_
#define UART_H_

/*- Prototypes -------------------------------------------------------------*/
void init_UART(void);
char Lis_UART(void);
void Ecris_UART(char data);
void Ecris_UART_string(char const * data, ...);



#endif /* UART_H_ */