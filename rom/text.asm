; text drawing routines

; draw a null-terminated string to the screen
; inputs:
; a0: pointer to null-terminated string
; d0: X coordinate
; d1: Y coordinate
; d2: flags
; outputs:
; d0: X coordinate of the end of the text
draw_string:
    movem.l d1-d3/a0,-(sp)
.print:
    clr.l d3
    move.b (a0)+,d3
    beq .done
    bsr draw_character
    addi #8,d0
    bra .print
.done:
    movem.l (sp)+,d1-d3/a0
    rts
