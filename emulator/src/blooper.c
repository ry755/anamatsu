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
bool use_region = false;
uint16_t region_x;
uint16_t region_y;
uint16_t region_width;
uint16_t region_height;
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
    CmdSetRegionCoords = 0x20,
    CmdSetRegionSize = 0x21,
    CmdClrRegion = 0x22,
    CmdRegionByteOr = 0x23,
    CmdRegionByteXor = 0x24,
    CmdRegionByteAnd = 0x25,
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
    ModeRegionXSet = 0x20,
    ModeRegionXSetLow = 0x21,
    ModeRegionYSet = 0x22,
    ModeRegionYSetLow = 0x23,
    ModeRegionWidthSet = 0x24,
    ModeRegionWidthSetLow = 0x25,
    ModeRegionHeightSet = 0x26,
    ModeRegionHeightSetLow = 0x27,
    ModeRegionByteOr = 0x28,
    ModeRegionByteXor = 0x29,
    ModeRegionByteAnd = 0x2A,
    ModeAcknowledge = 0xFF
};

static inline void increment_pixels_pointer() {
    if (use_region) {
        pixels_pointer += (region_width + FRAMEBUFFER_WIDTH) / 8;
        if (pixels_pointer >= ((region_y * FRAMEBUFFER_WIDTH) + region_x + region_width) / 8)
            pixels_pointer -= region_width / 8;
    } else if (++pixels_pointer >= sizeof(pixels)) {
        pixels_pointer = 0;
    }
}

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

        case CmdSetRegionCoords:
            // set region coordinates
            mode = ModeRegionXSet;
            break;
        case CmdSetRegionSize:
            // set region size
            mode = ModeRegionWidthSet;
            break;
        case CmdClrRegion:
            // disable region
            use_region = false;
            mode = ModePixelPtrSet;
            break;

        case CmdRegionByteOr:
            mode = ModeRegionByteOr;
            break;
        case CmdRegionByteXor:
            mode = ModeRegionByteXor;
            break;
        case CmdRegionByteAnd:
            mode = ModeRegionByteAnd;
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
        // raw byte reads and writes
        case ModePixelReadWrite:
            // pixel write
            pixels[pixels_pointer] = data;
            increment_pixels_pointer();
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
            mode = ModePixelPtrSetLow;
            use_region = false;
            break;
        case ModePixelPtrSetLow:
            // set low byte of pixels pointer
            pixels_pointer = pixels_pointer | data;
            mode = ModePixelReadWrite;
            use_region = false;
            break;
        case ModeColorPtrSet:
            // set high byte of colors pointer
            colors_pointer = data << 8;
            mode = ModeColorPtrSetLow;
            break;
        case ModeColorPtrSetLow:
            // set low byte of colors pointer
            colors_pointer = colors_pointer | data;
            mode = ModePixelReadWrite;
            break;

        // region coordinates
        case ModeRegionXSet:
            // set high byte of region X coordinate
            region_x = data << 8;
            mode = ModeRegionXSetLow;
            break;
        case ModeRegionXSetLow:
            // set low byte of region X coordinate
            region_x = region_x | data;
            mode = ModeRegionYSet;
            break;
        case ModeRegionYSet:
            // set high byte of region Y coordinate
            region_y = data << 8;
            mode = ModeRegionYSetLow;
            break;
        case ModeRegionYSetLow:
            // set low byte of region Y coordinate
            region_y = region_y | data;
            mode = ModePixelReadWrite;
            use_region = true;
            pixels_pointer = ((region_y * FRAMEBUFFER_WIDTH) + region_x) / 8;
            break;

        // region size
        case ModeRegionWidthSet:
            // set high byte of region width
            region_width = data << 8;
            mode = ModeRegionWidthSetLow;
            break;
        case ModeRegionWidthSetLow:
            // set low byte of region width
            region_width = region_width | data;
            mode = ModeRegionHeightSet;
            break;
        case ModeRegionHeightSet:
            // set high byte of region height
            region_height = data << 8;
            mode = ModeRegionHeightSetLow;
            break;
        case ModeRegionHeightSetLow:
            // set low byte of region height
            region_height = region_height | data;
            mode = ModePixelReadWrite;
            use_region = true;
            pixels_pointer = ((region_y * FRAMEBUFFER_WIDTH) + region_x) / 8;
            break;

        // region bitwise operations
        case ModeRegionByteOr:
            // bitwise OR the incoming byte with the byte at pixels_pointer
            pixels[pixels_pointer] = pixels[pixels_pointer] | data;
            increment_pixels_pointer();
            break;
        case ModeRegionByteXor:
            // bitwise XOR the incoming byte with the byte at pixels_pointer
            pixels[pixels_pointer] = pixels[pixels_pointer] ^ data;
            increment_pixels_pointer();
            break;
        case ModeRegionByteAnd:
            // bitwise AND the incoming byte with the byte at pixels_pointer
            pixels[pixels_pointer] = pixels[pixels_pointer] & data;
            increment_pixels_pointer();
            break;

        default:
            break;
    }
}

uint8_t blooper_data_read() {
    switch (mode) {
        case ModePixelReadWrite: {
            // pixel read
            uint8_t pixel = pixels[pixels_pointer];
            increment_pixels_pointer();
            if (pixels_pointer >= sizeof(pixels))
                pixels_pointer = 0;
            return pixel;
        }

        case ModeColorReadWrite: {
            // color read
            uint8_t color = colors[colors_pointer++];
            if (colors_pointer >= sizeof(colors))
                colors_pointer = 0;
            return color;
        }

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
