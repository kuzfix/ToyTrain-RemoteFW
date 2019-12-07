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
#include <avr/sleep.h>

#define TIME_BEFORE_GOING_TO_SLEEP_ms  5000
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

void InitSleep()
{
  //Init pin change interrupt for pins PE1,2,3,(PCINT25,26,27) PD5,6,7 (PCINT21,22,23)
  //PCICR |= (1<<PCIE2) | (1<<PCIE3);
  PCMSK2 |= (1<<PCINT21) | (1<<PCINT22) | (1<<PCINT23);
  PCMSK3 |= (1<<PCINT25) | (1<<PCINT26) | (1<<PCINT27);
  //UART Start Frame detection
  UCSR0D = (1<<SFDE);
  //Init sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void GoToSleep()
{
  //TODO: Check the datasheet on all the details on how to conserve more power
  sleep_enable();
  PCIFR  = (1<<PCIF2) | (1<<PCIF3);
  PCICR |= (1<<PCIE2) | (1<<PCIE3);
  UCSR0D = (1<<RXS);  //clear flag before enabling interrupt (probably not necessary to do it separately from enabling the interrupt)
  UCSR0D = (1<<RXSIE) | (1<RXS) | (1<<SFDE);  //Enable start frame detection and the associate interrupt
  sleep_cpu();
}

//Wakeup by serial communication
ISR(USART0_START_vect)
{
  sleep_disable();
  UCSR0D &= ~(1<<RXSIE);
  PCICR &= ~( (1<<PCIE2) | (1<<PCIE3) ); //disable these interrupts
}

//Wakeup by button
ISR(PCINT2_vect)
{
  sleep_disable();
  UCSR0D &= ~(1<<RXSIE);
  PCICR &= ~( (1<<PCIE2) | (1<<PCIE3) ); //disable these interrupts
}
ISR(PCINT3_vect)
{
  sleep_disable();
  UCSR0D &= ~(1<<RXSIE);
  PCICR &= ~( (1<<PCIE2) | (1<<PCIE3) ); //disable these interrupts
}

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
int ProcessMsgQueue()
{
  static uint8_t msg[5];  //initialized to zero before main, 5th byte never written (always \o)
  static int state,cnt;
  int status;
  static int NRFstate=0;
  int busy=1;
  
  switch (state)
  {
    case 0: //WakeUp the receiver
      if (!isMsgQueueEmpty())
      {
        if (!NRFstate)
        {
          NRFstate=1;
          nrf24_powerUpTx();
          break;
        }
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
      else
      {
        busy = 0;
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
        state = 4;
        printf("mC%d",cnt);
      }
      else //if(status == NRF24_MESSAGE_LOST)
      {
        cnt++;
        if (cnt > MAX_MSG_SEND_RETRIES)  //after MAX_MSG_SEND_RETRIES Give up
        {
          state = 4;
          printf("Send failed (%s)",msg);
        }
        nrf24_send(msg);
      }
      break;
    case 4:
      if (isMsgQueueEmpty())
      {
        NRFstate=0;
        busy = 0;
        nrf24_powerDown();
      }
      state=0;
      break;
    default:
      printf("Unknown state %d (%s,L:%d,%s)",state,__FILE__,__LINE__,__FUNCTION__);
      state=0;
      break;
  }
  
  return busy;
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
      }
      else //if(status == NRF24_MESSAGE_LOST)
      {
        cnt++;
        if (cnt > MAX_RETRIES)  //after MAX_RETRIES Give up
        {
          result = -1;
          state = 0;
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

inline int isNum(char data)
{
  return ((data >= '0') && (data <= '9'));
}

int ReceiveBTcommand()
{
  static int state;
  char  data;
  static uint8_t cmd[4];
  int busy = 0;
  
  while (UART0_DataReady())
  {
    UART0_GetByte(&data);
    busy = 1;
    
    switch (state)
    {
      case 0: //waiting for '\n' (start of msg)
        if (data == '\n') state++;
        break;
      case 1: //if valid command
        if ((data == 'F') || (data == 'B') || (data == 'T') || (data == 't') || (data == 's'))
        {
          cmd[0]=data;
          state++;
        }
        else state = 0;
        break;
      case 2: //if numeral
        if (isNum(data))
        {
          cmd[1]=data;
          state++;
        }
        else state = 0;
        break;
      case 3: //if numeral
        if (isNum(data))
        {
          cmd[2]=data;
          state++;
        }
        else state = 0;
        break;
      case 4: //if numeral or !, then send the message. Restart state machine.
        if (cmd[0] == 's')
        {
          if (isNum(data))
          {
            cmd[3]=data;
            PushMsg(cmd);
          }
        }
        else if (data == '!')
        {
          cmd[3] = '!';
          PushMsg(cmd);
        }
        state = 0;
        break;
      default:
        printf("Unknown state %d (%s, L:%d, %s)",state,__FILE__,__LINE__,__FUNCTION__);
        state=0;
        break;
    }
  }
  return busy;
}

int main(void)
{
  uint32_t u32_lastActivity=0;
  int activity;
  char key;
  uint8_t tx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
  uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};

	KBD_Init();
	LED_Init();
	Systime_Init();
	UART0_Init();
	stdout=&UART0_str;
  InitSleep();
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
      activity = ProcessMsgQueue();
      if (activity) u32_lastActivity = GetSysTick();
      activity = ReceiveBTcommand();
      if (activity) u32_lastActivity = GetSysTick();
      KBD_Read();
      key = KBD_GetKey();
      if (key)
      {
        switch (key)
        {
          case BTN1: printf("  B1 "); PushMsg((uint8_t*)"F20!"); break;
          case BTN2: printf("  B2 "); PushMsg((uint8_t*)"F99!"); break;
          case BTN3: printf("  B3 "); PushMsg((uint8_t*)"F00!"); break;
          case BTN4: printf("  B4 "); PushMsg((uint8_t*)"B99!"); break;
          case BTN6: printf("  B6 "); PushMsg((uint8_t*)"B20!"); break;
        }
        u32_lastActivity = GetSysTick();
      }
      if (GetSysTick() - u32_lastActivity > TIME_BEFORE_GOING_TO_SLEEP_ms)
      {
        printf("Sleep...");
        _delay_ms(20);  //Wait until printf is finished
        GoToSleep();
        //Sleeping...
        //After wakeing up:
        u32_lastActivity = GetSysTick();
        printf("Awake! ");
      }
    }
  }
}
