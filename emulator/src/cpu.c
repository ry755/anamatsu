#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "blooper.h"
#include "cpu.h"
#include "framebuffer.h"
#include "../musashi/m68k.h"

#define READ_BYTE(BASE, ADDR) (BASE)[ADDR]
#define READ_WORD(BASE, ADDR) (((BASE)[ADDR] << 8) | \
                              (BASE)[(ADDR) + 1])
#define READ_LONG(BASE, ADDR) (((BASE)[ADDR] << 24)      | \
                              ((BASE)[(ADDR) + 1] << 16) | \
                              ((BASE)[(ADDR) + 2] << 8)  | \
                              (BASE)[(ADDR) + 3])

#define WRITE_BYTE(BASE, ADDR, VAL) (BASE)[ADDR]       = (VAL) & 0xff
#define WRITE_WORD(BASE, ADDR, VAL) (BASE)[ADDR]       = ((VAL) >> 8) & 0xff; \
                                    (BASE)[(ADDR) + 1] = (VAL) & 0xff
#define WRITE_LONG(BASE, ADDR, VAL) (BASE)[ADDR]       = ((VAL) >> 24) & 0xff; \
                                    (BASE)[(ADDR) + 1] = ((VAL) >> 16) & 0xff; \
                                    (BASE)[(ADDR) + 2] = ((VAL) >> 8) & 0xff;  \
                                    (BASE)[(ADDR) + 3] = (VAL) & 0xff

unsigned char rom[ROM_SIZE];
unsigned char ram[RAM_SIZE];

unsigned int m68k_read_memory_8(unsigned int address) {
    switch (address) {
        case 0x000000 ... 0x00FFFF: // ROM
            return READ_BYTE(rom, address);
        case 0x200000 ... 0x2FFFFF: // ROM (mirror)
            return READ_BYTE(rom, address - 0x200000);
        case 0x800000: // Blooper data
            return blooper_data_read();
        case 0x800002: // Blooper command
            break;
        case 0xA0000A: // UART line status register
            return 0b00100000;
        case 0xC00000 ... 0xCFFFFF: // RAM
            return READ_BYTE(ram, address - 0xC00000);
        case 0xE00000 ... 0xEFFFFF: // RAM (mirror)
            return READ_BYTE(ram, address - 0xE00000);
        default:
            break;
    }
    return 0;
}

unsigned int m68k_read_memory_16(unsigned int address) {
    switch (address) {
        case 0x000000 ... 0x00FFFF: // ROM
            return READ_WORD(rom, address);
        case 0x200000 ... 0x2FFFFF: // ROM (mirror)
            return READ_WORD(rom, address - 0x200000);
        case 0x800000: // Blooper data
            return blooper_data_read();
        case 0x800002: // Blooper command
            break;
        case 0xA0000A: // UART line status register
            return 0b00100000;
        case 0xC00000 ... 0xCFFFFF: // RAM
            return READ_WORD(ram, address - 0xC00000);
        case 0xE00000 ... 0xEFFFFF: // RAM (mirror)
            return READ_WORD(ram, address - 0xE00000);
        default:
            break;
    }
    return 0;
}

unsigned int m68k_read_memory_32(unsigned int address) {
    switch (address) {
        case 0x000000 ... 0x00FFFF: // ROM
            return READ_LONG(rom, address);
        case 0x200000 ... 0x2FFFFF: // ROM (mirror)
            return READ_LONG(rom, address - 0x200000);
        case 0x800000: // Blooper data
            return blooper_data_read();
        case 0x800002: // Blooper command
            break;
        case 0xA0000A: // UART line status register
            return 0b00100000;
        case 0xC00000 ... 0xCFFFFF: // RAM
            return READ_LONG(ram, address - 0xC00000);
        case 0xE00000 ... 0xEFFFFF: // RAM (mirror)
            return READ_LONG(ram, address - 0xE00000);
        default:
            break;
    }
    return 0;
}

unsigned int m68k_read_disassembler_16(unsigned int address) {
    return READ_WORD(rom, address);
}

unsigned int m68k_read_disassembler_32(unsigned int address) {
    return READ_LONG(rom, address);
}

void m68k_write_memory_8(unsigned int address, unsigned int value) {
    switch (address) {
        case 0x000000 ... 0x00FFFF: // ROM
            break;
        case 0x200000 ... 0x2FFFFF: // ROM (mirror)
            break;
        case 0x800000: // Blooper data
            blooper_data_write((uint8_t)value);
            break;
        case 0x800002: // Blooper command
            blooper_command_write((uint8_t)value);
            break;
        case 0xA00000: // UART character print
            putchar((char)value);
            fflush(stdout);
            break;
        case 0xC00000 ... 0xCFFFFF: // RAM
            WRITE_BYTE(ram, address - 0xC00000, value);
            break;
        case 0xE00000 ... 0xEFFFFF: // RAM (mirror)
            WRITE_BYTE(ram, address - 0xE00000, value);
            break;
        default:
            break;
    }
}

void m68k_write_memory_16(unsigned int address, unsigned int value) {
    switch (address) {
        case 0x000000 ... 0x00FFFF: // ROM
            break;
        case 0x200000 ... 0x2FFFFF: // ROM (mirror)
            break;
        case 0x800000: // Blooper data
            blooper_data_write((uint8_t)value);
            break;
        case 0x800002: // Blooper command
            blooper_command_write((uint8_t)value);
            break;
        case 0xA00000:
            putchar((char)value);
            fflush(stdout);
            break;
        case 0xC00000 ... 0xCFFFFF: // RAM
            WRITE_WORD(ram, address - 0xC00000, value);
            break;
        case 0xE00000 ... 0xEFFFFF: // RAM (mirror)
            WRITE_WORD(ram, address - 0xE00000, value);
            break;
        default:
            break;
    }
}

void m68k_write_memory_32(unsigned int address, unsigned int value) {
    switch (address) {
        case 0x000000 ... 0x00FFFF: // ROM
            break;
        case 0x200000 ... 0x2FFFFF: // ROM (mirror)
            break;
        case 0x800000: // Blooper data
            blooper_data_write((uint8_t)value);
            break;
        case 0x800002: // Blooper command
            blooper_command_write((uint8_t)value);
            break;
        case 0xA00000:
            putchar((char)value);
            fflush(stdout);
            break;
        case 0xC00000 ... 0xCFFFFF: // RAM
            WRITE_LONG(ram, address - 0xC00000, value);
            break;
        case 0xE00000 ... 0xEFFFFF: // RAM (mirror)
            WRITE_LONG(ram, address - 0xE00000, value);
            break;
        default:
            break;
    }
}

void make_hex(char* buff, unsigned int pc, unsigned int length) {
    char* ptr = buff;

    for(; length > 0; length -= 2) {
        sprintf(ptr, "%04x", m68k_read_disassembler_16(pc));
        pc += 2;
        ptr += 4;
        if(length > 2)
            *ptr++ = ' ';
    }
}

void instr_callback(unsigned int pc) {
    static char buff[100];
    static char buff2[100];
    static unsigned int instr_size;

    pc = m68k_get_reg(NULL, M68K_REG_PC);
    instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
    make_hex(buff2, pc, instr_size);
    printf("E %03x: %-20s: %s\n", pc, buff2, buff);
    fflush(stdout);
}
