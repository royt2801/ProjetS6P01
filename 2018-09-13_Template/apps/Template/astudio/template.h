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

/*- Defines ---------------------------------------------------------------*/
#define NB_STATIONS 2
#define TRANSMIT_BUF_LEN 24
#define PREFIX_LEN 6
#define RESERVED_LEN 9
/*- Variables --------------------------------------------------------------*/
volatile uint8_t tot_overflow;       // global variable to count the number of overflows
volatile uint8_t tot_overflow_flag;
volatile uint8_t timeout_flag;

uint8_t station_id;
char transmit_buff[TRANSMIT_BUF_LEN];
char receive_buff[TRANSMIT_BUF_LEN];


uint8_t receivedWireless;	//cette variable deviendra 1 lorsqu'un nouveau paquet aura été recu via wireless (et copié dans "PHY_DataInd_t ind"
//il faut la mettre a 0 apres avoir géré le paquet; tout message recu via wireless pendant que cette variable est a 1 sera jeté


PHY_DataInd_t ind; //cet objet contiendra les informations concernant le dernier paquet qui vient de rentrer

/*- Structures -------------------------------------------------------------*/
typedef struct data
{
	char prefix[PREFIX_LEN];
	uint8_t station_id;
	char actuator_status;
	char feedback_status;
	char sensor_status;
	char station_status;
	char reserved[RESERVED_LEN];
	uint32_t checksumValue;
}data;

data database[NB_STATIONS];



/*- Prototypes -------------------------------------------------------------*/
void SYS_Init(void);
void init_timer1(void);

void init_data(void);
void read_station_status(void);
void request_station_status(void);
int confirm_station_status(void);
void pack();
void unpack();
void command_station(void);

uint32_t checksum(char* buffer, uint32_t size);

#endif /* template_H_ */