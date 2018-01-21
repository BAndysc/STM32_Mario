#ifndef ADC_H
#define ADC_H

#include "pins.h"
#include "dma.h"
#include "device.h"

typedef enum {
	ADC_RESOLUTION_6,
	ADC_RESOLUTION_8,
	ADC_RESOLUTION_10,
	ADC_RESOLUTION_12	
} ADC_RESOLUTION;

typedef struct AnalogIn {
	Pin Pin;
	int8_t Channel;
	ADC_TypeDef* Ptr;
	
	uint16_t (*read)(struct AnalogIn* ain, ADC_RESOLUTION resolution);

	void* data;
	void (*handler)(void* data, uint16_t read);
} AnalogIn;

void InitAnalogIn(AnalogIn* ain, Pin pin);
void SetAnalogInHandler(AnalogIn* ain, void (*handler)(void* data, uint16_t read), void* data);

typedef enum {
	ADC_ALIGN_RIGHT,
	ADC_ALIGN_LEFT
} ADC_Align;

typedef enum {
	ADC_MODE_SINGLE,
	ADC_MODE_CONTINOUS,
	ADC_MODE_DISCONTINOUS
} ADC_Mode;

typedef enum {
	ADC_EOC_AFTER_SEQUENCE,
	ADC_EOC_AFTER_EACH
} ADC_EndOfConversionSelection;

typedef struct {
	bool ScanMode;
	bool UseDMA;
	bool DisableDMAAfterLast;
	bool GenerateInterruptAfterConversion;
	ADC_Align Align;
	ADC_Mode Mode;
	ADC_EndOfConversionSelection EOCSelection;
} ADC_Configuration;

#define MAX_ADC_CHANNELS 2
typedef struct {
	ADC_TypeDef* Ptr;

    DMAt Dma;

	DmaData DmaSettings;

    int8_t numberOfChannels;
    int8_t channels[MAX_ADC_CHANNELS];
} ADCt;

void InitADC(ADCt* adc);
void AddPinToADC(ADCt* adc, Pin pin);
void ADCStartContinous(ADCt* adc, ADC_RESOLUTION resolution, void* destination, InterruptPriority priority);

#endif