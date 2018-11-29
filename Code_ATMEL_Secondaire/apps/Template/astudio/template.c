/*- Includes ---------------------------------------------------------------*/
#include "sys.h"
#include "phy.h"
#include <stdio.h>
#include <stdarg.h>
#include "template.h"

/*- Definitions ------------------------------------------------------------*/
// Put your preprocessor definitions here

/*- Types ------------------------------------------------------------------*/
// Put your type definitions here

/*- Implementations --------------------------------------------------------*/

// Put your function implementations here

/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{
  char receivedUart = 0;

  receivedUart = Lis_UART();  
  /*if(receivedUart)		//est-ce qu'un caractere a été recu par l'UART?
  {
	  Ecris_UART(receivedUart);	//envoie l'echo du caractere recu par l'UART

	  if(receivedUart == '1')	//est-ce que le caractere recu est 'a'? 
		{
		uint8_t demonstration_string[128] = "1"; //data packet bidon
		Ecris_Wireless(demonstration_string, 1); //envoie le data packet; nombre d'éléments utiles du paquet à envoyer
		}
		
	if(receivedUart == '2')	//est-ce que le caractere recu est 'a'?
	{
		uint8_t demonstration_string[128] = "2"; //data packet bidon
		Ecris_Wireless(demonstration_string, 1); //envoie le data packet; nombre d'éléments utiles du paquet à envoyer
	}
	
	if(receivedUart == '3')	//est-ce que le caractere recu est 'a'?
	{
		uint8_t demonstration_string[128] = "3"; //data packet bidon
		Ecris_Wireless(demonstration_string, 1); //envoie le data packet; nombre d'éléments utiles du paquet à envoyer
	}
	
	if(receivedUart == '4')	//est-ce que le caractere recu est 'a'?
	{
		uint8_t demonstration_string[128] = "4"; //data packet bidon
		Ecris_Wireless(demonstration_string, 1); //envoie le data packet; nombre d'éléments utiles du paquet à envoyer
	}
		
  }*/
  
  if(receivedWireless == 1) //est-ce qu'un paquet a été recu sur le wireless? 
  {
	Ecris_UART_string( "\n\rnew trame! size: %d, RSSI: %ddBm\n\r", ind.size, ind.rssi );
	Ecris_UART_string( "\n\rcontenu: %s", ind.data );
	
	if(ind.data[6] == (1 + '0'))
	{
		Ecris_Wireless("S6GEP11cccu0000000000000",24);
		Ecris_UART_string("S6GEP11cccu0000000000000\n\r");
	}	
	receivedWireless = 0; 
  }
}



/*************************************************************************//**
*****************************************************************************/
int main(void)
{
  SYS_Init();
  //ANT_DIV |= (1 << ANT_EXT_SW_EN);
  //ANT_DIV |= (1 << ANT_CTRL0);
  //ANT_DIV &= ~(1 << ANT_CTRL1);
  
  DDRD |= 1 << DDRD4;
  
   
  while (1)
  {
    PHY_TaskHandler(); //stack wireless: va vérifier s'il y a un paquet recu
    APP_TaskHandler(); //l'application principale roule ici
	//Ecris_UART_string( "Count : %d\n\r", tot_overflow);
	//Ecris_UART_string( "Count : %u\n\r", TCNT1);
	
  }
}







//FONCTION D'INITIALISATION
/*************************************************************************//**
*****************************************************************************/
void SYS_Init(void)
{
receivedWireless = 0;
wdt_disable(); 
init_UART();
PHY_Init(); //initialise le wireless
PHY_SetRxState(1); //TRX_CMD_RX_ON
}


//FONCTIONS POUR L'UART
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


