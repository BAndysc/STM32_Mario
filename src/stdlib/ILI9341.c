#include "ILI9341.h"

typedef enum {
    ILI9341_COMMAND_SOFTWARE_RESET = 0x01,
    ILI9341_COMMAND_READ_DISPLAY_IDENTIFICATION_INFORMATION = 0x04,
    ILI9341_COMMAND_READ_DISPLAY_STATUS = 0x09,
    ILI9341_COMMAND_READ_DISPLAY_POWER_MODE = 0x0A,
    ILI9341_COMMAND_READ_DISPLAY_MADCTL = 0x0B,
    ILI9341_COMMAND_READ_DISPLAY_PIXEL_FORMAT = 0x0C,
    ILI9341_COMMAND_READ_DISPLAY_IMAGE_FORMAT = 0x0D,
    ILI9341_COMMAND_READ_DISPLAY_SIGNAL_MODE = 0x0E,
    ILI9341_COMMAND_READ_DISPLAY_SELF_DIAGNOSTIC_RESULT = 0x0F,
    ILI9341_COMMAND_ENTER_SLEEP_MODE = 0x10,
    ILI9341_COMMAND_SLEEP_OUT = 0x11,
    ILI9341_COMMAND_PARTIAL_MODE_ON = 0x12,
    ILI9341_COMMAND_NORMAL_DISPLAY_MODE_ON = 0x13,
    ILI9341_COMMAND_DISPLAY_INVERSION_OFF = 0x20,
    ILI9341_COMMAND_DISPLAY_INVERSION_ON = 0x21,
    ILI9341_COMMAND_GAMMA_SET = 0x26,
    ILI9341_COMMAND_DISPLAY_OFF = 0x28,
    ILI9341_COMMAND_DISPLAY_ON = 0x29,
    ILI9341_COMMAND_COLUMN_ADDRESS_SET = 0x2A,
    ILI9341_COMMAND_PAGE_ADDRESS_SET = 0x2B,
    ILI9341_COMMAND_MEMORY_WRITE = 0x2C,
    ILI9341_COMMAND_COLOR_SET = 0x2D,
    ILI9341_COMMAND_MEMORY_READ = 0x2E,
    ILI9341_COMMAND_PARTIAL_AREA = 0x30,
    ILI9341_COMMAND_VERTICAL_SCROLLING_DEFINITION = 0x33,
    ILI9341_COMMAND_TEARING_EFFECT_LINE_OFF = 0x34,
    ILI9341_COMMAND_TEARING_EFFECT_LINE_ON = 0x35,
    ILI9341_COMMAND_MEMORY_ACCESS_CONTROL = 0x36,
    ILI9341_COMMAND_VERTICAL_SCROLLING_START_ADDRESS = 0x37,
    ILI9341_COMMAND_IDLE_MODE_OFF = 0x38,
    ILI9341_COMMAND_IDLE_MODE_ON = 0x39,
    ILI9341_COMMAND_COLMOD_PIXEL_FORMAT_SET = 0x3A,
    ILI9341_COMMAND_WRITE_MEMORY_CONTINUE = 0x3C,
    ILI9341_COMMAND_READ_MEMORY_CONTINUE = 0x3E,
    ILI9341_COMMAND_SET_TEAR_SCANLINE = 0x44,
    ILI9341_COMMAND_GET_SCANLINE = 0x45,
    ILI9341_COMMAND_WRITE_DISPLAY_BRIGHTNESS = 0x51,
    ILI9341_COMMAND_READ_DISPLAY_BRIGHTNESS = 0x52,
    ILI9341_COMMAND_WRITE_CTRL_DISPLAY = 0x53,
    ILI9341_COMMAND_READ_CTRL_DISPLAY = 0x54,
    ILI9341_COMMAND_WRITE_CONTENT_ADAPTIVE_BRIGHTNESS_CONTROL = 0x55,
    ILI9341_COMMAND_READ_CONTENT_ADAPTIVE_BRIGHTNESS_CONTROL = 0x56,
    ILI9341_COMMAND_WRITE_CABC_MINIMUM_BRIGHTNESS = 0x5E,
    ILI9341_COMMAND_READ_CABC_MINIMUM_BRIGHTNESS = 0x5F,
    ILI9341_COMMAND_READ_ID1 = 0xDA,
    ILI9341_COMMAND_READ_ID2 = 0xDB,
    ILI9341_COMMAND_READ_ID3 = 0xDC,
    ILI9341_COMMAND_RGB_INTERFACE_SIGNAL_CONTROL = 0xB0,
    ILI9341_COMMAND_FRAME_RATE_CONTROL_NORMAL = 0xB1,
    ILI9341_COMMAND_FRAME_RATE_CONTROL_IDLE = 0xB2,
    ILI9341_COMMAND_FRAME_RATE_CONTROL_PARTIAL = 0xB3,
    ILI9341_COMMAND_DISPLAY_INVERSION_CONTROL = 0xB4,
    ILI9341_COMMAND_BLANKING_PORCH_CONTROL = 0xB5,
    ILI9341_COMMAND_DISPLAY_FUNCTION_CONTROL = 0xB6,
    ILI9341_COMMAND_ENTRY_MODE_SET = 0xB7,
    ILI9341_COMMAND_BACKLIGHT_CONTROL_1 = 0xB8,
    ILI9341_COMMAND_BACKLIGHT_CONTROL_2 = 0xB9,
    ILI9341_COMMAND_BACKLIGHT_CONTROL_3 = 0xBA,
    ILI9341_COMMAND_BACKLIGHT_CONTROL_4 = 0xBB,
    ILI9341_COMMAND_BACKLIGHT_CONTROL_5 = 0xBC,
    ILI9341_COMMAND_BACKLIGHT_CONTROL_7 = 0xBE,
    ILI9341_COMMAND_BACKLIGHT_CONTROL_8 = 0xBF,
    ILI9341_COMMAND_POWER_CONTROL_1 = 0xC0,
    ILI9341_COMMAND_POWER_CONTROL_2 = 0xC1,
    ILI9341_COMMAND_VCOM_CONTROL_1 = 0xC5,
    ILI9341_COMMAND_VCOM_CONTROL_2 = 0xC7,
    ILI9341_COMMAND_NV_MEMORY_WRITE = 0xD0,
    ILI9341_COMMAND_NV_MEMORY_PROTECTION_KEY = 0xD1,
    ILI9341_COMMAND_NV_MEMORY_STATUS_READ = 0xD2,
    ILI9341_COMMAND_READ_ID4 = 0xD3,
    ILI9341_COMMAND_POSITIVE_GAMMA_CORRECTION = 0xE0,
    ILI9341_COMMAND_NEGATIVE_GAMMA_CORRECTION = 0xE1,
    ILI9341_COMMAND_DIGITAL_GAMMA_CONTROL_1 = 0xE2,
    ILI9341_COMMAND_DIGITAL_GAMMA_CONTROL_2 = 0xE3,
    ILI9341_COMMAND_INTERFACE_CONTROL = 0xF6,
    ILI9341_COMMAND_POWER_CONTROL_A = 0xCB,
    ILI9341_COMMAND_POWER_CONTROL_B = 0xCF,
    ILI9341_COMMAND_DIVER_TIMING_CONTROL_A = 0xE8,
    ILI9341_COMMAND_DIVER_TIMING_CONTROL_B = 0xEA,
    ILI9341_COMMAND_POWER_ON_SEQUENCE_CONTROL = 0xED,
    ILI9341_COMMAND_ENABLE_3_GAMMA = 0xF2,
    ILI9341_COMMAND_PUMP_RADIO_CONTROL = 0xF7
} ILI9341_COMMAND;

