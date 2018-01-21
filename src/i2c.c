#include <gpio.h>
#include <stm32.h>
#include "uart.h"
#include "timer.h"
#include "i2c.h"
#include "leds.h"
#include "debug.h"

#define LIS35DE_ADDR 0b0011100

static void TurnClockOn()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
}

static void ConfigureAf()
{
	GPIOafConfigure(GPIOB, 8, GPIO_OType_OD, GPIO_Low_Speed, GPIO_PuPd_NOPULL, GPIO_AF_I2C1);
	GPIOafConfigure(GPIOB, 9, GPIO_OType_OD, GPIO_Low_Speed, GPIO_PuPd_NOPULL, GPIO_AF_I2C1);
}

int8_t ReadI2C(uint32_t addr, uint32_t reg)
{
	I2C1->CR1 |= I2C_CR1_START;
	while (!(I2C1->SR1 & I2C_SR1_SB));
	I2C1->DR = addr << 1;

	while (!(I2C1->SR1 & I2C_SR1_ADDR));

	I2C1->SR2;

	I2C1->DR = reg;

	while (!(I2C1->SR1 & I2C_SR1_BTF));
	I2C1->CR1 |= I2C_CR1_START;
	while (!(I2C1->SR1 & I2C_SR1_SB));

	I2C1->DR = (addr << 1) | 1U;

	I2C1->CR1 &= ~I2C_CR1_ACK;
	while (!(I2C1->SR1 & I2C_SR1_ADDR));
	I2C1->SR2;

	I2C1->CR1 |= I2C_CR1_STOP;
	while (!(I2C1->SR1 & I2C_SR1_RXNE));

	int32_t value = I2C1->DR;
	return value;
}

int val = 0;

void sprawdz()
{
	int32_t valueX = ReadI2C(LIS35DE_ADDR, 0x29) + 60;
	int32_t valueY = ReadI2C(LIS35DE_ADDR, 0x2B) + 60;

	TIM4->CR1 |= TIM_CR1_UDIS;

	//val +=150;
	//val %= 9999;
	if (valueX >= 1000000 || valueX <= -1000000)
		valueX =0;
	TIM4->CCR1 = valueX;
	DebugInt(valueY);
	//UsartSend("    ");
	//UartSendInt(valueY);
	Debug("\r\n");
	TIM4->CR1 &= ~TIM_CR1_UDIS;

	if (valueX > 0)
		TIM3->CCR1 = valueX * 1000 / 120;

	if (valueY > 0)
		TIM3->CCR2 = valueY * 1000 / 120;
}

void InitI2C()
{
	TurnClockOn();
	ConfigureAf();

	I2C1->CR1 = 0;

	#define I2C_SPEED_HZ 100000
	#define PCLK1_MHZ 16
	I2C1->CR2 = PCLK1_MHZ;
	I2C1->CCR = (PCLK1_MHZ * 1000000) / (I2C_SPEED_HZ << 1);
	I2C1->TRISE = PCLK1_MHZ + 1;
	I2C1->CR1 |= I2C_CR1_PE;

	I2C1->CR1 |= I2C_CR1_START;

	while (!(I2C1->SR1 & I2C_SR1_SB));

	I2C1->DR = LIS35DE_ADDR << 1;

	while (!(I2C1->SR1 & I2C_SR1_ADDR));

	I2C1->SR2;

	I2C1->DR = 0x20;


	while (!(I2C1->SR1 & I2C_SR1_TXE));

	I2C1->DR = 0b01000111;

	while (!(I2C1->SR1 & I2C_SR1_BTF));

	I2C1->CR1 |= I2C_CR1_STOP;

	//SetTimerHandler(sprawdz);


}
