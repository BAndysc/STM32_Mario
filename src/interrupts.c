#include "interrupts.h"
#include <gpio.h>
#include "irq.h"
#include "debug.h"
#include <stm32.h>

InterruptDataArray InterruptHandlers[INTERRUPT_MAX];

IRQn_Type InterruptTypeToIRQnMap[] = {
        255, // INTERRUPT_EXTI
        EXTI0_IRQn, // INTERRUPT_EXTI0
        EXTI1_IRQn, // INTERRUPT_EXTI1
        EXTI2_IRQn, // INTERRUPT_EXTI2
        EXTI3_IRQn, // INTERRUPT_EXTI3
        EXTI4_IRQn, // INTERRUPT_EXTI4
        EXTI9_5_IRQn, // INTERRUPT_EXTI59
        EXTI15_10_IRQn, // INTERRUPT_EXTI1015
        DMA1_Stream0_IRQn, // INTERRUPT_DMA1_STREAM_0
        DMA1_Stream1_IRQn, // INTERRUPT_DMA1_STREAM_1
        DMA1_Stream2_IRQn, // INTERRUPT_DMA1_STREAM_2
        DMA1_Stream3_IRQn, // INTERRUPT_DMA1_STREAM_3
        DMA1_Stream4_IRQn, // INTERRUPT_DMA1_STREAM_4
        DMA1_Stream5_IRQn, // INTERRUPT_DMA1_STREAM_5
        DMA1_Stream6_IRQn, // INTERRUPT_DMA1_STREAM_6
        DMA1_Stream7_IRQn, // INTERRUPT_DMA1_STREAM_7
        DMA2_Stream0_IRQn, // INTERRUPT_DMA2_STREAM_0
        DMA2_Stream1_IRQn, // INTERRUPT_DMA2_STREAM_1
        DMA2_Stream2_IRQn, // INTERRUPT_DMA2_STREAM_2
        DMA2_Stream3_IRQn, // INTERRUPT_DMA2_STREAM_3
        DMA2_Stream4_IRQn, // INTERRUPT_DMA2_STREAM_4
        DMA2_Stream5_IRQn, // INTERRUPT_DMA2_STREAM_5
        DMA2_Stream6_IRQn, // INTERRUPT_DMA2_STREAM_6
        DMA2_Stream7_IRQn, // INTERRUPT_DMA2_STREAM_7
        255,
        255, // TIM1_IRQn
        TIM2_IRQn,
        TIM3_IRQn,
        TIM4_IRQn,
        TIM5_IRQn,
        255,
        255,
        255,
        255, // TIM9_IRQn
        255, // TIM10_IRQn
        255, // TIM11_IRQn,
        SPI1_IRQn,
        SPI2_IRQn,
        SPI3_IRQn,
        USART1_IRQn,
        USART2_IRQn,
        USART6_IRQn,
        ADC_IRQn,
};

// domyślny proponowany bit oznaczający wystąpienie danego przerwania
static const uint32_t const InterruptCheckBits[] = {
    0,
    (1 << 0),
    (1 << 1),
    (1 << 2),
    (1 << 3),
    (1 << 4),
    (1 << 5),
    (1 << 10),
    DMA_LISR_TCIF0,
    DMA_LISR_TCIF1,
    DMA_LISR_TCIF2,
    DMA_LISR_TCIF3,
    DMA_HISR_TCIF4,
    DMA_HISR_TCIF5,
    DMA_HISR_TCIF6,
    DMA_HISR_TCIF7,
    DMA_LISR_TCIF0,
    DMA_LISR_TCIF1,
    DMA_LISR_TCIF2,
    DMA_LISR_TCIF3,
    DMA_HISR_TCIF4,
    DMA_HISR_TCIF5,
    DMA_HISR_TCIF6,
    DMA_HISR_TCIF7,
    0,
    TIM_SR_UIF,
    TIM_SR_UIF,
    TIM_SR_UIF,
    TIM_SR_UIF,
    TIM_SR_UIF,
    TIM_SR_UIF,
    TIM_SR_UIF,
    TIM_SR_UIF,
    TIM_SR_UIF,
    TIM_SR_UIF,
    TIM_SR_UIF,
    0, // spi1
    0, // spi 2
    0, // spi 3
    0, // usart 1
    0, // usart 2
    0, // usart 6
    ADC_SR_EOC,
};


