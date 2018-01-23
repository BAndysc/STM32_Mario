#ifndef SPI_H
#define SPI_H

#include "pins.h"
#include "stdint.h"
#include "dma.h"

/*

========
= SPIt =
========


Sprzętowa (z DMA) i programowa obsługa po interfejsie SPIt. Wystarczy użyć funkcji
na strukturze wraz z podanymi pinami. InitSPI wykrywa czy na danych pinach jest
sprzętowa obsługa SPIt i w takim przypadku używa sprzętowego wspomagania. W przeciwnym
razie wysyła wiadomosć na USART i przełącza się na programowe sterowanie.

Ponieważ obsługa SPIt była tworzona na razie z myślą o LCD, obsługiwana jest jedynie
jednokierunkowa komunikacja: Master -> Slave, zatem pin MISO nie jest używany.

*/

typedef struct SPIt
{

	Pin Clock;
	Pin MOSI;
	Pin MISO; // not supported

	SPI_TypeDef* hardwareSPI;
	DMAt dmaTX;

	void (*writeAsync)(struct SPIt* spi, char* data, int length);
	void (*writeSync)(struct SPIt* spi, char* data, int length);
	void (*write8)(struct SPIt* spi, uint8_t data);
	void (*write16)(struct SPIt* spi, uint16_t data);

	void (*onFinishedWrite)(void* data);

	void* handlerData;

} SPIt;

typedef enum
{
	SPI_TRANSFER_SIZE_BYTE,
	SPI_TRANSFER_SIZE_HALF_WORD
} SPI_TransferSize;

typedef enum
{
    SPI_BIDI_MODE_1_LINE_BI,
    SPI_BIDI_MODE_2_LINE_UNI
} SPI_BIDIMode;

typedef enum
{
    SPI_RECEIVE_ONLY,
    SPI_TRANSMIT_ONLY,
    SPI_TRANSMIT_AND_RECEIVE,
} SPI_TransferDirection;

typedef enum
{
    SPI_SLAVE_MANAGMENT_SOFTWARE,
    SPI_SLAVE_MANAGMENT_HARDWARE
} SPI_SlaveManagment;

typedef enum
{
    SPI_MOST_SIG_BYTE_FIRST,
    SPI_LEAST_SIG_BYTE_FIRST
} SPI_ByteOrder;

// value is important!
typedef enum
{
    SPI_BAUD_DIV2 = 0,
    SPI_BAUD_DIV4 = 1,
    SPI_BAUD_DIV8 = 2,
    SPI_BAUD_DIV16 = 3,
    SPI_BAUD_DIV32 = 4,
    SPI_BAUD_DIV64 = 5,
    SPI_BAUD_DIV128 = 6,
    SPI_BAUD_DIV256 = 7,
} SPI_BaudRatePrescaler;

typedef enum
{
    SPI_ROLE_SLAVE,
    SPI_ROLE_MASTER
} SPI_Role;

typedef enum
{
    SPI_CLOCK_0_WHEN_IDLE,
    SPI_CLOCK_1_WHEN_IDLE
} SPI_ClockPolarity;

typedef enum
{
    SPI_FIRST_CLOCK_TRANSITION_IS_FIRST_DATA_CAPTURE_EDGE,
    SPI_SECOND_CLOCK_TRANSITION_IS_FIRST_DATA_CAPTURE_EDGE
} SPI_ClockPhase;

typedef struct
{
    SPI_BIDIMode BidiMode;
    SPI_TransferDirection Direction;
    SPI_TransferSize TransferSize;
    SPI_SlaveManagment SlaveManagment;
    SPI_ByteOrder ByteOrder;
    SPI_BaudRatePrescaler BaudRatePrescaler;
    SPI_Role Role;
    SPI_ClockPolarity ClockPolarity;
    SPI_ClockPhase ClockPhase;

    bool CalculateCRC;
    bool InternalSlaveSelect;
} SPI_Configuration;

void InitSPI(SPIt* spi, Pin clock, Pin mosi, Pin miso, SPI_Configuration* configuration);
void SPISetHandler(SPIt* spi, void (*handler)(void* data), void* data);
void SPISetTranfserSize(SPIt* spi, SPI_TransferSize transferSize);

#endif