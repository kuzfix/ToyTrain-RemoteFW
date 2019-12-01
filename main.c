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

/* PROTOCOL
Wake - wake up the receiver - no action if already awake
Fnn! - Forward with speed nn: ASCII 0(stop)-99(max speed)
Bnn! - Backwards with speed nn (same format as F)
Tss! - Time to auto stop.
tss! - Time to auto stop if command receifed frome remote controller
*/

#define MSG_QUEUE_LENGTH  10
#define MSG_LENGTH  4

uint8_t MsgQueue[MSG_QUEUE_LENGTH][MSG_LENGTH];
int MQin;
int MQout;
int MQN;

int PushMsg(uint8_t* msg);
int PullMsg(uint8_t* dst);
int isMsgQueueFull() {return (MQN >=MSG_QUEUE_LENGTH);}
int isMsgQueueEmpty() {return (!MQN);}
int numberOfItemsInMsgQueue() {return MQN;}

int PushMsg(uint8_t* msg)
{
  if (isMsgQueueFull()) return 0;
  memcpy(&MsgQueue[MQin][0],msg,MSG_LENGTH);
  MQN++;
  MQin++;
  if (MQin >= MSG_QUEUE_LENGTH) MQin = 0;
  return 1;
}

int PullMsg(uint8_t* dst)
{
  if (isMsgQueueEmpty()) return 0;
  memcpy(dst, &MsgQueue[MQout][0],MSG_LENGTH);
  MQN--;
  MQout++;
  if (MQout >= MSG_QUEUE_LENGTH) MQout = 0;
  return 1;
}

#define MAX_MSG_SEND_RETRIES  10
void ProcessMsgQueue()
{
  static uint8_t msg[5];  //initialized to zero before main, 5th byte never written (always \o)
  static int state,cnt;
  int status;
  
  switch (state)
  {
    case 0: //WakeUp the receiver
      if (!isMsgQueueEmpty())
      {
        status = WakeUpReceiver();
        if (status == -1) //ERROR - didn't wake up (report and discard message)
        {
          printf("Can't wake it up");
          PullMsg(msg);          
        }
        else if (status == 1) //Receiver woken up
        {
          state++;
        }
        //else: still trying...
      }
      break;
    case 1: //Get message
      if (PullMsg(msg))
      {
        state++;
      }
      else  //Error - reset state machine
      {
        state=0;
        printf("Couldn't get msg in state 1. Someone stealing them? (%s,L:%d)",__FILE__,__LINE__);
      }
      //Do not put a break here!
    case 2:
      nrf24_send(msg);
      cnt = 0;
      state++;
      break;
    case 3:
      if (nrf24_isSending()) break;
      status = nrf24_lastMessageStatus();
      if(status == NRF24_TRANSMISSON_OK)
      {
        state = 0;
        printf("mC%d",cnt);
      }
      else //if(status == NRF24_MESSAGE_LOST)
      {
        cnt++;
        if (cnt > MAX_MSG_SEND_RETRIES)  //after MAX_MSG_SEND_RETRIES Give up
        {
          state = 0;
          printf("Send failed (%s)",msg);
        }
        nrf24_send(msg);
      }
      break;
    default:
      printf("Unknown state %d (%s,L:%d,%s)",state,__FILE__,__LINE__,__FUNCTION__);
      state=0;
      break;
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
        printf("wC%d",cnt);
      }
      else //if(status == NRF24_MESSAGE_LOST)
      {
        cnt++;
        if (cnt > MAX_RETRIES)  //after MAX_RETRIES Give up
        {
          result = -1;
          state = 0;
          printf("wF");
        }
        nrf24_send(data);
      }
      break;
    default:
      printf("Unknown state %d (%s,L:%d,%s)",state,__FILE__,__LINE__,__FUNCTION__);
      state=0;
      break;
  }
  return result;
}



int main(void)
{
  //char temp;
  char key;
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
      ProcessMsgQueue();
      KBD_Read();
      key = KBD_GetKey();
      switch (key)
      {
        case BTN1: printf("  B1 "); PushMsg((uint8_t*)"F50!"); break;
        case BTN2: printf("  B2 "); PushMsg((uint8_t*)"F99!"); break;
        case BTN3: printf("  B3 "); PushMsg((uint8_t*)"F00!"); break;
        case BTN4: printf("  B4 "); PushMsg((uint8_t*)"B99!"); break;
        case BTN6: printf("  B6 "); PushMsg((uint8_t*)"B50!"); break;
      }
      /*if (WakeUpReceiver())
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
      }*/
    }
  }
}


