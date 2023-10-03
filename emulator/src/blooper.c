#include <SDL2/SDL.h>
#include <getopt.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blooper.h"
#include "framebuffer.h"
#include "keyboard.h"

uint8_t pixels[FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT / 8];
uint8_t colors[(FRAMEBUFFER_WIDTH / 32) * (FRAMEBUFFER_HEIGHT / 32) * 2];

uint16_t pixels_pointer = 0x0000;
uint16_t colors_pointer = 0x0000;
uint8_t mode = 0x00;
uint8_t old_mode = 0x00;

enum commands {
    CmdSetPixelPtr = 0x00,
    CmdSetColorPtr = 0x01,
    CmdSetPixelReadWrite = 0x02,
    CmdSetColorReadWrite = 0x03,
    CmdDecPixelPtr = 0x04,
    CmdDecColorPtr = 0x05,
    CmdKeyboardRead = 0x10,
    CmdAcknowledge = 0xFF
};

enum modes {
    ModePixelReadWrite = 0x00,
    ModeColorReadWrite = 0x01,
    ModePixelPtrSet = 0x02,
    ModePixelPtrSetLow = 0x03,
    ModeColorPtrSet = 0x04,
    ModeColorPtrSetLow = 0x05,
    ModeKeyboardRead = 0x10,
    ModeAcknowledge = 0xFF
};

void blooper_command_write(uint8_t command) {
    switch (command) {
        case CmdSetPixelPtr:
            // set pixels pointer
            mode = ModePixelPtrSet;
            break;
        case CmdSetColorPtr:
            // set colors pointer
            mode = ModeColorPtrSet;
            break;
        case CmdSetPixelReadWrite:
            // set read/write to pixels
            mode = ModePixelReadWrite;
            break;
        case CmdSetColorReadWrite:
            // set read/write to colors
            mode = ModeColorReadWrite;
            break;
        case CmdDecPixelPtr:
            // decrement pixels pointer
            pixels_pointer--;
            break;
        case CmdDecColorPtr:
            // decrement colors pointer
            colors_pointer--;
            break;

        case CmdKeyboardRead:
            // set read from keyboard
            old_mode = mode;
            mode = ModeKeyboardRead;
            break;

        case CmdAcknowledge:
            // set acknowledge
            old_mode = mode;
            mode = ModeAcknowledge;
            break;

        default:
            break;
    }
}

void blooper_data_write(uint8_t data) {
    switch (mode) {
        case ModePixelReadWrite:
            // pixel write
            pixels[pixels_pointer++] = data;
            if (pixels_pointer >= sizeof(pixels))
                pixels_pointer = 0;
            break;
        case ModeColorReadWrite:
            // color write
            colors[colors_pointer++] = data;
            if (colors_pointer >= sizeof(colors))
                colors_pointer = 0;
            break;
        case ModePixelPtrSet:
            // set high byte of pixels pointer
            pixels_pointer = data << 8;
            mode = 3;
            break;
        case ModePixelPtrSetLow:
            // set low byte of pixels pointer
            pixels_pointer = pixels_pointer | data;
            mode = 0;
            break;

        default:
            break;
    }
}

uint8_t blooper_data_read() {
    switch (mode) {
        case ModePixelReadWrite:
            // pixel read
            uint8_t pixel = pixels[pixels_pointer++];
            if (pixels_pointer >= sizeof(pixels))
                pixels_pointer = 0;
            return pixel;
        case ModeColorReadWrite:
            // color read
            uint8_t color = colors[colors_pointer++];
            if (colors_pointer >= sizeof(colors))
                colors_pointer = 0;
            return color;

        case ModeKeyboardRead:
            // keyboard read
            mode = old_mode;
            return (uint8_t)key_take();

        case ModeAcknowledge:
            // acknowledge
            mode = old_mode;
            return 'B';

        default:
            return 0;
    }
}
