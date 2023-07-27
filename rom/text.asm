; text drawing routines

; draw a null-terminated string to the screen
; inputs:
; a0: pointer to null-terminated string
; d0: X coordinate
; d1: Y coordinate.
; d2: inverted (0 or 1)
; outputs:
; d0: X coordinate of the end of the text
draw_string:
    movem.l d1-d3/a0,-(sp)
.print:
    clr.l d3
    move.b (a0)+,d3
    beq .done
    bsr draw_character
    addi #FONT_WIDTH,d0
    bra .print
.done:
    movem.l (sp)+,d1-d3/a0
    rts

; draw a character to the screen
; inputs:
; d0: X coordinate
; d1: Y coordinate
; d2: inverted (0 or 1)
; d3: character
; outputs:
; none
draw_character:
    movem.l d0-d7/a0,-(sp)

    ; calculate character graphics offset
    mulu #FONT_HEIGHT,d3
    lea font,a0
    adda.l d3,a0

    move.l d0,d7
    move.l d2,d3
    move.l d1,d2
    moveq #FONT_HEIGHT,d6 ; Y counter
.y_loop:
    move.l d7,d1
    moveq #FONT_WIDTH,d4 ; X counter
    move.b (a0)+,d5
.x_loop:
    tst.l d3
    bne .invert
    btst d4,d5
    beq .font_bit_clear
.font_bit_set:
    st d0
    bsr set_pixel
    bra .x_loop_end
.font_bit_clear:
    clr.l d0
    bsr set_pixel
    bra .x_loop_end
.invert:
    btst d4,d5
    bne .font_bit_clear
    bra .font_bit_set
.x_loop_end:
    addq #1,d1
    ;subq #1,d4
    ;bne .x_loop
    dbra d4,.x_loop
    addq #1,d2
    subq #1,d6
    bne .y_loop
    ;dbra d6,.y_loop

    movem.l (sp)+,d0-d7/a0
    rts
