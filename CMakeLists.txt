cmake_minimum_required(VERSION 3.12)
project(mario)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_VERBOSE_MAKEFILE off)

SET(CMAKE_C_COMPILER arm-none-eabi-gcc)

SET(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -Wall -g -DDMA -O2 -ffunction-sections -fdata-sections -Wno-trigraphs")

SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS_INIT} -mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -Wl,--gc-sections -nostartfiles -L/opt/arm/stm32/lds -Tstm32f411re.lds")

set(CMAKE_CXX_STANDARD 11)

include_directories(src/includes/inc src/stdlib src/includes/CMSIS/Include src/includes/CMSIS/Device/ST/STM32F4xx/Include/)

add_definitions(-DSTM32F411xE -DUSE_FLOATS)

file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_SOURCE_DIR} 
        "src/game/*.c"
        "src/stdlib/*.c"
        "src/driver/*.c"
        "src/devices/*.c")

add_executable(mario ${SOURCE_FILES} src/main.c)

SET_TARGET_PROPERTIES(mario PROPERTIES LINK_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -Wl,--gc-sections -nostartfiles -L${CMAKE_CURRENT_SOURCE_DIR}/src/devices -Tstm32f411re.lds")

SET_TARGET_PROPERTIES(mario PROPERTIES OUTPUT_NAME "mario.elf")

add_custom_command(TARGET mario
    POST_BUILD
    COMMAND arm-none-eabi-objcopy mario.elf mario.bin -O binary)
