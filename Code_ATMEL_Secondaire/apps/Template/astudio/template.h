/*
 * template.h
 *
 * Created: 2018-11-29 12:35:57
 *  Author: royt2801
 */ 


#ifndef TEMPLATE_H_
#define TEMPLATE_H_

/*- Defines ---------------------------------------------------------------*/
#define NB_STATIONS 1
#define STATION_ID 1
#define TRANSMIT_BUF_LEN 24
#define PREFIX_LEN 6
#define RESERVED_LEN 9

/*- Variables --------------------------------------------------------------*/
uint8_t station_id;
char transmit_buff[TRANSMIT_BUF_LEN];
uint8_t receivedWireless;	//cette variable deviendra 1 lorsqu'un nouveau paquet aura été recu via wireless (et copié dans "PHY_DataInd_t ind"
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
// Put your function prototypes here
char Lis_UART(void);
void Ecris_UART(char data);
void Ecris_UART_string(char const * data, ...);
void init_UART(void);
void SYS_Init(void);
void GPIO_init(void);

void init_data(void);
int confirm_station_status(void);
void read_input(void);
void pack(void);
void unpack(void);
void send_station_status(void);
uint32_t checksum(char* buffer, uint32_t size);

#endif /* TEMPLATE_H_ */