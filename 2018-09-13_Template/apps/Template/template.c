#include "sys.h"
#include "phy.h"
#include <stdio.h>
#include <stdarg.h>
#include "template.h"
#include <string.h>

/*- Variables --------------------------------------------------------------*/
volatile uint8_t tot_overflow;       // global variable to count the number of overflows
volatile uint8_t tot_overflow_flag;

volatile uint32_t station_id;

uint8_t receivedWireless;	//cette variable deviendra 1 lorsqu'un nouveau paquet aura été recu via wireless (et copié dans "PHY_DataInd_t ind"
//il faut la mettre a 0 apres avoir géré le paquet; tout message recu via wireless pendant que cette variable est a 1 sera jeté


PHY_DataInd_t ind; //cet objet contiendra les informations concernant le dernier paquet qui vient de rentrer

/*- Structures -------------------------------------------------------------*/
typedef struct entete
{
	char prefix[6];
	uint32_t station_id;
	char actuator_status;
	char feedback_status;
	char sensor_status;
	char reserved[10];
}entete;

entete database[10];

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

  if(receivedWireless == 1) //est-ce qu'un paquet a été recu sur le wireless? 
  {
	Ecris_UART_string( "\n\rnew trame! size: %d, RSSI: %ddBm\n\r", ind.size, ind.rssi );
	Ecris_UART_string( "contenu: %s", ind.data );	
	receivedWireless = 0; 
	
	if (ind.data[0] == '1') //lumiere éteinte pas de soleil
	{
		uint8_t command_string[128] = "1";
		Ecris_Wireless(command_string, 1);
		PORTD &= 0xEF; 
	}
  }
}

uint32_t a = 0;

/*************************************************************************//**
*****************************************************************************/
int main(void)
{
	
	SYS_Init();
	init_timer1();
	init_entete();

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


// TIMER1 overflow interrupt service routine
// called whenever TCNT0 overflows
ISR(TIMER1_OVF_vect)
{
	// keep a track of number of overflows
	tot_overflow++;
	if(tot_overflow == 2)
	{
		tot_overflow = 0;
		tot_overflow_flag = 1;
		/*uint8_t demonstration_string[128] = "1"; //data packet bidon
		Ecris_Wireless(demonstration_string, 1);
		Ecris_UART_string("Envoi de paquet\n");
		tot_overflow = 0;*/
	}
	
}

void read_station_status(void)
{
	if(tot_overflow_flag == 1)
	{
		Ecris_UART_string("Read station %u status...\n\r",station_id);
		pack();
		request_station_status();
		int a = confirm_station_status();
		
		
		
		
		
		
		if(station_id != 10)
			station_id ++;
		else
			station_id = 1;
		
		tot_overflow_flag = 0;
	}
}

void request_station_status(void)
{
	
}

int confirm_station_status(void)
{
	return 1;
}

void pack(void)
{
	
}

void unpack(void)
{
	
}

void init_entete(void)
{
	char buf[6] = "S6GEP1";
	char buf2[10] = "0000000000";
	for(int i = 0; i < 10; i++)
	{
		memcpy((void *)database[i].prefix,(void *)buf,sizeof(buf));
		database[i].station_id = i+1;
		database[i].actuator_status = 'c';
		database[i].feedback_status = 'c';
		database[i].sensor_status = 'c';
		memcpy((void *)database[i].reserved,(void *)buf2,sizeof(buf2));
	}
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


