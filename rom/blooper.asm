; blooper co-processor routines

; base address
BLOOPER equ $800000

; offsets
BLOOPER_DATA equ 0
BLOOPER_CMD  equ 2

; commands
BLOOPER_CMD_SET_PIXEL_POINTER       equ $00
BLOOPER_CMD_SET_COLOR_POINTER       equ $01
BLOOPER_CMD_SET_READ_WRITE_PIXEL    equ $02
BLOOPER_CMD_SET_READ_WRITE_COLOR    equ $03
BLOOPER_CMD_DECREMENT_PIXEL_POINTER equ $04
BLOOPER_CMD_DECREMENT_COLOR_POINTER equ $05
BLOOPER_CMD_READ_KEYBOARD           equ $10
BLOOPER_CMD_SET_REGION_COORDS       equ $20
BLOOPER_CMD_SET_REGION_SIZE         equ $21
BLOOPER_CMD_CLEAR_REGION            equ $22
BLOOPER_CMD_REGION_BYTE_OR          equ $23
BLOOPER_CMD_REGION_BYTE_XOR         equ $24
BLOOPER_CMD_REGION_BYTE_AND         equ $25

; draw a character to the screen
; inputs:
; d0: X coordinate
; d1: Y coordinate
; d2: flags
;     invert: bit 0
;     transparent background: bit 1
; d3: character
; outputs:
; none
draw_character:
    movem.l d0-d4/a0-a1,-(sp)

    ; calculate character graphics offset
    mulu #FONT_HEIGHT,d3
    lea font,a0
    adda.l d3,a0

    ; set blooper region
    lea BLOOPER,a1
    move.b #BLOOPER_CMD_SET_REGION_COORDS,BLOOPER_CMD(a1)
    move.l d0,d4
    lsr #8,d4
    move.b d4,BLOOPER_DATA(a1)
    move.b d0,BLOOPER_DATA(a1)
    move.l d1,d4
    lsr #8,d4
    move.b d4,BLOOPER_DATA(a1)
    move.b d1,BLOOPER_DATA(a1)
    move.b #BLOOPER_CMD_SET_REGION_SIZE,BLOOPER_CMD(a1)
    move.b #0,BLOOPER_DATA(a1)
    move.b #8,BLOOPER_DATA(a1)
    move.b #0,BLOOPER_DATA(a1)
    move.b #FONT_HEIGHT,BLOOPER_DATA(a1)

    moveq #FONT_HEIGHT,d4 ; Y counter
    btst #1,d2
    bne .transparent_background
    move.b #BLOOPER_CMD_SET_READ_WRITE_PIXEL,BLOOPER_CMD(a1)
    bra .loop
.transparent_background:
    move.b #BLOOPER_CMD_REGION_BYTE_OR,BLOOPER_CMD(a1)
.loop:
    move.b (a0)+,d0

    btst #0,d2
    beq .no_invert
    not.b d0
.no_invert:
    move.b d0,BLOOPER_DATA(a1)
    subq #1,d4
    bne .loop

    movem.l (sp)+,d0-d4/a0-a1
    rts

; set the blooper drawing region
; inputs:
; d0: X coordinate
; d1: Y coordinate
; d2: width
; d3: height
set_region:
    movem.l d4/a0,-(sp)

    lea BLOOPER,a0

    move.b #BLOOPER_CMD_SET_REGION_COORDS,BLOOPER_CMD(a0)
    move.l d0,d4
    lsr #8,d4
    move.b d4,BLOOPER_DATA(a0)
    move.b d0,BLOOPER_DATA(a0)
    move.l d1,d4
    lsr #8,d4
    move.b d4,BLOOPER_DATA(a0)
    move.b d1,BLOOPER_DATA(a0)
    move.b #BLOOPER_CMD_SET_REGION_SIZE,BLOOPER_CMD(a0)
    move.l d2,d4
    lsr #8,d4
    move.b d4,BLOOPER_DATA(a0)
    move.b d2,BLOOPER_DATA(a0)
    move.l d3,d4
    lsr #8,d4
    move.b d4,BLOOPER_DATA(a0)
    move.b d3,BLOOPER_DATA(a0)

    movem.l (sp)+,d4/a0
    rts

; draw a single pixel to the screen
; NOTE: this is extremely slow!! use this only as a last resort
; inputs:
; d0: pixel (0 or 1)
; d1: X coordinate
; d2: Y coordinate
; outputs:
; none
set_pixel:
    movem.l d0-d3/a0,-(sp)

    lea BLOOPER,a0

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
    move.b #BLOOPER_CMD_SET_PIXEL_POINTER,BLOOPER_CMD(a0)
    move.l d1,d2
    lsr #8,d2
    move.b d2,BLOOPER_DATA(a0)
    move.b d1,BLOOPER_DATA(a0)

    ; read the current pixels byte
    move.b BLOOPER_DATA(a0),d1

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
    move.b #BLOOPER_CMD_DECREMENT_PIXEL_POINTER,BLOOPER_CMD(a0)
    move.b d1,BLOOPER_DATA(a0)

    movem.l (sp)+,d0-d3/a0
    rts

; read a key from the keyboard
; inputs:
; none
; outputs:
; d0: keyboard scancode or zero if none
read_keyboard:
    move.b #BLOOPER_CMD_READ_KEYBOARD,BLOOPER+BLOOPER_CMD
    clr.l d0
    move.b BLOOPER+BLOOPER_DATA,d0

    rts
