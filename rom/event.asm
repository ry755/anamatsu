; event queue routines

EVENT_SIZE equ 16

; event types
EVENT_TYPE_KEY_DOWN equ $00000000
EVENT_TYPE_KEY_UP   equ $00000001
EVENT_TYPE_EMPTY    equ $FFFFFFFF

; initialize the event queue by setting the queue pointer to the bottom of the queue
; inputs:
; none
; outputs:
; none
initialize_event_queue:
    move.l #event_queue_bottom,event_queue_ptr
    rts

; add an event to the queue
; inputs:
; d0: event type
; d1-d3: event parameters
; outputs:
; none
new_event:
    move.l a0,-(sp)

    movea.l event_queue_ptr,a0
    bsr store_event
    move.l a0,event_queue_ptr

    move.l (sp)+,a0
    rts

; get and consume the next event in the queue
; inputs:
; none
; outputs:
; d0: event type
; d1-d3: event parameters
get_next_event:
    move.l a0,-(sp)
    movea.l event_queue_ptr,a0
    cmpa.l #event_queue_bottom,a0
    beq empty_event
    move.l (sp)+,a0

    movem.l a0-a1,-(sp)

    ; push SR and mask all interrupts
    move.w sr,-(sp)
    ori #$0700,sr

    lea event_queue_bottom,a0
    bsr load_event
    lea event_temp,a0
    bsr store_event

    lea event_queue_bottom,a1
.loop:
    adda.l #EVENT_SIZE,a1

    cmpa.l event_queue_ptr,a1
    beq .end

    movea.l a1,a0
    bsr load_event

    movea.l a1,a0
    suba.l #EVENT_SIZE,a0
    bsr store_event

    bra .loop
.end:
    lea event_temp,a0
    bsr load_event

    subi.l #EVENT_SIZE,event_queue_ptr

    ; restore interrupts
    move.w (sp)+,sr

    movem.l (sp)+,a0-a1
    rts

; internal routine
load_event:
    move.l (a0)+,d0
    move.l (a0)+,d1
    move.l (a0)+,d2
    move.l (a0)+,d3
    rts

; internal routine
store_event:
    move.l d0,(a0)+
    move.l d1,(a0)+
    move.l d2,(a0)+
    move.l d3,(a0)+
    rts

; internal routine
empty_event:
    move.l (sp)+,a0
    move.l #EVENT_TYPE_EMPTY,d0
    clr.l d1
    clr.l d2
    clr.l d3
    rts
