/*
 * FW_VlakecRemote.c
 *
 * Created: 22/09/2019 20:03:30
 * Author : maticpi
 */ 

#include <avr/io.h>
#include <string.h>
#include "pdlib_nrf24l01.h"
#include "LED.h"
#include "kbd.h"
#include "systime.h"
#include "UART0_IRQ.h"


int main(void)
{
	uint32_t t1;
  int status;
  char pipe;
  char temp;
  unsigned char address[5] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01};
  char data[32];

	KBD_Init();
	LED_Init();
	Systime_Init();
	UART0_Init();
	stdout=&UART0_str;
	sei();
  NRF24L01_Init();
  NRF24L01_SetAirDataRate(1); //Set 1Mbps, does not support 250kbps yet. Default is 2Mbps
  NRF24L01_SetPAGain(-18);  //default is 0. Possible values: -18, -12, -6, 0dBm
  /* Set the address */
  NRF24L01_SetRxAddress(PDLIB_NRF24_PIPE0, address);
  /* Set the packet size */
  NRF24L01_SetRXPacketSize(PDLIB_NRF24_PIPE0, 23);
  //NRF24L01_InterruptInit(GPIO_PORTE_BASE, GPIO_PIN_3, SYSCTL_PERIPH_GPIOE, INT_GPIOE);

  /* Replace with your application code */
  while (1) 
  {
    status = NRF24L01_WaitForDataRx(&pipe);

    if(PDLIB_NRF24_SUCCESS == status)
    {
	    temp = NRF24L01_GetRxDataAmount(pipe);
	    printf("Data Available in: %d\n",pipe);
	    printf("Data amount available : %d\n",temp);
	    memset(data,0x00,32);
	    status = NRF24L01_GetData(pipe, data, &temp);
	    printf("Data Read: ");
	    printf((const char*)data);
	    printf("\n");
    }

		if (Has_X_MillisecondsPassed(1000,&t1))
		{
			LED_1Tgl();
			printf(".");
		}
  }
}

