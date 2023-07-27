#include <SDL2/SDL.h>
#include <getopt.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "framebuffer.h"
#include "screen.h"

static uint8_t final_framebuffer[FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT];

extern uint8_t pixels[FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT / 8];
extern uint8_t colors[(FRAMEBUFFER_WIDTH / 32) * (FRAMEBUFFER_HEIGHT / 32) * 2];

void draw_framebuffer(struct Screen *screen) {
    SDL_Texture *texture = ScreenGetTexture(screen);

    // FIXME: colors are currently hard-coded to black and white for 0 and 1 respectively, the color table is not used
    for (int y = 0; y < FRAMEBUFFER_HEIGHT; y++) {
        for (int x = 0; x < FRAMEBUFFER_WIDTH; x += 8) {
            for (int bit = 0; bit < 8; bit++) {
                uint8_t pixel = (pixels[(y * FRAMEBUFFER_WIDTH / 8) + (x / 8)] >> bit) & 1;
                final_framebuffer[(y * FRAMEBUFFER_WIDTH) + x + bit] = pixel ? 0xFF : 0x00;
            }
        }
    }

    SDL_UpdateTexture(texture, NULL, final_framebuffer, FRAMEBUFFER_WIDTH);
}
