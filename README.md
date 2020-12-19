# ST-Mario 32

![Demo picture of mario](https://raw.githubusercontent.com/BAndysc/STM32_Mario/master/docs/images/stm32_1_s.jpg)
![Demo picture of mario](https://raw.githubusercontent.com/BAndysc/STM32_Mario/master/docs/images/stm32_2_s.jpg)

This is a simple mario clone coded from scratch, for Nucleo STM32 F411 + ILI 9341 screen + analog joystick.

### Pin connection

#### Screen
Connection to ILI 9341 compatibile screen

```
  Nucleo           Screen
    pin              pin
+3V3       |     Screen VCC
GND        |     Screen GND
PD2        |     Screen CS (chip select)
RESET      |     Screen Reset
PC11       |     Screen DC/RS (data/register select)
PA7        |     Screen SDI/MOSI (SPI write)
PA5        |     Screen SPI clock
IORef      |     Screen LED backlight
           |     Screen SDO/MISO (SPI read)
```

#### Joystick

The joystick is cheapest analog two axis + fire button.

```
  Nucleo          Joystick
    pin              pin
AVDD      |          VCC
PA1       |       Vertical axis
PA4       |       Horizonal axis
PB10      |        Fire button
GND       |        GND
```