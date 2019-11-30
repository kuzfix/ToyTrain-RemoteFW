/*
 * FW_VlakecRemote.c
 *
 * Created: 22/09/2019 20:03:30
 * Author : maticpi
 */ 

#include <avr/io.h>
#include <string.h>
//#include "pdlib_nrf24l01.h"
#include "nrf24.h"
#include "LED.h"
#include "kbd.h"
#include "systime.h"
#include "UART0_IRQ.h"
#include <util/delay.h>

int WakeupReceiver();

int main(void)
{
  int oldSuccess=0;
  char temp;
  uint8_t tx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
  uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
  uint8_t data[33]="Wake";

	KBD_Init();
	LED_Init();
	Systime_Init();
	UART0_Init();
	stdout=&UART0_str;
	sei();
  
  nrf24_init();                   // init hardware pins
  nrf24_config(2,4);              // Channel #2 , payload length: 4
  nrf24_tx_address(tx_address);   // Set device addresses
  nrf24_rx_address(rx_address);

	printf("while(1):\r\n");
  while (1)
  {
    
    nrf24_send(data);                  // Automatically goes to TX mode
    while(nrf24_isSending());         // Wait for transmission to end 
    
    temp = nrf24_lastMessageStatus(); // Make analysis on last transmission attempt 

    if(temp == NRF24_TRANSMISSON_OK)
    {
      temp = nrf24_retransmissionCount();
      if (oldSuccess) printf(" %d", temp);
      else printf("\r\nOK R=%d ",temp);
      oldSuccess=1;
    }
    else if(temp == NRF24_MESSAGE_LOST)
    {
      if (!oldSuccess) printf("L");
      else printf("\r\nL");
      oldSuccess=0;
    }
    
    nrf24_powerUpRx();  // Optionally, go back to RX mode ...
    // nrf24_powerDown(); //Or you might want to power down after TX

    _delay_ms(10);
  }
}

int WakeupReceiver()
{
  static int state=0;
  char data[32]="Wake up!?!";
  static int cnt=0;
  static int status=0;
  int result=0, new_st;
    
/*  switch (state)
  {
    case 0:
      NRF24L01_FlushTX();
      status = NRF24L01_SendData(data,6);
      state++;
      break;
    case 1:
      if (status == PDLIB_NRF24_SUCCESS)
      {
        result = 1;
        state = 0;
        printf("success");
        _delay_ms(1000);
      }
      else if (status == PDLIB_NRF24_TX_ARC_REACHED) 
      {
        NRF24L01_FlushTX();
        status = NRF24L01_SendData(data,6);
        if (cnt > 333)
        {
          printf("R");
          cnt=0;
        }
        cnt++;
      }
      else
      {
        //status = NRF24L01_SendData(data,1);
        new_st = NRF24L01_AttemptTx();
        if (cnt > 333)
        {
          printf("!");
          PrintNRF24L01Status(status, __FILE__, __LINE__);
          PrintNRF24L01Status(new_st, __FILE__, __LINE__);
          cnt=0;
        }
        cnt++;
        status=new_st;
      }       
      break;       
    default:
      printf("Unknown state %d",state);
      state=0;
      break;
  }*/
  return result;
}
