#include "sys.h"
#include "phy.h"
#include <stdio.h>
#include <stdarg.h>
#include "template.h"
#include "uart.h"
#include <string.h>

// Put your function implementations here

/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{
	char receivedUart = 0;

	read_station_status();
}

uint32_t a = 0;

/*************************************************************************//**
*****************************************************************************/
int main(void)
{
	
	SYS_Init();
	while (1)
	{
		PHY_TaskHandler(); //stack wireless: va vérifier s'il y a un paquet recu
		APP_TaskHandler(); //l'application principale roule ici
	}
}

//FONCTION COMM
/*************************************************************************//**
*****************************************************************************/

void read_station_status(void)
{
	if(tot_overflow_flag == 1)
	{
		Ecris_UART_string("Impossible de se connecter a la station %u\n\r\n\r",station_id);
		database[station_id-1].station_status = 'd';
		
		if (timeout_flag == 1)
		{
			if(station_id < NB_STATIONS)
			station_id ++;
			else
			station_id = 1;
		}
		
		timeout_flag = 1;
				
		pack(station_id);
		Ecris_UART_string("Read station %u status...\n\r",station_id);
		request_station_status();
				
		tot_overflow_flag = 0;
		
		Ecris_UART_string("\n\r");
	}
		
	if(confirm_station_status() == 1)
	{
		unpack();
		command_station();
		
		timeout_flag = 0;
		if(station_id < NB_STATIONS)
			station_id ++;
		else
			station_id = 1;
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
			Ecris_UART_string( "new trame! size: %d, RSSI: %ddBm\n\r", ind.size, ind.rssi );
			Ecris_UART_string( "contenu: %s\n\r", ind.data );
			
			uint32_t checksum_32bits = (ind.data[20] << 24) + (ind.data[21] << 16) + (ind.data[22] << 8) + ind.data[23];
			if (checksum(ind.data, 20) == checksum_32bits)
			{
				Ecris_UART_string("Signal sans erreur!\n\r");
			}
			else
			{
				Ecris_UART_string("Erreur dans la trame detectee!\n\r");
				receivedWireless = 0;
				return 0;
			}
			
			char bufPrefix[PREFIX_LEN] = "P1S6GE";
			if(memcmp(bufPrefix,ind.data,PREFIX_LEN) == 0)
			{
				Ecris_UART_string("Prefixe reconnu!\n\r");
			}
			else
			{
				Ecris_UART_string("Prefixe non reconnu!\n\r");
				receivedWireless = 0;
				return 0;
			}
			
			if(ind.data[6] == station_id + '0')
			{
				Ecris_UART_string("Station %c est connectee!\n\r",ind.data[6]);
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
	database[station_id-1].actuator_status = ind.data[7];
	database[station_id-1].feedback_status = ind.data[8];
	database[station_id-1].sensor_status = ind.data[9];
	database[station_id-1].station_status = 'u';
}


void command_station(void)
{
	if(database[station_id-1].feedback_status == 'c' && database[station_id-1].sensor_status == 'c')
		database[station_id-1].actuator_status = 'o';
	if(database[station_id-1].feedback_status == 'o' && database[station_id-1].sensor_status == 'o')
		database[station_id-1].actuator_status = 'c';
	
	pack();
	request_station_status();
		
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
init_timer1();
init_data();
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
if(tot_overflow >= 2)
{
	tot_overflow = 0;
	tot_overflow_flag = 1;
}

}

