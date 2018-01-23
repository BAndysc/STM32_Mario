#ifndef DMA_H
#define DMA_H

#include <stdbool.h>
#include <stdint.h>
#include <stm32.h>
#include "interrupts.h"

/*

========
= DMAt =
========


Interfejs do obsługi DMA. Możliwość przesyłu danych w dwie strony:
 Master -> Peripherial, Peripherial -> Master. Obsługiwana różna wielkość paczek,
 prosty callback po zakończeniu transmisji.

*/

typedef enum
{
    DMA_TRANSFER_SIZE_BYTE,
    DMA_TRANSFER_SIZE_HALF_WORD,
    DMA_TRANSFER_SIZE_WORD
} DMA_Transfer_Size;

typedef enum
{
    DMA_DEVICE_MASTER,
    DMA_DEVICE_PERIPHERIAL
} DMA_Device;

typedef enum
{
    DMA_1 = 1,
    DMA_2,
    DMA_3,
    DMA_4
} DMA_Number;

typedef enum
{
    DMA_CIRCULAR_NO,
    DMA_CIRCULAR
} DMA_Circular;

// Value of enum is important!
typedef enum
{
    DMA_Priority_Low        = 0,
    DMA_Priority_Medium        = 1,
    DMA_Priority_High         = 2,
    DMA_Priority_VeryHigh    = 3,
} DMA_Priority;


// Value of enum is important!
typedef enum
{
    DMA_Dir_Peripherial_Memory = 0,
    DMA_Dir_Memory_Peripherial = 1,
    DMA_Dir_Memory_Memory = 2,
} DMA_Dir;

typedef struct
{
    bool MemoryIncrement;
    bool PeripherialIncrement;
    DMA_Priority Priority;
    DMA_Dir Direction;
    DMA_Circular Circular;
} DMA_Configuration;

typedef struct DMAt
{
    DMA_Stream_TypeDef* ptr;

    InterruptType Interrupt;

    void (*transmit)(struct DMAt* dma, void* source, void* dest, uint32_t length);

    bool (*isBusy)(struct DMAt* dma);

    void (*onSuccess)(void* dma);

    void* handlerData;

} DMAt;

void InitDMA(DMAt* dma, DMA_Number no, uint8_t stream, uint8_t channel, DMA_Configuration* config);

void DmaSetTransferSize(DMAt* dma, DMA_Device device, DMA_Transfer_Size transferSize);

void DmaSetHandler(DMAt* dma, InterruptPriority priority, void (*func)(void* data), void* data);

#endif