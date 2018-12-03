/*
 * UART.c
 *
 * Created: 2018-12-03 11:01:18
 *  Author: rolj1802
 */ 

//FONCTIONS POUR L'UART
#include "sys.h"
#include "phy.h"
#include <stdio.h>
#include <stdarg.h>
#include "uart.h"
#include <string.h>

char Lis_UART(void)
{

	char data = 0;


	if(UCSR1A & (0x01 << RXC1))
	{
		data = UDR1;
	}
	
	return data;
}

void Ecris_UART(char data)
{
	UDR1 = data;
	while(!(UCSR1A & (0x01 << UDRE1)));
}

void Ecris_UART_string(char const * data, ...)
{
	va_list args;
	char buf[256];
	unsigned int index = 0;
	
	va_start(args, data);
	vsprintf(buf, data, args);
	
	while(buf[index] != 0x00 && index<256)
	{
		Ecris_UART(buf[index]);
		index++;
	}
	
	va_end(args);
}

void init_UART(void)
{
	//baud rate register = Fcpu / (16*baud rate - 1)
	//baudrate = 9600bps //s'assurer que la fuse CLKDIV8 n'est pas activée dans les Fuses, sinon ca donne 1200bps
	UBRR1H = 0x00;
	UBRR1L = 53;
	
	UCSR1A = 0x00;
	UCSR1B = 0x18; //receiver, transmitter enable, no parity
	UCSR1C = 0x06; //8-bits per character, 1 stop bit
}


