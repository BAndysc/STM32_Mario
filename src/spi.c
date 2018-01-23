#include <string.h>
#include <stdbool.h>
#include <stm32f411xe.h>
#include "spi.h"
#include "interrupts.h"
#include "device.h"
#include "debug.h"
#include "dma.h"

static void SPIDmaDidSent(void* data);
static void SpiBufferEmpty(void* data);

static bool SpiTransmitBufferEmpty(SPIt* spi)
{
    return (spi->hardwareSPI->SR & SPI_SR_TXE) != 0;
}

static bool SpiBusy(SPIt* spi)
{
    return (spi->hardwareSPI->SR & SPI_SR_BSY) != 0;
}

static void writeAsync(struct SPIt* spi, char* data, int length)
{
    spi->dmaTX.transmit(&spi->dmaTX, data, (void*)&spi->hardwareSPI->DR, length);
}

static void writeSoftwareByte(struct SPIt* spi, char data)
{
    uint32_t mask;
    uint32_t length = 8;

    mask = 1U << (length - 1);
    while (length > 0) {
        SetPin(spi->MOSI, (data & mask) != 0); /* Set bit. */
        --length;                              /* Add some delay. */
        SetLow(spi->Clock);                      /* Rising edge writes bit. */
        mask >>= 1;                              /* Add some delay. */
        SetHigh(spi->Clock);                      /* Falling edge ends the bit transmission. */
    }
}

static void writeSoftwareBytes(struct SPIt* spi, char* data, int length)
{
    for (int i = 0; i < length; ++i)
        writeSoftwareByte(spi, data[i]);
}

static void writeSoftwareBytesAsyncFake(struct  SPIt* spi, char* data, int length)
{
    writeSoftwareBytes(spi, data, length);
    if (spi->onFinishedWrite)
        spi->onFinishedWrite(spi);
}

static void writeSyncByte(struct SPIt* spi, char data)
{
    while (SpiBusy(spi))
        __NOP();

    while (!SpiTransmitBufferEmpty(spi))
        __NOP();

    spi->hardwareSPI->DR = (uint32_t)data;

    while (!SpiTransmitBufferEmpty(spi))
        __NOP();

    while (SpiBusy(spi))
        __NOP();
}

static void writeSyncWord(struct SPIt* spi, uint16_t data)
{
    while (SpiBusy(spi))
        __NOP();

    while (!SpiTransmitBufferEmpty(spi))
        __NOP();

    spi->hardwareSPI->DR = data;

    while (!SpiTransmitBufferEmpty(spi))
        __NOP();

    while (SpiBusy(spi))
        __NOP();
}

static void writeSync(struct SPIt* spi, char* data, int length)
{
    if (spi->hardwareSPI->CR1 & SPI_CR1_DFF)
    {
        for (int i = 0; i < length / 2; ++i)
            writeSyncWord(spi, ((uint16_t*)data)[i]);

        if ((length % 2) != 0)
        {
            spi->hardwareSPI->CR1 &= ~SPI_CR1_DFF;
            writeSyncByte(spi, data[length - 1]);
            spi->hardwareSPI->CR1 |= SPI_CR1_DFF;
        }
    }
    else
    {
        for (int i = 0; i < length; ++i)
            writeSyncByte(spi, data[i]);
    }

}

static void write8(struct SPIt* spi, uint8_t data)
{
    spi->writeSync(spi, (char*)&data, 1);
}

static void write16(struct SPIt* spi, uint16_t data)
{
    spi->writeSync(spi, (char*)&data, 2);
}

static void EnableSPI(SPIt* spi)
{
    spi->hardwareSPI->CR1 |= SPI_CR1_SPE;
}

static void EnableDMA(SPIt* spi)
{
    spi->hardwareSPI->CR2 = SPI_CR2_TXDMAEN;
}


void SpiConfigure(SPIt* spi, SPI_Configuration* configuration)
{
    spi->hardwareSPI->CR1 = 0;

    if (configuration->BidiMode == SPI_BIDI_MODE_1_LINE_BI)
        spi->hardwareSPI->CR1 |= SPI_CR1_BIDIMODE;

    if (configuration->ByteOrder == SPI_LEAST_SIG_BYTE_FIRST)
        spi->hardwareSPI->CR1 |= SPI_CR1_LSBFIRST;

    if (configuration->TransferSize == SPI_TRANSFER_SIZE_HALF_WORD)
        spi->hardwareSPI->CR1 |= SPI_CR1_DFF;

    if (configuration->SlaveManagment == SPI_SLAVE_MANAGMENT_SOFTWARE)
        spi->hardwareSPI->CR1 |= SPI_CR1_SSM;

    if (configuration->InternalSlaveSelect)
        spi->hardwareSPI->CR1 |= SPI_CR1_SSI;

    if (configuration->Role == SPI_ROLE_MASTER)
        spi->hardwareSPI->CR1 |= SPI_CR1_MSTR;

    if (configuration->ClockPolarity == SPI_CLOCK_1_WHEN_IDLE)
        spi->hardwareSPI->CR1 |= SPI_CR1_CPOL;

    if (configuration->ClockPhase == SPI_SECOND_CLOCK_TRANSITION_IS_FIRST_DATA_CAPTURE_EDGE)
        spi->hardwareSPI->CR1 |= SPI_CR1_CPHA;

    if (configuration->CalculateCRC)
        spi->hardwareSPI->CR1 |= SPI_CR1_CRCEN;

    if (configuration->BaudRatePrescaler & 1)
        spi->hardwareSPI->CR1 |= SPI_CR1_BR_0;
    if (configuration->BaudRatePrescaler & 2)
        spi->hardwareSPI->CR1 |= SPI_CR1_BR_1;
    if (configuration->BaudRatePrescaler & 4)
        spi->hardwareSPI->CR1 |= SPI_CR1_BR_2;

    if (configuration->BidiMode == SPI_BIDI_MODE_1_LINE_BI)
    {
        if (configuration->Direction == SPI_TRANSMIT_ONLY)
            spi->hardwareSPI->CR1 |= SPI_CR1_BIDIOE;
        else if (configuration->Direction == SPI_TRANSMIT_AND_RECEIVE)
            Abort("Duplex mode and directional mode is not allowed!");
    }
    else
    {
        if (configuration->Direction == SPI_TRANSMIT_AND_RECEIVE) {}
        else if (configuration->Direction == SPI_RECEIVE_ONLY)
            spi->hardwareSPI->CR1 |= SPI_CR1_RXONLY;
        else
            Abort("Transmit only and unidirectional mode is not allowed!");
    }
}


