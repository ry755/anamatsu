; memory reserved for the system

; base addresses
ROM equ $00000000
RAM equ $00C00000
ROM_DATA_START equ RAM
    rsset ROM_DATA_START

; main.asm
x_counter: rs.w 1

; event.asm
event_queue_bottom: rs.b EVENT_SIZE*16
event_queue_ptr: rs.l 1
event_temp: rs.b EVENT_SIZE

; keyboard.asm
modifier_bitmap: rs.b 1
