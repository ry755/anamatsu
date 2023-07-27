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

void blooper_command_write(uint8_t command) {
    switch (command) {
        case 0x00:
            // set pixels pointer
            mode = 0x02;
            break;
        case 0x01:
            // set colors pointer
            mode = 0x04;
            break;
        case 0x02:
            // set read/write to pixels
            mode = 0x00;
            break;
        case 0x03:
            // set read/write to colors
            mode = 0x01;
            break;
        case 0x04:
            // decrement pixels pointer
            pixels_pointer--;
            break;
        case 0x05:
            // decrement colors pointer
            colors_pointer--;
            break;

        case 0x10:
            // set read from keyboard
            old_mode = mode;
            mode = 0x10;
            break;

        case 0xFF:
            // set acknowledge
            old_mode = mode;
            mode = 0xFF;
            break;

        default:
            break;
    }
}

void blooper_data_write(uint8_t data) {
    switch (mode) {
        case 0x00:
            // pixel write
            pixels[pixels_pointer++] = data;
            if (pixels_pointer >= sizeof(pixels))
                pixels_pointer = 0;
            break;
        case 0x01:
            // color write
            colors[colors_pointer++] = data;
            if (colors_pointer >= sizeof(colors))
                colors_pointer = 0;
            break;
        case 0x02:
            // set high byte of pixels pointer
            pixels_pointer = data << 8;
            mode = 3;
            break;
        case 0x03:
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
        case 0x00:
            // pixel read
            uint8_t pixel = pixels[pixels_pointer++];
            if (pixels_pointer >= sizeof(pixels))
                pixels_pointer = 0;
            return pixel;
        case 0x01:
            // color read
            uint8_t color = colors[colors_pointer++];
            if (colors_pointer >= sizeof(colors))
                colors_pointer = 0;
            return color;

        case 0x10:
            // keyboard read
            mode = old_mode;
            return (uint8_t)key_take();

        case 0xFF:
            // acknowledge
            mode = old_mode;
            return 'B';

        default:
            return 0;
    }
}
