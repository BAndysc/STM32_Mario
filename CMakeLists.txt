cmake_minimum_required(VERSION 3.6)
project(mario)

set(CMAKE_VERBOSE_MAKEFILE off)

SET(CMAKE_C_COMPILER arm-eabi-gcc)

SET(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -Wall -g -DDMA -O2 -ffunction-sections -fdata-sections")

SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS_INIT} -mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -Wl,--gc-sections -nostartfiles -L/opt/arm/stm32/lds -Tstm32f411re.lds")

set(CMAKE_CXX_STANDARD 11)

include_directories(src/includes/inc src/includes/CMSIS/Include src/includes/CMSIS/Device/ST/STM32F4xx/Include/)

add_definitions(-DSTM32F411xE)

set(SOURCE_FILES
        src/driver/delay.c
        src/driver/fonts.c
        src/driver/gpio.c
        src/driver/lcd.c
        src/driver/sbrk.c
        src/driver/startup_stm32_fpu.c
        src/devices/stm32f411xe.c
        src/game/game.c
        src/game/game.h
        src/game/gameobject.c
        src/game/gameobject.h
        src/game/gfx.c
        src/game/gfx.h
        src/game/input.c
        src/game/input.h
        src/game/levels.c
        src/game/levels.h
        src/game/mario.c
        src/game/mario.h
        src/game/render.c
        src/game/render.h
        src/game/store.c
        src/game/store.h
        src/game/vector.c
        src/game/vector.h
        src/adc.c
        src/adc.h
        src/clock.c
        src/clock.h
        src/buttons.c
        src/buttons.h
        src/debug.c
        src/debug.h
        src/device.h
        src/dma.c
        src/dma.h
        src/interrupts.c
        src/interrupts.h
        src/lcd.c
        src/lcd.h
        src/main.c
        src/pins.c
        src/pins.h
        src/queue.c
        src/queue.h
        src/spi.c
        src/spi.h
        src/timer.c
        src/timer.h
        src/uart.c
        src/uart.h
        src/ILI9341.c
        src/ILI9341.h)

add_executable(mario ${SOURCE_FILES})

SET_TARGET_PROPERTIES(mario PROPERTIES LINK_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -Wl,--gc-sections -nostartfiles -L/opt/arm/stm32/lds -Tstm32f411re.lds")

SET_TARGET_PROPERTIES(mario PROPERTIES OUTPUT_NAME "mario.elf")

add_custom_command(TARGET mario
    POST_BUILD
    COMMAND arm-eabi-objcopy mario.elf mario.bin -O binary)
