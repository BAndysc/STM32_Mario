#ifdef STM32F411xE

#include <stddef.h>
#include "../stdlib/device.h"


// nucleo f411xx
Pin SPI1_CLOCK = {.gpio = GPIOA, .pin = 5};
Pin SPI1_MISO = {.gpio = GPIOA, .pin = 6};
Pin SPI1_MOSI = {.gpio = GPIOA, .pin = 7};


Pin SPI2_CLOCK = {.gpio = GPIOB, .pin = 13};
Pin SPI2_MISO = {.gpio = GPIOB, .pin = 14};
Pin SPI2_MOSI = {.gpio = GPIOB, .pin = 15};


Pin SPI3_CLOCK = {.gpio = GPIOC, .pin = 10};
Pin SPI3_MISO = {.gpio = GPIOC, .pin = 11};
Pin SPI3_MOSI = {.gpio = GPIOC, .pin = 12};

uint32_t GPIO_AF_SPI1 = 0x5;
uint32_t GPIO_AF_SPI2 = 0x5;

DeviceSPI GetHardwareSPIForPin(Pin clock, Pin mosi, Pin miso)
{
    DeviceSPI spi;
    spi.SPI = 0;
    spi.AlternateFunction = 0;

    if (PinsEq(clock, SPI1_CLOCK) && PinsEq(mosi, SPI1_MOSI) && PinsEq(miso, SPI1_MISO))
    {
        spi.SPI = SPI1;
        spi.AlternateFunction = GPIO_AF_SPI1;
        spi.DMAtx.Number = 2;
        spi.DMAtx.Stream = 2;
        spi.DMAtx.Channel = 2;
        spi.Interrupt = INTERRUPT_SPI_1;
    }
    else if (PinsEq(clock, SPI2_CLOCK) && PinsEq(mosi, SPI2_MOSI) && PinsEq(miso, SPI2_MISO))
    {
        spi.SPI = SPI2;
        spi.AlternateFunction = GPIO_AF_SPI2;
        spi.DMAtx.Number = 1;
        spi.DMAtx.Stream = 4;
        spi.DMAtx.Channel = 0;
        spi.Interrupt = INTERRUPT_SPI_2;
    }
    else if (PinsEq(clock, SPI3_CLOCK) && PinsEq(mosi, SPI3_MOSI) && PinsEq(miso, SPI3_MISO))
    {
        spi.SPI = SPI3;
        spi.AlternateFunction = GPIO_AF_SPI3;
        spi.DMAtx.Number = 1;
        spi.DMAtx.Stream = 5;
        spi.DMAtx.Channel = 0;
        spi.Interrupt = INTERRUPT_SPI_3;
    }

    return spi;
}


////////////////////////////////////////////////
// DMA
////////////////////////////////////////////////

static DMA_Stream_TypeDef* DMA_1_Mapping[] = {
    DMA1_Stream0,
    DMA1_Stream1,
    DMA1_Stream2,
    DMA1_Stream3,
    DMA1_Stream4,
    DMA1_Stream5,
    DMA1_Stream6,
    DMA1_Stream7
};

static DMA_Stream_TypeDef* DMA_2_Mapping[] = {
    DMA2_Stream0,
    DMA2_Stream1,
    DMA2_Stream2,
    DMA2_Stream3,
    DMA2_Stream4,
    DMA2_Stream5,
    DMA2_Stream6,
    DMA2_Stream7
};

static DMA_Stream_TypeDef** DMA_Mapping[] = {
    DMA_1_Mapping,
    DMA_2_Mapping
};

static InterruptType DMA_1_Interrupt_Mapping[] = {
    INTERRUPT_DMA1_STREAM_0,
    INTERRUPT_DMA1_STREAM_1,
    INTERRUPT_DMA1_STREAM_2,
    INTERRUPT_DMA1_STREAM_3,
    INTERRUPT_DMA1_STREAM_4,
    INTERRUPT_DMA1_STREAM_5,
    INTERRUPT_DMA1_STREAM_6,
    INTERRUPT_DMA1_STREAM_7,
};

static InterruptType DMA_2_Interrupt_Mapping[] = {
    INTERRUPT_DMA2_STREAM_0,
    INTERRUPT_DMA2_STREAM_1,
    INTERRUPT_DMA2_STREAM_2,
    INTERRUPT_DMA2_STREAM_3,
    INTERRUPT_DMA2_STREAM_4,
    INTERRUPT_DMA2_STREAM_5,
    INTERRUPT_DMA2_STREAM_6,
    INTERRUPT_DMA2_STREAM_7,
};

static InterruptType* DMA_Interrupt_Mapping[] = {
    DMA_1_Interrupt_Mapping,
    DMA_2_Interrupt_Mapping
};


DeviceDMA GetDMA(uint8_t number, uint8_t stream, uint8_t channel)
{
    DeviceDMA dma;
    dma.Ptr = DMA_Mapping[number - 1][stream];
    dma.Interrupt = DMA_Interrupt_Mapping[number - 1][stream];

    return dma;
}


/////////////////////////////////////////////
// ADC
/////////////////////////////////////////////

#define INVALID_PIN_FOR_ANALOG_IN 255

