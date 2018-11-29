#include "sys.h"
#include "phy.h"
#include <stdio.h>
#include <stdarg.h>
#include "template.h"
#include <string.h>

// Put your function implementations here

/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{
	char receivedUart = 0;

	
	
	read_station_status();
	receivedUart = Lis_UART();  
	if(receivedUart)		//est-ce qu'un caractere a été recu par l'UART?
	 {
		 Ecris_UART(receivedUart);	//envoie l'echo du caractere recu par l'UART
	  
			/*if(receivedUart == 'o')	//est-ce que le caractere recu est 'a'? 
			{
			uint8_t demonstration_string[128] = "1111111111"; //data packet bidon
			Ecris_Wireless(demonstration_string, 10); //envoie le data packet; nombre d'éléments utiles du paquet à envoyer
			}
			if(receivedUart == 'f')	//est-ce que le caractere recu esst 'a'? 
			{
			uint8_t demonstration_string[128] = "0000000000"; //data packet bidon
			Ecris_Wireless(demonstration_string, 10); //envoie le data packet; nombre d'éléments utiles du paquet à envoyer*/
	}
	receivedWireless = 0; 
  
}

uint32_t a = 0;

/*************************************************************************//**
*****************************************************************************/
int main(void)
{
	
	SYS_Init();
	init_timer1();
	init_data();
	
	DDRD |= 1 << DDRD4;
	PORTD |= 1 << PORTD4;
	while (1)
	{
		PHY_TaskHandler(); //stack wireless: va vérifier s'il y a un paquet recu
		APP_TaskHandler(); //l'application principale roule ici
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
//
void init_timer1(void)
{
	// set up timer with prescaler = 256
	TCCR1A &= 0;
	TCCR1B |= 0x03;
	
	// initialize counter
	TCNT1 = 0;

	// enable overflow interrupt
	TIMSK1 |= (1 << TOIE1);

	// enable global interrupts
	sei();

	// initialize overflow counter variable
	tot_overflow = 0;
	tot_overflow_flag = 0;
	station_id = 1;
}

ISR(TIMER1_OVF_vect)
{
// TIMER1 overflow interrupt service routine
// called whenever TCNT0 overflows

	// keep a track of number of overflows
	tot_overflow++;
	if(tot_overflow >= NB_STATIONS)
	{
		tot_overflow = 0;
		tot_overflow_flag = 1;
	}
	
}


//FONCTION COMM
/*************************************************************************//**
*****************************************************************************/

void read_station_status(void)
{
	if(tot_overflow_flag == 1)
	{
		
		pack(station_id);
		Ecris_UART_string("Read station %u status...\n\r",station_id);
		request_station_status();
		if(confirm_station_status() == 1)
			{
				unpack();
			}
			

		if(station_id < NB_STATIONS)
			station_id ++;
		else
			station_id = 1;
		
		tot_overflow_flag = 0;
		
		Ecris_UART_string("\n\r");
	}
}

void request_station_status(void)
{
	Ecris_Wireless(transmit_buff, sizeof(transmit_buff));
	Ecris_UART_string("Transmit : %s\n\r",transmit_buff);
}

int confirm_station_status(void)
{
	if(receivedWireless == 1) //est-ce qu'un paquet a été recu sur le wireless? 
	{
		Ecris_UART_string( "\n\rnew trame! size: %d, RSSI: %ddBm\n\r", ind.size, ind.rssi );
		Ecris_UART_string( "contenu: %s\n\r", ind.data );
	
		char bufPrefix[PREFIX_LEN] = "S6GEP1";
		if(memcmp(bufPrefix,ind.data,PREFIX_LEN) == 0)
		{
				Ecris_UART_string("Prefixe reconnu!\n\r");
				if(ind.data[6] == station_id + '0')
				{
					Ecris_UART_string("Station %c est connectee!\n\r",ind.data[6]);
					return 1;
				}		
		}
		else
		{
			Ecris_UART_string("Prefixe non reconnu!\n\r");
			return 0;
		}
	
	}
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
	//memcpy(transmit_buff+20,database[station_id-1].checksumValue,sizeof(uint32_t));
	transmit_buff[20] = (database[station_id-1].checksumValue >> 24) & 0xFF;
	transmit_buff[21] = (database[station_id-1].checksumValue >> 16) & 0xFF;
	transmit_buff[22] = (database[station_id-1].checksumValue >> 8) & 0xFF;
	transmit_buff[23] = database[station_id-1].checksumValue & 0xFF;
	
}

void unpack()
{
	/*database[station_id-1].prefix = receive_buff[0]; 
	database[station_id-1].station_id = receive_buff[6];
	database[station_id-1].actuator_status = receive_buff[7];
	database[station_id-1].feedback_status = receive_buff[8];
	database[station_id-1].sensor_status = receive_buff[9];
	database[station_id-1].station_status = receive_buff[10];
	database[station_id-1].reserved = receive_buff[11];*/
}

void init_data(void)
{
	char bufPrefix[PREFIX_LEN] = "S6GEP1";
	char bufReserved[RESERVED_LEN] = "000000000";
	
	for(int i = 0; i < NB_STATIONS; i++)
	{
		memcpy((void *)database[i].prefix,(void *)bufPrefix,sizeof(bufPrefix));
		database[i].station_id = i+1;
		database[i].actuator_status = 'c';
		database[i].feedback_status = 'c';
		database[i].sensor_status = 'c';
		database[i].station_status = 'd';
		memcpy((void *)database[i].reserved,(void *)bufReserved,sizeof(bufReserved));
		database[i].checksumValue = 0;
	}
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


