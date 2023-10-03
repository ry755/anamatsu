; main entry point

    org $0
    dc.l $00CFF000
    dc.l entry
    org $10
    dc.l illegal_opcode_handler
    org $64
    dc.l vsync_interrupt_handler

    org $400
entry:
    bsr initialize_event_queue

    ; wait for Blooper to start up
    lea waiting_for_blooper_str,a0
    bsr uart_transmit_string
    lea BLOOPER,a0
.blooper_startup_loop:
    move.b #$FF,BLOOPER_CMD(a0)
    move.b BLOOPER_DATA(a0),d0
    cmpi #'B',d0
    bne .blooper_startup_loop
    lea done_str,a0
    bsr uart_transmit_string

    ; enable all interrupts and keep supervisor mode enabled
    move.w #$2000,sr

    ; fill screen with pattern
    move.l #384-8,d1
    clr.l d2
    move.l #255,d3
.y_loop:
    move.l #512-8,d0
.x_loop:
    bsr draw_character
    subq #8,d0
    bne .x_loop
    bsr draw_character
    subq #FONT_HEIGHT,d1
    bne .y_loop
    move.l #512-8,d0
.x_loop_end:
    bsr draw_character
    subq #8,d0
    bne .x_loop_end
    bsr draw_character

    ; hello strings
    lea hello_str,a0
    moveq #16,d0
    moveq #16,d1
    clr.l d2 ; no invert, opaque background
    bsr draw_string
    lea inverted_str,a0
    moveq #16,d0
    moveq #24,d1
    moveq #1,d2 ; invert, opaque background
    bsr draw_string
    lea transparent_str,a0
    moveq #16,d0
    moveq #32,d1
    moveq #2,d2 ; no invert, transparent background
    bsr draw_string

event_loop:
    bsr get_next_event

    cmpi #EVENT_TYPE_KEY_DOWN,d0
    beq key_down_event

    bra event_loop

key_down_event:
    move.l d1,d0
    bsr scancode_to_ascii
    move.l d0,d3
    clr.l d0
    move.w x_counter,d0
    moveq #32,d1
    clr.l d2
    bsr draw_character
    addi.w #8,x_counter
    bra event_loop

hello_str: dc.b "hello world! this is my 68000 computer emulator :3", 0
inverted_str: dc.b "it can also draw with inverted colors!", 0
transparent_str: dc.b "here it is with a transparent background!", 0
    even

illegal_opcode_handler:
    lea illegal_opcode_str,a0
    bsr uart_transmit_string
.hang:
    bra .hang

illegal_opcode_str: dc.b "Invalid opcode!", 10, 0
waiting_for_blooper_str: dc.b "Waiting for Blooper... ", 0
done_str: dc.b "done", 10, 0
    even

    include "blooper.asm"
    include "event.asm"
    include "keyboard.asm"
    include "text.asm"
    include "uart.asm"
    include "vsync.asm"

FONT_HEIGHT equ 8
font:
    include "font/msx_font.inc"

    org $FFFF

data:
    include "data.asm"
