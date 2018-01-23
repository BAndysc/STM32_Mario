#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "pins.h"
#include "dma.h"
#include "queue.h"

#define USART_RX_BUFFER_LEN 256

/*

==========
= USARTt =
==========


Sprzętowa (z DMA) obsługa komunikacji szeregowej po usarcie. Wystarczy użyć funkcji
na strukturze wraz z podanymi pinami. InitUsart wykrywa czy na danych pinach jest
sprzętowa obsługa Usart i w takim przypadku używa sprzętowego wspomagania. W przeciwnym
razie `false`.

*/


typedef enum 
{
	UART_LENGTH_8b = 0x0000,
	UART_LENGTH_9b = USART_CR1_M
} UartLength;

typedef enum 
{
	UART_PARITY_NO = 0x0,
	UART_PARITY_EVEN = USART_CR1_PCE,
	UART_PARITY_ODD = (USART_CR1_PCE | USART_CR1_PS)
} UartParity;

typedef enum
{
	UART_STOP_BITS_1 = 0x0000,
	UART_STOP_BITS_0_5 = 0x1000,
	UART_STOP_BITS_2 = 0x2000,
	UART_STOP_BITS_1_5 = 0x3000
} UartStopBits;


typedef struct USARTt
{
	USART_TypeDef* usart;
	DMAt dmaTx;
	DMAt dmaRx;
	Queue Tx;
	char RxBuffer[USART_RX_BUFFER_LEN];
	void (*write)(struct USARTt* usart, char* string);
	void (*writeInt)(struct USARTt* usart, int32_t num);

    void (*writeSync)(struct USARTt* usart, char* string);
	void (*writeIntSync)(struct USARTt* usart, int32_t num);


	void (*readHandler)(void* data, char const* recv, uint8_t len);
	void* data;
	uint8_t packSize;
} USARTt;

bool InitUsart(USARTt* usart, Pin tx, Pin rx, uint32_t baudrate, UartLength len, UartParity parity, UartStopBits stop);
void UsartSetReadHandler(USARTt* usart, uint8_t packSize, void (*handler)(void* data, char const* recv, uint8_t len), void* data);

#endif