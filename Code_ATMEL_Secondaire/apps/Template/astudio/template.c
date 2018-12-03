/*- Includes ---------------------------------------------------------------*/
#include "sys.h"
#include "phy.h"
#include <stdio.h>
#include <stdarg.h>
#include "template.h"


static void APP_TaskHandler(void)
{
  char receivedUart = 0;
  
  if(confirm_station_status() == 1)
  {
	    database[station_id-1].actuator_status = ind.data[7];
		unpack();
		read_input();
		pack();
		send_station_status();
		if(database[0].actuator_status == 'c')
		{
			PORTD &= ~(1 << PORTD4);
		}
		else
		{	
			PORTD |= (1 << PORTD4);
		}
  }
}



/*************************************************************************//**
*****************************************************************************/
int main(void)
{
	station_id = STATION_ID;
	SYS_Init();
	GPIO_init();
	init_data();
	
	while (1)
	{
		PHY_TaskHandler(); //stack wireless: va vérifier s'il y a un paquet recu
		APP_TaskHandler(); //l'application principale roule ici	
	}
}


void init_data(void)
{
	char bufPrefix[PREFIX_LEN] = "P1S6GE";
	char bufReserved[RESERVED_LEN] = "000000000";
	
	for(int i = 0; i < NB_STATIONS; i++)
	{
		memcpy((void *)database[i].prefix,(void *)bufPrefix,sizeof(bufPrefix));
		database[i].station_id = i+1;
		database[i].actuator_status = 'c';
		database[i].feedback_status = 'c';
		database[i].sensor_status = 'c';
		database[i].station_status = 'u';
		memcpy((void *)database[i].reserved,(void *)bufReserved,sizeof(bufReserved));
		database[i].checksumValue = 0;
	}
}

void read_input(void)
{
	if (((PIND & (1 << PIND0)) >> PIND0) == 1) //feedback
	{
		database[0].feedback_status = 'o';
	}
	else
	{
		database[0].feedback_status = 'c';
	}
		
	if (((PIND & (1 << PIND6)) >> PIND6) == 1) //Sensor
	{
		database[0].sensor_status = 'o';
	}
	else
	{
		database[0].sensor_status = 'c';
	}
		
}

int confirm_station_status(void)
{
	{
		if(receivedWireless == 1) //est-ce qu'un paquet a été recu sur le wireless?
		{
			Ecris_UART_string( "new trame! size: %d, RSSI: %ddBm\n\r", ind.size, ind.rssi );
			Ecris_UART_string( "contenu: %s\n\r", ind.data );
			
			char bufPrefix[PREFIX_LEN] = "P1S6GE";
			if(memcmp(bufPrefix,ind.data,PREFIX_LEN) == 0)
			{
				Ecris_UART_string("Requete reconnue!\n\r");
				if(ind.data[6] == STATION_ID +'0')
				{
					Ecris_UART_string("Station principale %c est connectee!\n\r",ind.data[6]);
					
					receivedWireless = 0;
					return 1;
				}
				else
				{
					Ecris_UART_string("Mauvaise station connectee!\n\r",ind.data[6]);
					receivedWireless = 0;
					return 0;
				}
			}
			else
			{
				Ecris_UART_string("Prefixe non reconnu!\n\r");
				receivedWireless = 0;
				return 0;
			}
		}
	}
	return 0;
}		
		
void pack()
{
	memcpy(transmit_buff,database[station_id-1].prefix,sizeof(database[station_id-1].prefix)); //Prefix de 6 char
	transmit_buff[6] = database[station_id-1].station_id + '0';
	transmit_buff[7] = database[station_id-1].actuator_status;
	transmit_buff[8] = database[station_id-1].feedback_status;
	transmit_buff[9] = database[station_id-1].sensor_status;
	transmit_buff[10] = database[station_id-1].station_status;
	memcpy(transmit_buff+11,database[station_id-1].reserved,sizeof(database[station_id-1].reserved)); //Reserved 9 char
	database[station_id-1].checksumValue = checksum(transmit_buff,20);
	
	transmit_buff[20] = (database[station_id-1].checksumValue >> 24) & 0xFF;
	transmit_buff[21] = (database[station_id-1].checksumValue >> 16) & 0xFF;
	transmit_buff[22] = (database[station_id-1].checksumValue >> 8) & 0xFF;
	transmit_buff[23] = database[station_id-1].checksumValue & 0xFF;
	
}

void unpack()
{
	//database[station_id-1].actuator_status = ind.data[7];
}

void send_station_status(void)
{
	Ecris_Wireless(transmit_buff, sizeof(transmit_buff));
	Ecris_UART_string("Transmit : %s\n\r",transmit_buff);
	Ecris_UART_string("\n\r");
}

uint32_t checksum(char* buffer, uint32_t size)
{
	uint32_t checksumValue = 0;
	for(int i = 0; i < size; i++)
	{
		checksumValue += buffer[i];
	}
	return checksumValue;
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

void GPIO_init(void)
{
	//PORTD |= (1<<PORTD0) | (1<<PORTD6);
	DDRD |= (1 << DDRD4); //GPIO bitches
	DDRD &= ~((1 << DDRD0) | (1 << DDRD6)); //GPIO bitches
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

