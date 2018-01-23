#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

typedef enum
{
    INTERRUPT_EXTI = 0,
    INTERRUPT_EXTI0,
    INTERRUPT_EXTI1,
    INTERRUPT_EXTI2,
    INTERRUPT_EXTI3,
    INTERRUPT_EXTI4,
    INTERRUPT_EXTI59,
    INTERRUPT_EXTI1015,
    INTERRUPT_DMA1_STREAM_0,
    INTERRUPT_DMA1_STREAM_1,
    INTERRUPT_DMA1_STREAM_2,
    INTERRUPT_DMA1_STREAM_3,
    INTERRUPT_DMA1_STREAM_4,
    INTERRUPT_DMA1_STREAM_5,
    INTERRUPT_DMA1_STREAM_6,
    INTERRUPT_DMA1_STREAM_7,
    INTERRUPT_DMA2_STREAM_0,
    INTERRUPT_DMA2_STREAM_1,
    INTERRUPT_DMA2_STREAM_2,
    INTERRUPT_DMA2_STREAM_3,
    INTERRUPT_DMA2_STREAM_4,
    INTERRUPT_DMA2_STREAM_5,
    INTERRUPT_DMA2_STREAM_6,
    INTERRUPT_DMA2_STREAM_7,
    INTERRUPT_TIMER,
    INTERRUPT_TIMER1,
    INTERRUPT_TIMER2,
    INTERRUPT_TIMER3,
    INTERRUPT_TIMER4,
    INTERRUPT_TIMER5,
    INTERRUPT_TIMER6,
    INTERRUPT_TIMER7,
    INTERRUPT_TIMER8,
    INTERRUPT_TIMER9,
    INTERRUPT_TIMER10,
    INTERRUPT_TIMER11,
    INTERRUPT_SPI_1,
    INTERRUPT_SPI_2,
    INTERRUPT_SPI_3,
    INTERRUPT_USART_1,
    INTERRUPT_USART_2,
    INTERRUPT_USART_6,
    INTERRUPT_ADC_1,
    INTERRUPT_MAX
} InterruptType;

typedef struct
{
    void (*handler)(void* data);
    void* data;
    uint32_t checkBit;
    uint32_t handleBit;
} InterruptData;

#define MAX_INTERRUPTS 20

typedef struct
{
    InterruptData handlers[MAX_INTERRUPTS];
    int size;
} InterruptDataArray;

typedef enum
{
    INTERRUPT_PRIORITY_HIGHEST,
    INTERRUPT_PRIORITY_HIGH,
    INTERRUPT_PRIORITY_MEDIUM,
    INTERRUPT_PRIORITY_LOW
} InterruptPriority;

extern InterruptDataArray InterruptHandlers[INTERRUPT_MAX];

void AddInterruptHandlerManualBit(InterruptType interrupt, uint32_t checkBit, uint32_t handleBit, InterruptPriority priority, void (*handler)(void* data), void* data);
uint8_t AddInterruptHandler(InterruptType interrupt, InterruptPriority priority, void (*handler)(void* data), void* data);

#endif