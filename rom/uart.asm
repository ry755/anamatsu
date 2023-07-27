; UART routines

; base address
UART equ $A00000

; register offsets
UART_RHR equ 0  ; receive holding register (read)
UART_THR equ 0  ; transmit holding register (write)
UART_IER equ 2  ; interrupt enable register
UART_ISR equ 4  ; interrupt status register (read)
UART_FCR equ 4  ; FIFO control register (write)
UART_LCR equ 6  ; line control register
UART_MCR equ 8  ; modem control register
UART_LSR equ 10 ; line status register
UART_MSR equ 12 ; modem status register
UART_SPR equ 14 ; scratchpad register (reserved for system use)
UART_DLL equ 0  ; divisor latch LSB
UART_DLM equ 2  ; divisor latch MSB

; MCR register bits (output lines)
; a bit value of 1 drives the output low
UART_MCR_LED equ 1 ; ~RTS

; transmit a null-terminated string over the UART
; inputs:
; a0: pointer to null-terminated string
; outputs:
; none
uart_transmit_string:
    movem.l d0/a0,-(sp)
.print:
    move.b (a0)+,d0
    beq .done
    bsr uart_transmit_character
    bra .print
.done:
    movem.l (sp)+,d0/a0
    rts

; transmit a single character over the UART
; inputs:
; d0: character
; outputs:
; none
uart_transmit_character:
    ; wait for the UART to be ready
.loop:
    btst.b #5,UART+UART_LSR
    beq .loop

    ; transmit
    move.b d0,UART+UART_THR

    rts

; transmit a 32 bit integer as ASCII hexadecimal over the UART
; inputs:
; d0: integer
; outputs:
; none
uart_transmit_hex:
    movem.l d0-d2,-(sp)

    moveq #7,d1
.loop:
    rol.l #4,d0
    move.w d0,d2
    and.w #%1111,d2
    move.b (.digits,pc,d2),d2
.uart_loop:
    btst.b #5,UART+UART_LSR
    beq .uart_loop
    move.b d2,UART+UART_THR
    dbra d1,.loop

    movem.l (sp)+,d0-d2
    rts
.digits: dc.b "0123456789ABCDEF"

; turn the status LED on
; inputs:
; none
; outputs:
; none
turn_led_on:
    bset.b  #UART_MCR_LED,UART+UART_MCR
    rts

; turn the status LED off
; inputs:
; none
; outputs:
; none
turn_led_off:
    bclr.b  #UART_MCR_LED,UART+UART_MCR
    rts

; toggle the status LED
; inputs:
; none
; outputs:
; none
toggle_led:
    bchg.b  #UART_MCR_LED,UART+UART_MCR
    rts
