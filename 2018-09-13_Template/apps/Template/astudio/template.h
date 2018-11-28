/*
 * template.h
 *
 * Created: 2018-11-28 12:26:28
 *  Author: rolj1802
 */ 
/*- Includes ---------------------------------------------------------------*/
#include "sys.h"
#include "phy.h"
#include <stdio.h>
#include <stdarg.h>


#ifndef TEMPLATE_H_
#define TEMPLATE_H_




/*- Prototypes -------------------------------------------------------------*/
void SYS_Init(void);
void init_UART(void);
char Lis_UART(void);
void Ecris_UART(char data);
void Ecris_UART_string(char const * data, ...);
void init_timer1(void);

void init_entete(entete *station);
void read_station_status(void);
void request_station_status(void);
int confirm_station_status(void);
void pack(void);
void unpack(void);


#endif /* template_H_ */