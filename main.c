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

#define MAX_RETRIES 200
int WakeUpReceiver();

int main(void)
{
  char temp;
  uint8_t tx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
  uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};

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
    if (HasOneMillisecondPassed())
    {
      if (WakeUpReceiver())
      {
        nrf24_send((uint8_t*)"Msg!");
        while(nrf24_isSending());         // Wait for transmission to end
        temp = nrf24_lastMessageStatus(); // Make analysis on last transmission attempt
        if(temp == NRF24_TRANSMISSON_OK)
        {
          temp = nrf24_retransmissionCount();
          printf(" OK R=%d\r\n",temp);
        }
        else if(temp == NRF24_MESSAGE_LOST)
        {
          printf(" e\r\n");
        }
      }
    }
    /*
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

    _delay_ms(10);*/
  }
}

int WakeUpReceiver()
{
  static int state=0;
  uint8_t data[32]="Wake";
  static int cnt=0;
  int status=0;
  int result=0;
    
  switch (state)
  {
    case 0:
      nrf24_send(data);
      cnt=0;
      state++;
      break;
    case 1:
      if (nrf24_isSending()) break;
      status = nrf24_lastMessageStatus();
      if(status == NRF24_TRANSMISSON_OK)
      {
        result = 1;
        state = 0;
        printf("C%d",cnt);
      }
      else //if(status == NRF24_MESSAGE_LOST)
      {
        cnt++;
        if (cnt > MAX_RETRIES)  //after MAX_RETRIES Give up
        {
          result = -1;
          state = 0;
          printf("F");
        }
        nrf24_send(data);
      }
      break;       
    default:
      printf("Unknown state %d",state);
      state=0;
      break;
  }
  return result;
}