// domyślny proponowany bit oznaczający wystąpienie danego przerwania
static const uint32_t const InterruptClearBits[] = {
    0,
    (1 << 0),
    (1 << 1),
    (1 << 2),
    (1 << 3),
    (1 << 4),
    (1 << 5),
    (1 << 10),
    DMA_LIFCR_CTCIF0,
    DMA_LIFCR_CTCIF1,
    DMA_LIFCR_CTCIF2,
    DMA_LIFCR_CTCIF3,
    DMA_HIFCR_CTCIF4,
    DMA_HIFCR_CTCIF5,
    DMA_HIFCR_CTCIF6,
    DMA_HIFCR_CTCIF7,
    DMA_LIFCR_CTCIF0,
    DMA_LIFCR_CTCIF1,
    DMA_LIFCR_CTCIF2,
    DMA_LIFCR_CTCIF3,
    DMA_HIFCR_CTCIF4,
    DMA_HIFCR_CTCIF5,
    DMA_HIFCR_CTCIF6,
    DMA_HIFCR_CTCIF7,
    0,
    ~TIM_SR_UIF,
    ~TIM_SR_UIF,
    ~TIM_SR_UIF,
    ~TIM_SR_UIF,
    ~TIM_SR_UIF,
    ~TIM_SR_UIF,
    ~TIM_SR_UIF,
    ~TIM_SR_UIF,
    ~TIM_SR_UIF,
    ~TIM_SR_UIF,
    ~TIM_SR_UIF,
    0, // spi 1
    0,  // spi 2
    0, // spi3
    0, // usart 1
    0, // usart 2
    0, // usart 6
    ADC_CR1_EOCIE
};

typedef enum {
    HANDLE_OPERATION_SET = 0,
    HANDLE_OPERATION_AND_NOT = 1,
    HANDLE_OPERATION_OR = 2
} HandleOperation;

void Handle(InterruptType type, volatile uint32_t* check, volatile uint32_t* handle, HandleOperation mode)
{
    InterruptDataArray* array = &InterruptHandlers[type];

    for (int i = 0; i < array->size; ++i)
    {
        if (((*check) & array->handlers[i].checkBit))
        {
            if (mode == HANDLE_OPERATION_SET)
                *handle = array->handlers[i].handleBit;
            else if (mode == HANDLE_OPERATION_AND_NOT)
                *handle &= ~(array->handlers[i].handleBit);
            else if (mode == HANDLE_OPERATION_OR)
                *handle |= array->handlers[i].handleBit;

            irq_level_t lvl = IRQprotectAll();
            array->handlers[i].handler(array->handlers[i].data);
            IRQunprotectAll(lvl);
        }
    }
}

void TIM2_IRQHandler(void) {
    Handle(INTERRUPT_TIMER2, &(TIM2->SR), &(TIM2->SR), HANDLE_OPERATION_SET);
}

void TIM3_IRQHandler(void) {
    Handle(INTERRUPT_TIMER3, &(TIM3->SR), &(TIM3->SR), HANDLE_OPERATION_SET);
}

void TIM4_IRQHandler(void) {
    Handle(INTERRUPT_TIMER4, &(TIM4->SR), &(TIM4->SR), HANDLE_OPERATION_SET);
}

void TIM5_IRQHandler(void) {
    Handle(INTERRUPT_TIMER5, &(TIM5->SR), &(TIM5->SR), HANDLE_OPERATION_SET);
}

void EXTI0_IRQHandler()
{
    Handle(INTERRUPT_EXTI0, &(EXTI->PR), &(EXTI->PR), HANDLE_OPERATION_SET);
}