LcdInitInstruction InitInstructions[] = {
        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_SOFTWARE_RESET},
        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_DISPLAY_OFF},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_POWER_CONTROL_B},
        {.type = LCD_MSG_DATA, .byte = 0x00}, // always 0
        {.type = LCD_MSG_DATA, .byte = 0x81},
        {.type = LCD_MSG_DATA, .byte = 0x30},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_POWER_ON_SEQUENCE_CONTROL},
        {.type = LCD_MSG_DATA, .byte = 0x64},
        {.type = LCD_MSG_DATA, .byte = 0x03},
        {.type = LCD_MSG_DATA, .byte = 0x12},
        {.type = LCD_MSG_DATA, .byte = 0x81},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_DIVER_TIMING_CONTROL_A},
        {.type = LCD_MSG_DATA, .byte = 0x85},
        {.type = LCD_MSG_DATA, .byte = 0x01},
        {.type = LCD_MSG_DATA, .byte = 0x79},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_POWER_CONTROL_A},
        {.type = LCD_MSG_DATA, .byte = 0x39},
        {.type = LCD_MSG_DATA, .byte = 0x2C},
        {.type = LCD_MSG_DATA, .byte = 0x00},
        {.type = LCD_MSG_DATA, .byte = 0x34},
        {.type = LCD_MSG_DATA, .byte = 0x02},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_PUMP_RADIO_CONTROL},
        {.type = LCD_MSG_DATA, .byte = 0x20},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_DIVER_TIMING_CONTROL_B},
        {.type = LCD_MSG_DATA, .byte = 0x00},
        {.type = LCD_MSG_DATA, .byte = 0x00},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_POWER_CONTROL_1},
        {.type = LCD_MSG_DATA, .byte = 0x26},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_POWER_CONTROL_2},
        {.type = LCD_MSG_DATA, .byte = 0x11},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_VCOM_CONTROL_1},
        {.type = LCD_MSG_DATA, .byte = 0x35},
        {.type = LCD_MSG_DATA, .byte = 0x3E},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_VCOM_CONTROL_2},
        {.type = LCD_MSG_DATA, .byte = 0xBE},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_MEMORY_ACCESS_CONTROL},
        {.type = LCD_MSG_DATA, .byte = 0xE8}, // 48

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_COLMOD_PIXEL_FORMAT_SET},
        {.type = LCD_MSG_DATA, .byte = 0x55}, // 16 bit pixel

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_FRAME_RATE_CONTROL_NORMAL},
        {.type = LCD_MSG_DATA, .byte = 0x00},
        {.type = LCD_MSG_DATA, .byte = 0b11111},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_DIGITAL_GAMMA_CONTROL_1},
        {.type = LCD_MSG_DATA, .byte = 0x08},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_GAMMA_SET},
        {.type = LCD_MSG_DATA, .byte = 0x01}, // gamma set for curve 01/2/04/08

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_POSITIVE_GAMMA_CORRECTION},
        {.type = LCD_MSG_DATA, .byte = 0x1F},
        {.type = LCD_MSG_DATA, .byte = 0x1A},
        {.type = LCD_MSG_DATA, .byte = 0x18},
        {.type = LCD_MSG_DATA, .byte = 0x0A},
        {.type = LCD_MSG_DATA, .byte = 0x0F},
        {.type = LCD_MSG_DATA, .byte = 0x06},
        {.type = LCD_MSG_DATA, .byte = 0x45},
        {.type = LCD_MSG_DATA, .byte = 0x87},
        {.type = LCD_MSG_DATA, .byte = 0x32},
        {.type = LCD_MSG_DATA, .byte = 0x0A},
        {.type = LCD_MSG_DATA, .byte = 0x07},
        {.type = LCD_MSG_DATA, .byte = 0x02},
        {.type = LCD_MSG_DATA, .byte = 0x07},
        {.type = LCD_MSG_DATA, .byte = 0x05},
        {.type = LCD_MSG_DATA, .byte = 0x00},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_NEGATIVE_GAMMA_CORRECTION},
        {.type = LCD_MSG_DATA, .byte = 0x00},
        {.type = LCD_MSG_DATA, .byte = 0x25},
        {.type = LCD_MSG_DATA, .byte = 0x27},
        {.type = LCD_MSG_DATA, .byte = 0x05},
        {.type = LCD_MSG_DATA, .byte = 0x10},
        {.type = LCD_MSG_DATA, .byte = 0x09},
        {.type = LCD_MSG_DATA, .byte = 0x3A},
        {.type = LCD_MSG_DATA, .byte = 0x78},
        {.type = LCD_MSG_DATA, .byte = 0x4D},
        {.type = LCD_MSG_DATA, .byte = 0x05},
        {.type = LCD_MSG_DATA, .byte = 0x18},
        {.type = LCD_MSG_DATA, .byte = 0x0D},
        {.type = LCD_MSG_DATA, .byte = 0x38},
        {.type = LCD_MSG_DATA, .byte = 0x3A},
        {.type = LCD_MSG_DATA, .byte = 0x1F},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_TEARING_EFFECT_LINE_OFF},
        //{.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_TEARING_EFFECT_LINE_ON},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_ENTRY_MODE_SET},
        {.type = LCD_MSG_DATA, .byte = 0x07},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_DISPLAY_FUNCTION_CONTROL},
        {.type = LCD_MSG_DATA, .byte = 0x0A},
        {.type = LCD_MSG_DATA, .byte = 0x82},
        {.type = LCD_MSG_DATA, .byte = 0x27},
        {.type = LCD_MSG_DATA, .byte = 0x00},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_SLEEP_OUT},

        {.type = LCD_MSG_DELAY},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_DISPLAY_ON},

        {.type = LCD_MSG_DELAY},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_COLUMN_ADDRESS_SET},
        {.type = LCD_MSG_DATA16, .byte = 0},
        {.type = LCD_MSG_WIDTH},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_PAGE_ADDRESS_SET},
        {.type = LCD_MSG_DATA16, .byte = 0},
        {.type = LCD_MSG_HEIGHT},

        {.type = LCD_MSG_COMMAND, .byte = ILI9341_COMMAND_MEMORY_WRITE},

        {.type = LCD_MSG_END},
};

void InitILI9341LCD(LCDt* lcd, uint16_t width, uint16_t height, Pin mosi, Pin miso, Pin clock, Pin CS, Pin A0, Pin reset, LCDRenderLine requestLine)
{
    InitLCD(lcd, width, height, mosi, miso, clock, CS, A0, reset, requestLine, InitInstructions);
}