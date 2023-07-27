; keyboard routines

; add event to the event queue if a key was pressed or released
; this should only be called by vsync_interrupt_handler
update_keyboard:
    movem.l d0-d3,-(sp)

    bsr read_keyboard
    tst.b d0
    beq .end

    ; check if this is a make or break scancode
    btst #7,d0
    bne .break_scancode

    move.l d0,d1
    move.l #EVENT_TYPE_KEY_DOWN,d0
    clr.l d2
    clr.l d3
    bsr new_event
    bra .end
.break_scancode:
    move.l d0,d1
    move.l #EVENT_TYPE_KEY_UP,d0
    clr.l d2
    clr.l d3
    bsr new_event
.end:
    movem.l (sp)+,d0-d3
    rts

; convert a "make" scancode to an ASCII character
; inputs:
; d0: "make" scancode
; outputs:
; d0: ASCII character
scancode_to_ascii:
    move.l a0,-(sp)

    btst.b #0,modifier_bitmap
    bne .shift
    btst.b #1,modifier_bitmap
    bne .caps

    lea scancode_table,a0
    adda d0,a0
    clr.l d0
    move.b (a0),d0
.end:
    move.l (sp)+,a0
    rts
.shift:
    lea scancode_table_shift,a0
    adda d0,a0
    clr.l d0
    move.b (a0),d0
    bra .end
.caps:
    lea scancode_table_caps,a0
    adda d0,a0
    clr.l d0
    move.b (a0),d0
    bra .end

; set bit 0 in the modifier bitmap
; inputs:
; none
; outputs:
; none
shift_pressed:
    bset.b #0,modifier_bitmap
    rts

; clear bit 0 in the modifier bitmap
; inputs:
; none
; outputs:
; none
shift_released:
    bclr.b #0,modifier_bitmap
    rts

; toggle bit 1 in the modifier bitmap
; inputs:
; none
; outputs:
; none
caps_pressed:
    bchg.b #1,modifier_bitmap
    rts

scancode_table:
    dc.b 0,27,"1234567890-=",8
    dc.b 9,"qwertyuiop[]",10,0
    dc.b "asdfghjkl;'`",0,"\"
    dc.b "zxcvbnm,./",0,"*",0," "
    dc.b 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dc.b "-",0,0,0,"+",0,0,0,0,0,0,0,0,0,0,0
scancode_table_shift:
    dc.b 0,27,"!@#$%^&*()_+",8
    dc.b 9,"QWERTYUIOP{}",10,0
    dc.b "ASDFGHJKL:",34,"~",0,"|"
    dc.b "ZXCVBNM<>?",0,"*",0," "
    dc.b 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dc.b "-",0,0,0,"+",0,0,0,0,0,0,0,0,0,0,0
scancode_table_caps:
    dc.b 0,27,"1234567890-=",8
    dc.b 9,"QWERTYUIOP[]",10,0
    dc.b "ASDFGHJKL;'`",0,"\"
    dc.b "ZXCVBNM,./",0,"*",0," "
    dc.b 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    dc.b "-",0,0,0,"+",0,0,0,0,0,0,0,0,0,0,0
