#include <gpio.h>
#include <stm32.h>
#include <string.h>
#include "interrupts.h"
#include "queue.h"
#include "uart.h"
#include "leds.h"
#include "pins.h"
#include "device.h"
#include "debug.h"
#include "dma.h"

#define USART_Mode_Rx_Tx (USART_CR1_RE | USART_CR1_TE)
#define USART_Enable     USART_CR1_UE

#define USART_FlowControl_None 0x0000
#define USART_FlowControl_RTS  USART_CR3_RTSE
#define USART_FlowControl_CTS  USART_CR3_CTSE

#define HSI_HZ 48000000U
#define PCLK1_HZ HSI_HZ

static void EnableUsart(USARTt* usart)
{
	usart->usart->CR1 |= USART_CR1_UE;
}

static bool UsartTransmissionBufferEmpty(USARTt* usart)
{
	return (usart->usart->SR & USART_SR_TXE) != 0;
}

static void UsartEnableTransferCompleteInterrupt(USARTt* usart)
{
	usart->usart->CR1 |= USART_CR1_TXEIE;
}

static void UsartDirectSend(USARTt* usart)
{
	char* buffer = QueueGetPtr(&usart->Tx);		
	usart->dmaTx.transmit(&usart->dmaTx, buffer, (void*)&usart->usart->DR, 1);
}

static void UsartWrite(struct USARTt* usart, char* data)
{
	QueuePutString(&usart->Tx, data);
	if (!usart->dmaTx.isBusy(&usart->dmaTx) && UsartTransmissionBufferEmpty(usart))
		UsartDirectSend(usart);
}

static void UsartWriteInt(struct USARTt* usart, int32_t num)
{
	if (num == 0)
		usart->write(usart, " 0");
	else
	{
		int index = 0;
		char str[11]; // enough for int32
		char strRev[12];
		strRev[0] = ' ';

		if (num < 0)
		{
			strRev[0] = '-';
			num = -num;
		}

		while (num != 0)
		{
			str[index++] = '0' + (num % 10);
			num /= 10;
		}

		for (int i = 0; i < index; ++i)
			strRev[i+1] = str[index - i - 1];

		strRev[index+1] = 0;
		usart->write(usart, strRev);
	}
}

/*static void UsartWriteFloat(USARTt* usart, float num)
{
	if (num >= (0xFFFFFFFF / 2))
	{
		usart->writeInt(usart, (int32_t)num);
		return;
	}
	int32_t val = num * 1000;
	usart->writeInt(usart, val);
}*/

static void DMAFinishedRead2(void* data)
{
	USARTt* usart = (USARTt*)data;
	if (usart->readHandler)
	{
		usart->readHandler(usart->data, usart->RxBuffer, usart->packSize);
	}
}

static void DMAFinishedWrite2(void* data)
{
	USARTt* usart = (USARTt*)data;
	if (!QueueIsEmpty(&usart->Tx))
	{
		UsartEnableTransferCompleteInterrupt(usart);
	}
}

static void UsartTransferCompleteHandler(void* data)
{
	USARTt* usart = (USARTt*)data;
	if (!QueueIsEmpty(&usart->Tx)) {
		UsartDirectSend(usart);		
	}
	//usart->usart->CR1 &= ~USART_CR1_TXEIE;
}

static void UsartReadableHandler(void* data)
{
	USARTt* usart = (USARTt*)data;
	usart->RxBuffer[usart->packSize] = 0;
	usart->dmaRx.transmit(&usart->dmaRx, usart->RxBuffer, (void*)&usart->usart->DR, usart->packSize);
}

static void InitUsartSettings(USARTt* usart, uint32_t baudrate, UartLength len, UartParity parity, UartStopBits stop)
{
	usart->usart->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE | len | parity;
	usart->usart->CR2 = stop;
	usart->usart->CR3 = USART_FlowControl_None | USART_CR3_DMAT | USART_CR3_DMAR;
	usart->usart->BRR = (PCLK1_HZ + (baudrate / 2U)) / baudrate;
}
// siusiak<
static void ConfigureUsartDMA(USARTt* usart, DeviceUSART* device)
{
	DMA_Configuration config;
	config.MemoryIncrement = true;
	config.Direction = DMA_Dir_Memory_Peripherial;
	config.Priority = DMA_Priority_High;
	config.PeripherialIncrement = false;
	config.Circular = DMA_CIRCULAR_NO;
	InitDMA(&usart->dmaTx, device->DmaTX.Number, device->DmaTX.Stream, device->DmaTX.Channel, &config);
	DmaSetHandler(&usart->dmaTx, INTERRUPT_PRIORITY_HIGHEST, DMAFinishedWrite2, usart);

	config.MemoryIncrement = true;
	config.Direction = DMA_Dir_Peripherial_Memory;
	config.Priority = DMA_Priority_High;
	config.PeripherialIncrement = false;
	config.Circular = DMA_CIRCULAR_NO;

	InitDMA(&usart->dmaRx, device->DmaRX.Number, device->DmaRX.Stream, device->DmaRX.Channel, &config);
	DmaSetHandler(&usart->dmaRx, INTERRUPT_PRIORITY_HIGHEST, DMAFinishedRead2, usart);
}

void InitUsart(USARTt* usart, Pin tx, Pin rx, uint32_t baudrate, UartLength len, UartParity parity, UartStopBits stop)
{
	DeviceUSART device = GetUSARTForPin(tx, rx);

	if (device.Ptr == 0)
	{
		Debug("No USART on given pins!");
		return;
	}

	usart->usart = device.Ptr;
	GPIOClockEnableUSART(device.Ptr);
	PinConfigureAlternativeFunc(tx, GPIO_OType_PP, GPIO_Fast_Speed, GPIO_PuPd_NOPULL, device.AlternateFunction);
	PinConfigureAlternativeFunc(rx, GPIO_OType_PP, GPIO_Fast_Speed, GPIO_PuPd_UP, device.AlternateFunction);

	InitUsartSettings(usart, baudrate, len, parity, stop);

	ConfigureUsartDMA(usart, &device);

	usart->write = &UsartWrite;
	usart->writeInt = &UsartWriteInt;

	AddInterruptHandlerManualBit(device.Interrupt, USART_SR_RXNE, 0, INTERRUPT_PRIORITY_HIGHEST, &UsartReadableHandler, usart);
	AddInterruptHandlerManualBit(device.Interrupt, USART_SR_TXE, USART_CR1_TXEIE, INTERRUPT_PRIORITY_HIGHEST, &UsartTransferCompleteHandler, usart);

	EnableUsart(usart);
}

void UsartSetReadHandler(USARTt* usart, uint8_t packSize, void (*handler)(void* data, char* recv, uint8_t len), void* data)
{
	usart->readHandler = handler;
	usart->data = data;
	usart->packSize = packSize;
}
