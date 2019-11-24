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
#include <util/delay.h>

void PrintNRF24L01Status(int status, char* file, int line);
int WakeupReceiver();

int main(void)
{
	uint32_t t1;
  int status;
  char pipe;
  char temp;
  unsigned char address[5] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01};
  char data[33]="Wake";

	KBD_Init();
	LED_Init();
	Systime_Init();
	UART0_Init();
	stdout=&UART0_str;
	sei();
  
  printf("Here we go:\r\n");
  NRF24L01_Init();
  NRF24L01_PowerDown();
  _delay_ms(5);
  NRF24L01_PowerUp();
  _delay_ms(5);
  NRF24L01_SetAirDataRate(PDLIB_NRF24_DRATE_1MBPS);
  NRF24L01_SetARD(750);
  NRF24L01_SetARC(15);
  NRF24L01_SetPAGain(-12);
  /* Set the address */
  NRF24L01_SetTXAddress(address);
	printf("while(1):\r\n");
  while(1)
	{
    printf("S");
  	status = NRF24L01_SendData(data, 4);
    printf("s");
  	if(PDLIB_NRF24_TX_FIFO_FULL == status)
  	{
      printf("FIFO ");
    	NRF24L01_FlushTX();
  	}else if(PDLIB_NRF24_TX_ARC_REACHED == status)
  	{
    	while(status == PDLIB_NRF24_TX_ARC_REACHED)
    	{
        printf("R");
      	status = NRF24L01_AttemptTx();
        _delay_ms(100);
    	}
  	}
    printf("y");
    _delay_ms(300);
  }  
  
  
    
  while (1) 
  {
    if (HasOneMillisecondPassed())
    {
      WakeupReceiver();
    }      

		if (Has_X_MillisecondsPassed(1000,&t1))
		{
			LED_1Tgl();
			printf(".");
		}
  }
}

int WakeupReceiver()
{
  static int state=0;
  char data[32]="Wake up!?!";
  static int cnt=0;
  static int status=0;
  int result=0, new_st;
    
  switch (state)
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
  }
  return result;
}

void PrintNRF24L01Status(int status, char* file, int line)
{
  switch (status)
  {
    case PDLIB_NRF24_SUCCESS: printf("\r\n%s,%d: Status = 0 (SUCCESS)",file,line); break;
    case PDLIB_NRF24_ERROR: printf("\r\n%s,%d: Status = -1 (ERROR)",file,line); break;
    case PDLIB_NRF24_TX_FIFO_FULL: printf("\r\n%s,%d: Status = -2 (TX_FIFO_FULL)",file,line); break;
    case PDLIB_NRF24_TX_ARC_REACHED: printf("\r\n%s,%d: Status = -3 (TX_ARC_REACHED)",file,line); break;
    case PDLIB_NRF24_INVALID_ARGUMENT: printf("\r\n%s,%d: Status = -4 (INVALID_ARGUMENT)",file,line); break;
    case PDLIB_NRF24_BUFFER_TOO_SMALL: printf("\r\n%s,%d: Status = -5 (BUFFER_TOO_SMALL)",file,line); break;
    default: printf("\r\n%s,%d: Unknown status = %d",file,line,status); break;
  }
}
