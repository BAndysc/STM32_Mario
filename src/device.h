#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include "pins.h"
#include "interrupts.h"

typedef struct {
	DMA_Stream_TypeDef* Ptr;
	InterruptType Interrupt;
} DeviceDMA;

typedef struct {
	uint8_t Number;
	uint8_t Stream;
	uint8_t Channel;
} DmaData;

typedef struct {
	SPI_TypeDef* SPI;
	uint32_t AlternateFunction;
	DmaData DMAtx;
	InterruptType Interrupt;
} DeviceSPI;

typedef struct {
	ADC_TypeDef* Ptr;
	DmaData Dma;
	uint8_t Channel;
	InterruptType Interrupt;
} DeviceADC;

typedef struct {
	USART_TypeDef* Ptr;
	DmaData DmaTX;
	DmaData DmaRX;
	uint32_t AlternateFunction;
	InterruptType Interrupt;
} DeviceUSART;

typedef struct {
	TIM_TypeDef* Ptr;
	InterruptType Interrupt;
} DeviceTimer;

DeviceSPI GetHardwareSPIForPin(Pin clock, Pin mosi, Pin miso);

DeviceDMA GetDMA(uint8_t number, uint8_t stream, uint8_t channel);

DeviceADC GetADCForPin(Pin pin);

DeviceUSART GetUSARTForPin(Pin tx, Pin rx);

DeviceTimer GetNextUnusedTimer16();
DeviceTimer GetNextUnusedTimer32();

/////

void TurnTimerClockOn(TIM_TypeDef* timer);
InterruptType GetInterruptForTimer(TIM_TypeDef* timer);

////
void GPIOClockEnable(Pin pin);
void GPIOClockEnableDMA(uint8_t number);
void GPIOClockEnableSPI(SPI_TypeDef* spi);
void GPIOClockEnableUSART(USART_TypeDef* usart);
void GPIOClockEnableADC(ADC_TypeDef* adc);
#endif