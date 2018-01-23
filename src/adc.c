#include <stm32f411xe.h>
#include <string.h>
#include "adc.h"
#include "device.h"
#include "debug.h"

#define ADC_SQRx_BITS_PER_CHANNEL 5

static void SetupADC(ADCt* adc, ADC_Configuration* configuration)
{
    adc->Ptr->CR1 = 0;
    adc->Ptr->CR2 = 0;

    if (configuration->ScanMode)
        adc->Ptr->CR1 |= ADC_CR1_SCAN;

    if (configuration->UseDMA)
        adc->Ptr->CR2 |= ADC_CR2_DMA;

    if (!configuration->DisableDMAAfterLast)
        adc->Ptr->CR2 |= ADC_CR2_DDS;

    if (configuration->Align == ADC_ALIGN_LEFT)
        adc->Ptr->CR2 |= ADC_CR2_ALIGN;

    if (configuration->GenerateInterruptAfterConversion)
        adc->Ptr->CR1 |= ADC_CR1_EOCIE;

    if (configuration->EOCSelection == ADC_EOC_AFTER_EACH)
        adc->Ptr->CR2 |= ADC_CR2_EOCS;

    switch (configuration->Mode)
    {
        case ADC_MODE_SINGLE:
            break;
        case ADC_MODE_CONTINOUS:
            adc->Ptr->CR2 |= ADC_CR2_CONT;
            break;
        case ADC_MODE_DISCONTINOUS:
            adc->Ptr->CR1 |= ADC_CR1_DISCEN;
            break;
    }
}

static void InitADCForPin(ADCt* adc, Pin pin)
{
    DeviceADC device = GetADCForPin(pin);
    adc->Ptr = device.Ptr;
    adc->DmaSettings = device.Dma;
    GPIOClockEnableADC(adc->Ptr);
}

void InitADC(ADCt* adc)
{
    adc->Ptr = 0;
    adc->numberOfChannels = 0;
}

static void InternalAddPinADC(ADCt* adc, Pin pin)
{
    if (adc->numberOfChannels >= MAX_ADC_CHANNELS)
        Abort("Cannot add pin to adc; too many channels used. Aborting!");

    DeviceADC device = GetADCForPin(pin);

    if (device.Ptr != adc->Ptr)
        Abort("This pin doesn't belong to used ADC pointer. Aborting!");

    PinConfigureAnalogIn(pin);
    adc->channels[adc->numberOfChannels] = device.Channel;
    adc->numberOfChannels++;
}

void AddPinToADC(ADCt* adc, Pin pin)
{
    if (adc->Ptr == nullptr)
        InitADCForPin(adc, pin);

    InternalAddPinADC(adc, pin);
}

static void ADCConfigureDMAForContinous(ADCt* adc, ADC_RESOLUTION resolution)
{
    DMA_Configuration config;
    config.Priority = DMA_Priority_High;
    config.Direction = DMA_Dir_Peripherial_Memory;
    config.PeripherialIncrement = false;
    config.MemoryIncrement = true;
    config.Circular = DMA_CIRCULAR;

    InitDMA(&adc->Dma, adc->DmaSettings.Number, adc->DmaSettings.Stream, adc->DmaSettings.Channel, &config);

    DMA_Transfer_Size transferSize = DMA_TRANSFER_SIZE_HALF_WORD;
    if (resolution == ADC_RESOLUTION_6)
        transferSize = DMA_TRANSFER_SIZE_BYTE;

    DmaSetTransferSize(&adc->Dma, DMA_DEVICE_MASTER, transferSize);
    DmaSetTransferSize(&adc->Dma, DMA_DEVICE_PERIPHERIAL, transferSize);
}

static void ADCSetResolution(ADCt* adc, ADC_RESOLUTION resolution)
{
    adc->Ptr->CR1 &= ~(ADC_CR1_RES_0 | ADC_CR1_RES_1);
    switch (resolution)
    {
        case ADC_RESOLUTION_6:
            adc->Ptr->CR1 |= ADC_CR1_RES_0 | ADC_CR1_RES_1;
            break;
        case ADC_RESOLUTION_8:
            adc->Ptr->CR1 |= ADC_CR1_RES_1;
            break;
        case ADC_RESOLUTION_10:
            adc->Ptr->CR1 |= ADC_CR1_RES_0;
            break;
        case ADC_RESOLUTION_12:
            // zero
            break;
    }
}

static void ADCConfigureChannels(ADCt* adc)
{
    adc->Ptr->SQR1 = 0;
    adc->Ptr->SQR2 = 0;
    adc->Ptr->SQR3 = 0;

    int8_t channelsNum = (int8_t)(adc->numberOfChannels - 1);
    if (channelsNum & 1)
        adc->Ptr->SQR1 |= ADC_SQR1_L_0;
    if (channelsNum & 2)
        adc->Ptr->SQR1 |= ADC_SQR1_L_1;
    if (channelsNum & 4)
        adc->Ptr->SQR1 |= ADC_SQR1_L_2;
    if (channelsNum & 8)
        adc->Ptr->SQR1 |= ADC_SQR1_L_3;

    for (int i = 0; i < adc->numberOfChannels; ++i)
        adc->Ptr->SQR3 |= adc->channels[i] << (i * ADC_SQRx_BITS_PER_CHANNEL);
}

static void EnableADC(ADCt* adc)
{
    adc->Ptr->CR2 |= ADC_CR2_ADON;
}

static void ADCStartConversion(ADCt* adc)
{
    adc->Ptr->CR2 |= ADC_CR2_SWSTART;
}

void ADCStartContinous(ADCt* adc, ADC_RESOLUTION resolution, void* destination, InterruptPriority priority)
{
    if (adc->numberOfChannels <= 0)
        return;

    ADCConfigureDMAForContinous(adc, resolution);

    ADC_Configuration configuration;
    configuration.Align = ADC_ALIGN_LEFT;
    configuration.Mode = ADC_MODE_CONTINOUS;
    configuration.ScanMode = true;
    configuration.UseDMA = true;
    configuration.DisableDMAAfterLast = false;
    configuration.EOCSelection = ADC_EOC_AFTER_EACH;
    configuration.GenerateInterruptAfterConversion = false;

    SetupADC(adc, &configuration);

    EnableADC(adc);

    ADCSetResolution(adc, resolution);
    ADCConfigureChannels(adc);

    ADCStartConversion(adc);
    adc->Dma.transmit(&adc->Dma, destination, (void*)&adc->Ptr->DR, (uint32_t)adc->numberOfChannels);
}