static void SetupDMA(SPIt* spi, DmaData dmatx)
{
    DMA_Configuration config;
    config.MemoryIncrement = true;
    config.PeripherialIncrement = false;
    config.Priority = DMA_Priority_Low;
    config.Direction = DMA_Dir_Memory_Peripherial;
    config.Circular = DMA_CIRCULAR_NO;
    InitDMA(&spi->dmaTX, dmatx.Number, dmatx.Stream, dmatx.Channel, &config);
    DmaSetHandler(&spi->dmaTX, INTERRUPT_PRIORITY_LOW, &SPIDmaDidSent, spi);
}

static void TurnClocks(SPIt* spi)
{
    GPIOClockEnableSPI(spi->hardwareSPI);
}

void InitSPI(SPIt* spi, Pin clock, Pin mosi, Pin miso, SPI_Configuration* configuration)
{
    spi->Clock = clock;
    spi->MOSI = mosi;
    spi->MISO = miso;
    spi->writeSync = &writeSync;
    spi->writeAsync = &writeAsync;
    spi->write8 = &write8;
    spi->write16 = &write16;

    DeviceSPI hardwareSPI = GetHardwareSPIForPin(clock, mosi, miso);

    if (!hardwareSPI.SPI)
    {
        Debug("Pins do not match any hardware SPI! Fallback to software SPI");
        spi->writeAsync = &writeSoftwareBytesAsyncFake;
        spi->writeSync = &writeSoftwareBytes;
        PinConfigureOut(miso, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_NOPULL);
        PinConfigureOut(mosi, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_NOPULL);
        PinConfigureOut(clock, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_NOPULL);
    }
    else
    {
        spi->hardwareSPI = hardwareSPI.SPI;
        PinConfigureAlternativeFunc(miso, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_NOPULL, hardwareSPI.AlternateFunction);
        PinConfigureAlternativeFunc(mosi, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_NOPULL, hardwareSPI.AlternateFunction);
        PinConfigureAlternativeFunc(clock, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_NOPULL, hardwareSPI.AlternateFunction);
        TurnClocks(spi);
        SpiConfigure(spi, configuration);
        EnableDMA(spi);
        SetupDMA(spi, hardwareSPI.DMAtx);
        AddInterruptHandlerManualBit(hardwareSPI.Interrupt, SPI_SR_TXE, SPI_CR2_TXEIE, INTERRUPT_PRIORITY_MEDIUM, &SpiBufferEmpty, spi);
        EnableSPI(spi);
    }
}

void SPISetHandler(SPIt* spi, void (*handler)(void* data), void* data)
{
    spi->onFinishedWrite = handler;
    spi->handlerData = data;
}

void SPISetTranfserSize(SPIt* spi, SPI_TransferSize transferSize)
{
    if (transferSize == SPI_TRANSFER_SIZE_BYTE)
    {
        spi->hardwareSPI->CR1 &= ~SPI_CR1_DFF;
        DmaSetTransferSize(&spi->dmaTX, DMA_DEVICE_MASTER, DMA_TRANSFER_SIZE_BYTE);
        DmaSetTransferSize(&spi->dmaTX, DMA_DEVICE_PERIPHERIAL, DMA_TRANSFER_SIZE_BYTE);
    }
    else
    {
        spi->hardwareSPI->CR1 |= SPI_CR1_DFF;
        DmaSetTransferSize(&spi->dmaTX, DMA_DEVICE_MASTER, DMA_TRANSFER_SIZE_HALF_WORD);
        DmaSetTransferSize(&spi->dmaTX, DMA_DEVICE_PERIPHERIAL, DMA_TRANSFER_SIZE_HALF_WORD);
    }
}

void SpiBufferEmpty(void* data)
{
    SPIt* spi = (SPIt*)data;
    while (SpiBusy(spi))
        __NOP();

    if (spi->onFinishedWrite)
        spi->onFinishedWrite(spi->handlerData);
}

static void SPIDmaDidSent(void* data)
{
    SPIt* spi = (SPIt*)data;

    spi->hardwareSPI->CR2 |= SPI_CR2_TXEIE;
}