void EXTI3_IRQHandler()
{
    Handle(INTERRUPT_EXTI3, &(EXTI->PR), &(EXTI->PR), HANDLE_OPERATION_SET);
}

void EXTI4_IRQHandler()
{
    Handle(INTERRUPT_EXTI4, &(EXTI->PR), &(EXTI->PR), HANDLE_OPERATION_SET);
}

void EXTI9_5_IRQHandler()
{
    Handle(INTERRUPT_EXTI59, &(EXTI->PR), &(EXTI->PR), HANDLE_OPERATION_SET);
}

void EXTI15_10_IRQHandler()
{
    Handle(INTERRUPT_EXTI1015, &(EXTI->PR), &(EXTI->PR), HANDLE_OPERATION_SET);
}

void DMA1_Stream7_IRQHandler(void)
{
    Handle(INTERRUPT_DMA1_STREAM_7, &(DMA1->HISR), &(DMA1->HIFCR), HANDLE_OPERATION_SET);
}

void DMA1_Stream6_IRQHandler(void)
{
    Handle(INTERRUPT_DMA1_STREAM_6, &(DMA1->HISR), &(DMA1->HIFCR), HANDLE_OPERATION_SET);
}
void DMA1_Stream5_IRQHandler(void)
{
    Handle(INTERRUPT_DMA1_STREAM_5, &(DMA1->HISR), &(DMA1->HIFCR), HANDLE_OPERATION_SET);
}
void DMA1_Stream4_IRQHandler(void)
{
    Handle(INTERRUPT_DMA1_STREAM_4, &(DMA1->HISR), &(DMA1->HIFCR), HANDLE_OPERATION_SET);
}
void DMA1_Stream3_IRQHandler(void)
{
    Handle(INTERRUPT_DMA1_STREAM_3, &(DMA1->LISR), &(DMA1->LIFCR), HANDLE_OPERATION_SET);
}
void DMA1_Stream2_IRQHandler(void)
{
    Handle(INTERRUPT_DMA1_STREAM_2, &(DMA1->LISR), &(DMA1->LIFCR), HANDLE_OPERATION_SET);
}
void DMA1_Stream1_IRQHandler(void)
{
    Handle(INTERRUPT_DMA1_STREAM_1, &(DMA1->LISR), &(DMA1->LIFCR), HANDLE_OPERATION_SET);
}
void DMA1_Stream0_IRQHandler(void)
{
    Handle(INTERRUPT_DMA1_STREAM_0, &(DMA1->LISR), &(DMA1->LIFCR), HANDLE_OPERATION_SET);
}
void DMA2_Stream7_IRQHandler(void)
{
    Handle(INTERRUPT_DMA2_STREAM_7, &(DMA2->HISR), &(DMA2->HIFCR), HANDLE_OPERATION_SET);
}
void DMA2_Stream6_IRQHandler(void)
{
    Handle(INTERRUPT_DMA2_STREAM_6, &(DMA2->HISR), &(DMA2->HIFCR), HANDLE_OPERATION_SET);
}
void DMA2_Stream5_IRQHandler(void)
{
    Handle(INTERRUPT_DMA2_STREAM_5, &(DMA2->HISR), &(DMA2->HIFCR), HANDLE_OPERATION_SET);
}
void DMA2_Stream4_IRQHandler(void)
{
    Handle(INTERRUPT_DMA2_STREAM_4, &(DMA2->HISR), &(DMA2->HIFCR), HANDLE_OPERATION_SET);
}
void DMA2_Stream3_IRQHandler(void)
{
    Handle(INTERRUPT_DMA2_STREAM_3, &(DMA2->LISR), &(DMA2->LIFCR), HANDLE_OPERATION_SET);
}
void DMA2_Stream2_IRQHandler(void)
{
    Handle(INTERRUPT_DMA2_STREAM_2, &(DMA2->LISR), &(DMA2->LIFCR), HANDLE_OPERATION_SET);
}
void DMA2_Stream1_IRQHandler(void)
{
    Handle(INTERRUPT_DMA2_STREAM_1, &(DMA2->LISR), &(DMA2->LIFCR), HANDLE_OPERATION_SET);
}
void DMA2_Stream0_IRQHandler(void)
{
    Handle(INTERRUPT_DMA2_STREAM_0, &(DMA2->LISR), &(DMA2->LIFCR), HANDLE_OPERATION_SET);
}
void SPI1_IRQHandler(void)
{
    Handle(INTERRUPT_SPI_1, &(SPI1->SR), &(SPI1->CR2), HANDLE_OPERATION_AND_NOT);
}
void SPI2_IRQHandler(void)
{
    Handle(INTERRUPT_SPI_2, &(SPI2->SR), &(SPI2->CR2), HANDLE_OPERATION_AND_NOT);
}
void SPI3_IRQHandler(void)
{
    Handle(INTERRUPT_SPI_3, &(SPI3->SR), &(SPI3->CR2), HANDLE_OPERATION_AND_NOT);
}
void USART1_IRQHandler(void)
{
    Handle(INTERRUPT_USART_1, &(USART1->SR), &(USART1->CR1), HANDLE_OPERATION_AND_NOT);
}
void USART2_IRQHandler(void)
{
    Handle(INTERRUPT_USART_2, &(USART2->SR), &(USART2->CR1), HANDLE_OPERATION_AND_NOT);
}
void USART6_IRQHandler(void)
{
    Handle(INTERRUPT_USART_6, &(USART6->SR), &(USART6->CR1), HANDLE_OPERATION_AND_NOT);
}
void ADC_IRQHandler(void)
{
    Handle(INTERRUPT_ADC_1, &(ADC1->SR), &(ADC1->CR1), HANDLE_OPERATION_AND_NOT);
}

