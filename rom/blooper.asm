; blooper co-processor routines

; base address
BLOOPER equ $800000

; offsets
BLOOPER_DATA equ 0
BLOOPER_CMD  equ 2

; modes and commands
BLOOPER_MODE_SET_PIXEL_POINTER      equ $00
BLOOPER_MODE_SET_COLOR_POINTER      equ $01
BLOOPER_MODE_WRITE_PIXEL            equ $02
BLOOPER_MODE_WRITE_COLOR            equ $03
BLOOPER_CMD_DECREMENT_PIXEL_POINTER equ $04
BLOOPER_CMD_DECREMENT_COLOR_POINTER equ $05
BLOOPER_MODE_READ_KEYBOARD          equ $10

; draw a single pixel to the screen
; inputs:
; d0: pixel (0 or 1)
; d1: X coordinate
; d2: Y coordinate
; outputs:
; none
set_pixel:
    movem.l d0-d3,-(sp)

    ;mulu #64,d2 ; 512/8 pixels per line
    lsl.l #6,d2
    divu #8,d1 ; 8 bits in a byte
    move.l d1,d3
    and.l #$FFFF,d1
    add d2,d1
    clr.w d3
    swap d3

    ; d1 contains the offset of the byte to modify
    ; d3 contains the bit of the byte

    ; set the pixel pointer
    move.b #BLOOPER_MODE_SET_PIXEL_POINTER,BLOOPER+BLOOPER_CMD
    move.l d1,d2
    lsr #8,d2
    move.b d2,BLOOPER+BLOOPER_DATA
    move.b d1,BLOOPER+BLOOPER_DATA

    ; read the current pixels byte
    move.b BLOOPER+BLOOPER_DATA,d1

    tst.l d0
    beq .clear
    ; set the bit
    bset d3,d1
    bra .write
.clear:
    ; clear the bit
    bclr d3,d1
.write:
    ; decrement the pixel pointer back to where it should be, then write
    move.b #BLOOPER_CMD_DECREMENT_PIXEL_POINTER,BLOOPER+BLOOPER_CMD
    move.b d1,BLOOPER+BLOOPER_DATA

    movem.l (sp)+,d0-d3
    rts

; read a key from the keyboard
; inputs:
; none
; outputs:
; d0: keyboard scancode or zero if none
read_keyboard:
    move.b #BLOOPER_MODE_READ_KEYBOARD,BLOOPER+BLOOPER_CMD
    clr.l d0
    move.b BLOOPER+BLOOPER_DATA,d0

    rts
