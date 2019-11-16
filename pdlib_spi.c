/* 
 * ATmega328PB SPI interface
 * 
 * Description:
 * 
 * The library contains functions to configure the SPI interface. The library is designed to be used to
 * interface the NRF24L01 module with the processor.
 * 
 * Contributors:
 * 
 * Matija Pirc
 * 
 * Change log:
 * 
 * 2019-09-22 : Support for ATmega328PB processor (the first, as of yet untested version)
 * 
 */

#include <stdio.h>
#include "pdlib_spi.h"

/* PS: RX data */
char g_plRxData[256];

/* PS:
 * 
 * Function		: 	pdlibSPI_ConfigureSPIInterface
 * 
 * Arguments	: 	ucSSI - SSI module index (0,1,2,3,4,5) [5 is for SSI1 with GPIO PORTD]
 * 
 * Return		: 	None
 * 
 * Description	: 	The function will setup the SSI module to make it same as Freescale SPI module.
 * 					GPIO configurations required and Clock configurations also done in the function.
 * 
 */

void pdlibSPI_ConfigureSPIInterface()
{
	PDLIB_DDR_MOSI |= (1<<PDLIB_BIT_MOSI);	//MOSI set as output
	PDLIB_DDR_MISO &= ~(1<<PDLIB_BIT_MISO);	//MISO set as input
	PDLIB_PORT_MISO &= ~(1<<PDLIB_BIT_MISO);	//pull-up on MISO off
	PDLIB_DDR_SCK |= (1<<PDLIB_BIT_SCK);	//SCK set as output
	PDLIB_DDR_CS |= (1<<PDLIB_BIT_CS);	//CS set as output
	
	PDLIB_SPCR = (0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<< CPOL) | (0<<CPHA) | (0<<SPR0);
	PDLIB_SPSR = (1<<SPI2X);
}

/* PS:
 * 
 * Function		: 	pdlibSPI_SendData
 * 
 * Arguments	: 	pucData 		- Char array of data to be sent. 
 * 					uiLength		- Length of the data array
 * 
 * Return		: 	If success the function will return the number of data
 * 					bytes it has written to the SPI module. 
 * 					If failed the function will return ZERO.
 * 
 * Description	: 	The function will submit data byte by byte to the SPI module
 * 					for transmission and will wait until the total transmission
 * 					is over. The function is a blocking function.
 * 
 */

int pdlibSPI_SendData(unsigned char *pucData, unsigned int uiLength)
{
	int iIndex = 0;
	/* Validate parameters */
	if((pucData != NULL) && (uiLength > 0))
	{
			while(iIndex < uiLength)
			{
				pdlibSPI_TransferByte(pucData[iIndex++]);
			}
	}
	
	return iIndex;
}


/* PS:
 * 
 * Function		: 	pdlibSPI_TransferByte
 *
 * Arguments	: 	ucData	:	Data byte to transfer
 *
 * Return		: 	Function will return whatever data received from the module during the transfer.
 *
 * Description	: 	The function will submit a data byte the SPI module
 * 					for transmission and will wait until the total transmission
 * 					is over. Then it will read the TX buffer and read one byte out from the buffer.
 * 					This will clear the TX buffer.
 *
 */

unsigned char
pdlibSPI_TransferByte(unsigned char ucData)
{
	uint8_t result=0;

	PDLIB_SPDR = ucData;
	while(!(PDLIB_SPSR & (1<<SPIF)));
	result = PDLIB_SPDR;
	return result;
}


/* PS:
 *
 * Function		: 	pdlibSPI_ReceiveDataBlocking
 * 
 * Arguments	: 	None
 * 
 * Return		: 	Returns the byte value read
 * 
 * Description	: 	The function will read one byte of data from the SSI
 * 					module RX FIFO. The function will not return until 
 * 					data is available.
 * 
 */
/*
unsigned char
pdlibSPI_ReceiveDataBlocking()
{
	unsigned long ulRxData;

	ROM_SSIDataGet(g_SSIModule[g_SSI][SSIBASE], &ulRxData);
	
	return ((unsigned char)(ulRxData & 0xFF));
}
*/
/* PS:
 * 
 * Function		: 	pdlibSPI_ReceiveDataNonBlocking
 * 
 * Arguments	: 	pcData - Pre allocated 'char' value to receive data
 * 
 * Return		: 	unsigned int value, which contains the number of bytes read.
 * 
 * Description	: 	The function will read one byte of data from the SSI
 * 					module RX FIFO. The function is none blocking. It will
 * 					return zero if there are no data in the RX-FIFO of the
 * 					SSI module
 * 
 */

/*
unsigned int 
pdlibSPI_ReceiveDataNonBlocking(char *pcData)
{
	unsigned int iReturn = 0;
	unsigned long ulRxData;
	*/
	/* Validate the arguments */
/*	if(pcData != NULL)
	{
		iReturn = ROM_SSIDataGetNonBlocking(g_SSIModule[g_SSI][SSIBASE], &ulRxData);
	}
	
	if(iReturn > 0)
	{
		(*pcData) = (char)(ulRxData);
	}
	
	return iReturn;
}
*/