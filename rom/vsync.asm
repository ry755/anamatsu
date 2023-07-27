; vsync handling routines

vsync_interrupt_handler:
    bsr update_keyboard

    rte
