#!/bin/bash

set -e

mkdir -p build
rm -f build/rom.o build/rom.bin build/rom_u.bin build/rom_l.bin
~/Documents/vasm/vasmm68k_mot -Fsrec -m68000 -o build/rom.o main.asm
srec_cat build/rom.o -o build/rom.bin -binary
srec_cat build/rom.o -split 2 0 -o build/rom_u.bin -binary
srec_cat build/rom.o -split 2 1 -o build/rom_l.bin -binary
