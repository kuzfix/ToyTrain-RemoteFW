#ifndef _PDLIB_SPI
#define _PDLIB_SPI

#include <avr/io.h>

#define PDLIB_SPCR	SPCR0
#define PDLIB_SPSR	SPSR0
#define PDLIB_SPDR	SPDR0

#define PDLIB_DDR_MISO	DDRB
#define PDLIB_PORT_MISO	PORTB
#define PDLIB_PIN_MISO	PINB
#define PDLIB_BIT_MISO	4
#define PDLIB_DDR_MOSI	DDRB
#define PDLIB_PORT_MOSI	PORTB
#define PDLIB_BIT_MOSI	3
#define PDLIB_DDR_SCK	DDRB
#define PDLIB_PORT_SCK	PORTB
#define PDLIB_BIT_SCK	5
#define PDLIB_DDR_CS	DDRB
#define PDLIB_PORT_CS	PORTB
#define PDLIB_BIT_CS	2
#define PDLIB_DDR_CE	DDRB
#define PDLIB_PORT_CE	PORTB
#define PDLIB_BIT_CE	1

#define PDLIB_CE_LOW()	PDLIB_PORT_CE &= ~(1<<PDLIB_BIT_CE)
#define PDLIB_CE_HIGH()	PDLIB_PORT_CE |=  (1<<PDLIB_BIT_CE)

#define PDLIB_CS_LOW()	PDLIB_PORT_CS &= ~(1<<PDLIB_BIT_CS)
#define PDLIB_CS_HIGH()	PDLIB_PORT_CS |=  (1<<PDLIB_BIT_CS)

void pdlibSPI_ConfigureSPIInterface();
//unsigned char pdlibSPI_ReceiveDataBlocking();
//unsigned int pdlibSPI_ReceiveDataNonBlocking(char *pcData);
unsigned char pdlibSPI_TransferByte(unsigned char ucData);
int pdlibSPI_SendData(unsigned char *pucData, unsigned int uiLength);

#endif
