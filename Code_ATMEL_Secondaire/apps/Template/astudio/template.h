/*
 * template.h
 *
 * Created: 2018-11-29 12:35:57
 *  Author: royt2801
 */ 


#ifndef TEMPLATE_H_
#define TEMPLATE_H_

/*- Prototypes -------------------------------------------------------------*/
// Put your function prototypes here
char Lis_UART(void);
void Ecris_UART(char data);
void Ecris_UART_string(char const * data, ...);
void init_UART(void);
void SYS_Init(void);
void init_timer1(void);

/*- Variables --------------------------------------------------------------*/
// Put your variables here
uint8_t receivedWireless;	//cette variable deviendra 1 lorsqu'un nouveau paquet aura été recu via wireless (et copié dans "PHY_DataInd_t ind"
//il faut la mettre a 0 apres avoir géré le paquet; tout message recu via wireless pendant que cette variable est a 1 sera jeté

// global variable to count the number of overflows
volatile uint8_t tot_overflow;

PHY_DataInd_t ind; //cet objet contiendra les informations concernant le dernier paquet qui vient de rentrer



#endif /* TEMPLATE_H_ */