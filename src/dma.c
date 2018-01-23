#include <stm32.h>
#include "dma.h"
#include "debug.h"
#include "device.h"

static void DmaHandleInterrupt(void* data);

static void ResetDma(DMAt* dma)
{
    dma->ptr->CR = 0;
    dma->ptr->FCR = 0;
}

static void EnableDma(DMAt* dma)
{
    dma->ptr->CR |= DMA_SxCR_EN;
}

static bool IsBusy(DMAt* dma)
{
    return (dma->ptr->CR & DMA_SxCR_EN) != 0;
}

static void DmaSetChannel(DMAt* dma, uint8_t channel)
{
    if (channel & 1)
        dma->ptr->CR |= DMA_SxCR_CHSEL_0;
    if (channel & 2)
        dma->ptr->CR |= DMA_SxCR_CHSEL_1;
    if (channel & 4)
        dma->ptr->CR |= DMA_SxCR_CHSEL_2;
}

static void DmaConfigure(DMAt* dma, DMA_Configuration* config)
{
    if (config->MemoryIncrement)
        dma->ptr->CR |= DMA_SxCR_MINC;

    if (config->PeripherialIncrement)
        dma->ptr->CR |= DMA_SxCR_PINC;

    if (config->Priority & 1)
        dma->ptr->CR |= DMA_SxCR_PL_0;
    if (config->Priority & 2)
        dma->ptr->CR |= DMA_SxCR_PL_1;

    if (config->Direction == DMA_Dir_Memory_Peripherial)
        dma->ptr->CR |= DMA_SxCR_DIR_0;
    else if (config->Direction == DMA_Dir_Memory_Memory)
        dma->ptr->CR |= DMA_SxCR_DIR_1;
    // else default

    if (config->Circular == DMA_CIRCULAR)
        dma->ptr->CR |= DMA_SxCR_CIRC;
}

static void DmaEnableInterrupt(DMAt* dma, InterruptPriority priority)
{
    dma->ptr->CR |= DMA_SxCR_TCIE;
    AddInterruptHandler(dma->Interrupt, priority, &DmaHandleInterrupt, dma);
}

static void DmaTransmit(DMAt* dma, void* source, void* dest, uint32_t length)
{
    dma->ptr->PAR = (uint32_t)dest;
    dma->ptr->M0AR = (uint32_t)source;
    dma->ptr->NDTR = length;
    EnableDma(dma);
}

void InitDMA(DMAt* dma, DMA_Number no, uint8_t stream, uint8_t channel, DMA_Configuration* config)
{
    DeviceDMA dmaData = GetDMA(no, stream, channel);
    if (!dmaData.Ptr)
        Debug("Invalid DMA no/stream");

    dma->ptr = dmaData.Ptr;
    dma->Interrupt = dmaData.Interrupt;

    GPIOClockEnableDMA(no);
    ResetDma(dma);
    DmaSetChannel(dma, channel);
    DmaConfigure(dma, config);

    dma->transmit = &DmaTransmit;
    dma->isBusy = &IsBusy;
}


static void DmaHandleInterrupt(void* data)
{
    DMAt* dma = (DMAt*)data;

    if (dma->onSuccess)
        dma->onSuccess(dma->handlerData);
}


void DmaSetHandler(DMAt* dma, InterruptPriority priority, void (*func)(void* data), void* data)
{
    dma->onSuccess = func;
    dma->handlerData = data;
    DmaEnableInterrupt(dma, priority);
}

void DmaSetTransferSize(DMAt* dma, DMA_Device device, DMA_Transfer_Size transferSize)
{
    uint32_t reset = 0;
    uint32_t apply = 0;

    if (device == DMA_DEVICE_MASTER)
    {
        reset = DMA_SxCR_MSIZE_0 | DMA_SxCR_MSIZE_1;
        if (transferSize == DMA_TRANSFER_SIZE_HALF_WORD)
            apply = DMA_SxCR_MSIZE_0;
        else if (transferSize == DMA_TRANSFER_SIZE_WORD)
            apply = DMA_SxCR_MSIZE_1;
    }
    else
    {
        reset = DMA_SxCR_PSIZE_0 | DMA_SxCR_PSIZE_1;
        if (transferSize == DMA_TRANSFER_SIZE_HALF_WORD)
            apply = DMA_SxCR_PSIZE_0;
        else if (transferSize == DMA_TRANSFER_SIZE_WORD)
            apply = DMA_SxCR_PSIZE_1;
    }

    dma->ptr->CR &= ~(reset);
    dma->ptr->CR |= apply;
}

