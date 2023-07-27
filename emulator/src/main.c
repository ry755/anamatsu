#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "blooper.h"
#include "cpu.h"
#include "framebuffer.h"
#include "keyboard.h"
#include "screen.h"
#include "../musashi/m68k.h"

#define FPS 60
#define TPF 1
#define TPS (FPS * TPF)

void main_loop(void);
void exit_error(char* fmt, ...);

extern unsigned char rom[];

uint32_t tick_start;
uint32_t tick_end;
int ticks = 0;
bool done = false;

int main(int argc, char* argv[]) {
    FILE* fhandle;

    if(argc != 2) {
        printf("Usage: %s <program file>\n", argv[0]);
        exit(-1);
    }

    if((fhandle = fopen(argv[1], "rb")) == NULL)
        exit_error("Unable to open %s", argv[1]);

    if(fread(rom, 1, ROM_SIZE, fhandle) <= 0)
        exit_error("Error reading %s", argv[1]);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        exit_error("unable to initialize SDL: %s", SDL_GetError());
    }

    SDL_ShowCursor(SDL_DISABLE);

    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    m68k_pulse_reset();
    //m68k_set_instr_hook_callback(instr_callback);

    ScreenCreate(
        FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT,
        draw_framebuffer,
        key_pressed,
        key_released,
        NULL,
        NULL,
        NULL,
        NULL
    );

    ScreenInit();
    ScreenDraw();

    tick_start = SDL_GetTicks();
    tick_end = SDL_GetTicks();

    while (!done) {
        main_loop();

        tick_end = SDL_GetTicks();
        int delay = 1000/TPS - (tick_end - tick_start);
        if (delay > 0) {
            SDL_Delay(delay);
        } else {
            //printf("time overrun %d\n", delay);
        }
    }

    return 0;
}

void main_loop(void) {
    int dt = SDL_GetTicks() - tick_start;
    tick_start = SDL_GetTicks();
    if (!dt)
        dt = 1;

    int cycles_per_tick = CPU_HZ / TPS / dt;
    int extra_cycles = CPU_HZ / TPS - (cycles_per_tick * dt);

    for (int i = 0; i < dt; i++) {
        int cycles_left = cycles_per_tick;

        if (i == dt - 1)
            cycles_left += extra_cycles;

        while (cycles_left > 0) {
            uint32_t executed = m68k_execute(cycles_left);
            cycles_left -= executed;
        }
    }

    if ((ticks % TPF) == 0) {
        m68k_set_irq(1);
        ScreenDraw();
    }

    done = ScreenProcessEvents();

    ticks++;
}

void exit_error(char* fmt, ...) {
    static bool guard_val = false;
    char buff[100];
    unsigned int pc;
    va_list args;

    if (guard_val)
        return;
    else
        guard_val = true;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    pc = m68k_get_reg(NULL, M68K_REG_PPC);
    m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
    fprintf(stderr, "At %04x: %s\n", pc, buff);

    exit(EXIT_FAILURE);
}
