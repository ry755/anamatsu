#pragma once

#define CPU_HZ 12000000

#define ROM_SIZE 0x00010000 // 64 KiB
#define RAM_SIZE 0x00100000 // 1 MiB

void instr_callback(unsigned int pc);