static InterruptType GetActualInterruptForExtiBit(uint32_t handleBit)
{
    switch (handleBit)
    {
        case 1:
            return INTERRUPT_EXTI0;
        case 1<<1:
            return INTERRUPT_EXTI1;
        case 1<<2:
            return INTERRUPT_EXTI2;
        case 1<<3:
            return INTERRUPT_EXTI3;
        case 1<<4:
            return INTERRUPT_EXTI4;
        case 1<<5:
        case 1<<6:
        case 1<<7:
        case 1<<8:
        case 1<<9:
            return INTERRUPT_EXTI59;
        default:
            return INTERRUPT_EXTI1015;
    }
}

uint8_t AddInterruptHandler(InterruptType interrupt, InterruptPriority priority, void (*handler)(void* data), void* data)
{
    uint32_t checkBit = InterruptCheckBits[interrupt];
    uint32_t handleBit = InterruptClearBits[interrupt];

    if (!checkBit || !handleBit)
        return 0;

    AddInterruptHandlerManualBit(interrupt, checkBit, handleBit, priority, handler, data);

    return 1;
}

void AddInterruptHandlerManualBit(InterruptType interrupt, uint32_t checkBit, uint32_t handleBit, InterruptPriority priority, void (*handler)(void* data), void* data)
{
    if (interrupt == INTERRUPT_EXTI)
        interrupt = GetActualInterruptForExtiBit(handleBit);

    IRQn_Type irqn = InterruptTypeToIRQnMap[interrupt];
    NVIC_EnableIRQ(irqn);
    NVIC_SetPriority(irqn, NVIC_EncodePriority(3, priority, 0));

    InterruptDataArray* array = &InterruptHandlers[interrupt];

    if (array->size == MAX_INTERRUPTS)
        Abort("Maximum number of interrypt handlers added! Aborting.");

    array->handlers[array->size].handler = handler;
    array->handlers[array->size].data = data;
    array->handlers[array->size].checkBit = checkBit;
    array->handlers[array->size].handleBit = handleBit;

    ++array->size;
}