static uint8_t GetChannelForPin(Pin pin)
{
    if (pin.gpio == GPIOA && pin.pin <= 7)
        return pin.pin;

    if (pin.gpio == GPIOB)
    {
        if (pin.pin == 0)
            return 8;
        else if (pin.pin == 1)
            return 1;
    }

    if (pin.gpio == GPIOC)
    {
        if (pin.pin == 4)
            return 14;
        else if (pin.pin == 5)
            return 15;
    }

    return INVALID_PIN_FOR_ANALOG_IN; // ten pin nie może działać jako wejście analogowe
}

DeviceADC GetADCForPin(Pin pin)
{
    DeviceADC adc;
    adc.Ptr = 0;
    adc.Channel = GetChannelForPin(pin);

    if (adc.Channel != INVALID_PIN_FOR_ANALOG_IN)
    {
        adc.Ptr = ADC1;
        adc.Dma.Number = 2;
        adc.Dma.Stream = 0;
        adc.Dma.Channel = 0;
        adc.Interrupt = INTERRUPT_ADC_1;
    }

    return adc;
}

////

void TurnTimerClockOn(TIM_TypeDef* timer)
{
    if (timer == TIM1)
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    else if (timer == TIM2)
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    else if (timer == TIM3)
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    else if (timer == TIM4)
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    else if (timer == TIM5)
        RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    else if (timer == TIM9)
        RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
    else if (timer == TIM10)
        RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
    else if (timer == TIM11)
        RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
}


InterruptType GetInterruptForTimer(TIM_TypeDef* timer)
{
    if (timer == TIM1)
        return INTERRUPT_TIMER1;
    else if (timer == TIM2)
        return INTERRUPT_TIMER2;
    else if (timer == TIM3)
        return INTERRUPT_TIMER3;
    else if (timer == TIM4)
        return INTERRUPT_TIMER4;
    else if (timer == TIM5)
        return INTERRUPT_TIMER5;
    else if (timer == TIM9)
        return INTERRUPT_TIMER9;
    else if (timer == TIM10)
        return INTERRUPT_TIMER10;
    else if (timer == TIM11)
        return INTERRUPT_TIMER11;
    return INTERRUPT_MAX;
}

//// USART

Pin USART2_TX = {.gpio = GPIOA, .pin = 2};
Pin USART2_RX = {.gpio = GPIOA, .pin = 3};

DeviceUSART GetUSARTForPin(Pin tx, Pin rx)
{
    DeviceUSART usart;
    usart.Ptr = 0;
    if (PinsEq(tx, USART2_TX) && PinsEq(rx, USART2_RX))
    {
        usart.Ptr = USART2;
        usart.DmaTX.Number = 1;
        usart.DmaTX.Stream = 6;
        usart.DmaTX.Channel = 4;
        usart.DmaRX.Number = 1;
        usart.DmaRX.Stream = 5;
        usart.DmaRX.Channel = 4;

        usart.AlternateFunction = GPIO_AF_USART2;
        usart.Interrupt = INTERRUPT_USART_2;
    }

    return usart;
}


//// CLOCKS

void GPIOClockEnable(Pin pin)
{
    if (pin.gpio == GPIOA)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    else if (pin.gpio == GPIOB)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    else if (pin.gpio == GPIOC)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    else if (pin.gpio == GPIOD)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    else if (pin.gpio == GPIOE)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
}

void GPIOClockEnableSPI(SPI_TypeDef* spi)
{
    if (spi == SPI1)
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    else if (spi == SPI2)
        RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    else if (spi == SPI3)
        RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

}

void GPIOClockEnableUSART(USART_TypeDef* usart)
{
    if (usart == USART2)
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
}

void GPIOClockEnableADC(ADC_TypeDef* adc)
{
    if (adc == ADC1)
        RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}

void GPIOClockEnableDMA(uint8_t number)
{
    if (number == 1)
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    else if (number == 2)
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
}

///// TIMER

TIM_TypeDef* Timers16[] = {TIM3, TIM4, nullptr};
TIM_TypeDef* Timers32[] = {TIM2, TIM5, nullptr};

InterruptType Timer16Interrupts[] = {INTERRUPT_TIMER3, INTERRUPT_TIMER4, 0};
InterruptType Timer32Interrupts[] = {INTERRUPT_TIMER2, INTERRUPT_TIMER5, 0};

uint8_t CurrentFreeTimer16 = 0;
uint8_t CurrentFreeTimer32 = 0;

DeviceTimer GetNextUnusedTimer16()
{
    DeviceTimer timer;
    timer.Ptr = Timers16[CurrentFreeTimer16];
    timer.Interrupt = Timer16Interrupts[CurrentFreeTimer16];

    if (timer.Ptr != nullptr)
        CurrentFreeTimer16++;

    return timer;
}

DeviceTimer GetNextUnusedTimer32()
{
    DeviceTimer timer;
    timer.Ptr = Timers32[CurrentFreeTimer32];
    timer.Interrupt = Timer32Interrupts[CurrentFreeTimer32];

    if (timer.Ptr != nullptr)
        CurrentFreeTimer32++;

    return timer;
}

#endif