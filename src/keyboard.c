#include <spede/flames.h>
#include <spede/stdio.h>
#include <spede/machine/io.h>

#include "interrupts.h"
#include "kernel.h"
#include "keyboard.h"
#include "kproc.h"
#include "tty.h"

// Keyboard data port
#define KBD_PORT_DATA           0x60

// Keyboard status port
#define KBD_PORT_STAT           0x64

// Keyboard status bits (CTRL, ALT, SHIFT, CAPS, NUMLOCK)
#define KEY_STATUS_CTRL         0x01
#define KEY_STATUS_ALT          0x02
#define KEY_STATUS_SHIFT        0x04
#define KEY_STATUS_CAPS         0x08
#define KEY_STATUS_NUMLOCK      0x10

// Keyboard scancode definitions
#define KEY_CTRL_L              0x1D
#define KEY_CTRL_R              0xE01D

#define KEY_ALT_L               0x38
#define KEY_ALT_R               0xE038

#define KEY_SHIFT_L             0x2A
#define KEY_SHIFT_R             0x36

#define KEY_CAPS                0x3A
#define KEY_NUMLOCK             0x45

// Macros for handling keyboard presses or releases
#define KEY_PRESSED(c)          ((c & 0x80) == 0)
#define KEY_RELEASED(c)         ((c & 0x80) != 0)

// Macros for testing key status combinations
#define KEY_STATUS_ALL(stat, test) ((stat & test) == test)
#define KEY_STATUS_ANY(stat, test) ((stat & test) != 0)

// If this sequence of keys is pressed along with another character,
// the kernel debug command function will be called
#define KEY_KERNEL_DEBUG        (KEY_STATUS_CTRL)

// Bit-map to keep track of CTRL, ALT, SHIFT, CAPS, NUMLOCK
//
// When any of these keys are pressed, the appropriate bit
// should be set. When released, the bit should be cleared.
//   CTRL, ALT, SHIFT
//
// When any of these keys are pressed and then released, the
// appropriate bits should be toggled:
//   CAPS, NUMLOCK
static unsigned int kbd_status = 0x0;
static unsigned int esc_status = 0;

// Primary keymap
static const char keyboard_map_primary[] = {
    KEY_NULL,           /* 0x00 - Null */
    KEY_ESCAPE,         /* 0x01 - Escape  */
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b',               /* 0x0e - Backspace */
    '\t',               /* 0x0f - Tab */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n',               /* 0x1e - Enter */
    KEY_NULL,           /* 0x1d - Left Ctrl */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    KEY_NULL,           /* 0x2a - Left Shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    KEY_NULL,           /* 0x36 - Right Shift */
    KEY_NULL,           /* 0x37 - Print Screen */
    KEY_NULL,           /* 0x38 - Left Alt */
    ' ',                /* 0x39 - Spacebar */
    KEY_NULL,           /* 0x3a - CapsLock */
    KEY_F1,             /* 0x3b - F1 */
    KEY_F2,             /* 0x3c - F2 */
    KEY_F3,             /* 0x3d - F3 */
    KEY_F4,             /* 0x3e - F4 */
    KEY_F5,             /* 0x3f - F5 */
    KEY_F6,             /* 0x40 - F6 */
    KEY_F7,             /* 0x41 - F7 */
    KEY_F8,             /* 0x42 - F8 */
    KEY_F9,             /* 0x43 - F9 */
    KEY_F10,            /* 0x44 - F10 */
    KEY_NULL,           /* 0x45 - NumLock */
    KEY_NULL,           /* 0x46 - ScrollLock */
    '7',                /* 0x47 - Numpad 7 */
    KEY_UP, //'8',                /* 0x48 - Numpad 8 */
    '9',                /* 0x49 - Numpad 9 */
    '-',                /* 0x4a - Numpad Minus */
    KEY_LEFT, //'4',                /* 0x4b - Numpad 4 */
    '5',                /* 0x4c - Numpad 5 */
    KEY_RIGHT, //'6',                /* 0x4d - Numpad 6 */
    '+',                /* 0x4e - Numpad Plus */
    '1',                /* 0x4f - Numpad 1 */
    KEY_DOWN, //'2',                /* 0x50 - Numpad 2 */
    '3',                /* 0x51 - Numpad 3 */
    KEY_INSERT,         /* 0x52 - Insert */
    KEY_DELETE,         /* 0x53 - Delete */
    KEY_NULL,           /* 0x54 */
    KEY_NULL,           /* 0x55 */
    KEY_NULL,           /* 0x56 */
    KEY_F11,            /* 0x57 - F11 */
    KEY_F12,            /* 0x58 - F12 */
    KEY_NULL,           /* 0x59 */
    KEY_NULL,           /* 0x5a */
    KEY_NULL,           /* 0x5b */
    KEY_NULL,           /* 0x5c */
    KEY_NULL,           /* 0x5d */
    KEY_NULL,           /* 0x5e */
    KEY_NULL,           /* 0x5f */
    KEY_NULL,           /* 0x60 */
    KEY_NULL,           /* 0x61 */
    KEY_NULL,           /* 0x62 */
    KEY_NULL,           /* 0x63 */
    KEY_NULL,           /* 0x64 */
    KEY_NULL,           /* 0x65 */
    KEY_NULL,           /* 0x66 */
    KEY_NULL,           /* 0x67 */
    KEY_NULL,           /* 0x68 */
    KEY_NULL,           /* 0x69 */
    KEY_NULL,           /* 0x6a */
    KEY_NULL,           /* 0x6b */
    KEY_NULL,           /* 0x6c */
    KEY_NULL,           /* 0x6d */
    KEY_NULL,           /* 0x6e */
    KEY_NULL,           /* 0x6f */
    KEY_NULL,           /* 0x70 */
    KEY_NULL,           /* 0x71 */
    KEY_NULL,           /* 0x72 */
    KEY_NULL,           /* 0x73 */
    KEY_NULL,           /* 0x74 */
    KEY_NULL,           /* 0x75 */
    KEY_NULL,           /* 0x76 */
    KEY_NULL,           /* 0x77 */
    KEY_NULL,           /* 0x78 */
    KEY_NULL,           /* 0x79 */
    KEY_NULL,           /* 0x7a */
    KEY_NULL,           /* 0x7b */
    KEY_NULL,           /* 0x7c */
    KEY_NULL,           /* 0x7d */
    KEY_NULL,           /* 0x7e */
    KEY_NULL            /* 0x7f */
};

// Secondary keymap (when CAPS ^ SHIFT is enabled)
static const char keyboard_map_secondary[] = {
    KEY_NULL,           /* 0x00 - Undefined */
    KEY_ESCAPE,         /* 0x01 - Escape */
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    '\b',               /* 0x0e - Backspace */
    '\t',               /* 0x0f - Tab */
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    '\n',               /* 0x1e - Enter */
    KEY_NULL,           /* 0x1d - Left Ctrl */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    KEY_NULL,           /* 0x2a - Left Shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    KEY_NULL,           /* 0x36 - Right Shift */
    KEY_NULL,           /* 0x37 - Print Screen */
    KEY_NULL,           /* 0x38 - Left Alt */
    ' ',
    KEY_NULL,           /* 0x3a - CapsLock */
    KEY_F1,             /* 0x3b - F1 */
    KEY_F2,             /* 0x3c - F2 */
    KEY_F3,             /* 0x3d - F3 */
    KEY_F4,             /* 0x3e - F4 */
    KEY_F5,             /* 0x3f - F5 */
    KEY_F6,             /* 0x40 - F6 */
    KEY_F7,             /* 0x41 - F7 */
    KEY_F8,             /* 0x42 - F8 */
    KEY_F9,             /* 0x43 - F9 */
    KEY_F10,            /* 0x44 - F10 */
    KEY_NULL,           /* 0x45 - NumLock */
    KEY_NULL,           /* 0x46 - ScrollLock */
    KEY_HOME,           /* 0x47 - Home */
    KEY_UP,             /* 0x48 - Up Arrow */
    KEY_PAGE_UP,        /* 0x49 - Page Up */
    '-',                /* 0x4a - Numpad minus */
    KEY_LEFT,           /* 0x4b - Left Arrow */
    KEY_NULL,           /* 0x4c - Numpad Center */
    KEY_RIGHT,          /* 0x4d - Right Arrow */
    '+',                /* 0x4e - Numpad plus */
    KEY_END,            /* 0x4f - Page End */
    KEY_DOWN,           /* 0x50 - Down Arrow */
    KEY_PAGE_DOWN,      /* 0x51 - Page Down */
    KEY_INSERT,         /* 0x52 - Insert */
    KEY_DELETE,         /* 0x53 - Delete */
    KEY_NULL,           /* 0x54 */
    KEY_NULL,           /* 0x55 */
    KEY_NULL,           /* 0x56 */
    KEY_F11,            /* 0x57 - F11 */
    KEY_F12,            /* 0x58 - F12 */
    KEY_NULL,           /* 0x59 */
    KEY_NULL,           /* 0x5a */
    KEY_NULL,           /* 0x5b */
    KEY_NULL,           /* 0x5c */
    KEY_NULL,           /* 0x5d */
    KEY_NULL,           /* 0x5e */
    KEY_NULL,           /* 0x5f */
    KEY_NULL,           /* 0x60 */
    KEY_NULL,           /* 0x61 */
    KEY_NULL,           /* 0x62 */
    KEY_NULL,           /* 0x63 */
    KEY_NULL,           /* 0x64 */
    KEY_NULL,           /* 0x65 */
    KEY_NULL,           /* 0x66 */
    KEY_NULL,           /* 0x67 */
    KEY_NULL,           /* 0x68 */
    KEY_NULL,           /* 0x69 */
    KEY_NULL,           /* 0x6a */
    KEY_NULL,           /* 0x6b */
    KEY_NULL,           /* 0x6c */
    KEY_NULL,           /* 0x6d */
    KEY_NULL,           /* 0x6e */
    KEY_NULL,           /* 0x6f */
    KEY_NULL,           /* 0x70 */
    KEY_NULL,           /* 0x71 */
    KEY_NULL,           /* 0x72 */
    KEY_NULL,           /* 0x73 */
    KEY_NULL,           /* 0x74 */
    KEY_NULL,           /* 0x75 */
    KEY_NULL,           /* 0x76 */
    KEY_NULL,           /* 0x77 */
    KEY_NULL,           /* 0x78 */
    KEY_NULL,           /* 0x79 */
    KEY_NULL,           /* 0x7a */
    KEY_NULL,           /* 0x7b */
    KEY_NULL,           /* 0x7c */
    KEY_NULL,           /* 0x7d */
    KEY_NULL,           /* 0x7e */
    KEY_NULL            /* 0x7f */
};


/*
 *
 */
void keyboard_irq_handler(void) {
    unsigned int c = keyboard_poll();

    if (c) {
        tty_input(c);
    }
}


/**
 * Initializes keyboard data structures and variables
 */
void keyboard_init() {
    kernel_log_info("Initializing keyboard");

    // No status keys pressed by default
    kbd_status = 0x0;

    // Register the keyboard ISR
    interrupts_irq_register(IRQ_KEYBOARD, isr_entry_keyboard, keyboard_irq_handler);
}

/**
 * Scans for keyboard input and returns the raw character data
 * @return raw character data from the keyboard
 */
unsigned int keyboard_scan(void) {
    unsigned int c;
    c = inportb(KBD_PORT_DATA);
//    kernel_log_trace("keyboard: raw data [0x%02x]", c);
    return c;
}

/**
 * Polls for a keyboard character to be entered.
 *
 * If a keyboard character data is present, will scan and return
 * the decoded keyboard output.
 *
 * @return decoded character or KEY_NULL (0) for any character
 *         that cannot be decoded
 */
unsigned int keyboard_poll(void) {
    unsigned int c = KEY_NULL;

    if ((inportb(KBD_PORT_STAT) & 0x1) != 0) {
        c = keyboard_scan();
        c = keyboard_decode(c);
    }

    return c;
}

/**
 * Blocks until a keyboard character has been entered
 * @return decoded character entered by the keyboard or KEY_NULL
 *         for any character that cannot be decoded
 */
unsigned int keyboard_getc(void) {
    unsigned int c = KEY_NULL;
    while ((c = keyboard_poll()) == KEY_NULL);
    return c;
}

/**
 * Processes raw keyboard input and decodes it.
 *
 * Should keep track of the keyboard status for the following keys:
 *   SHIFT, CTRL, ALT, CAPS, NUMLOCK
 *
 * For all other characters, they should be decoded/mapped to ASCII
 * or ASCII-friendly characters.
 *
 * For any character that cannot be mapped, KEY_NULL should be returned.
 *
 * If *all* of the status keys defined in KEY_KERNEL_DEBUG are pressed,
 * while another character is entered, the kernel_debug_command()
 * function should be called.
 */
unsigned int keyboard_decode(unsigned int c) {
    unsigned int key_pressed = KEY_PRESSED(c);

    switch (c & ~0x80) {
        case KEY_CTRL_L:
        case KEY_CTRL_R:
            if (key_pressed) {
                if ((kbd_status & KEY_STATUS_CTRL) == 0) {
//                    kernel_log_trace("keyboard: CTRL pressed");
                }

                kbd_status |= KEY_STATUS_CTRL;
            } else {
                if ((kbd_status & KEY_STATUS_CTRL) != 0) {
//                    kernel_log_trace("keyboard: CTRL released");
                }

                kbd_status &= ~KEY_STATUS_CTRL;
            }
            break;

        case KEY_ALT_L:
        case KEY_ALT_R:
            if (key_pressed) {
                if ((kbd_status & KEY_STATUS_ALT) == 0) {
//                    kernel_log_trace("keyboard: ALT pressed");
                }

                kbd_status |= KEY_STATUS_ALT;
            } else {
                if ((kbd_status & KEY_STATUS_ALT) != 0) {
//                    kernel_log_trace("keyboard: ALT released");
                }

                kbd_status &= ~KEY_STATUS_ALT;
            }
            break;

        case KEY_SHIFT_L:
        case KEY_SHIFT_R:
            if (key_pressed) {
                if ((kbd_status & KEY_STATUS_SHIFT) == 0) {
//                    kernel_log_trace("keyboard: SHIFT pressed");
                }

                kbd_status |= KEY_STATUS_SHIFT;
            } else {
                if ((kbd_status & KEY_STATUS_SHIFT) != 0) {
//                    kernel_log_trace("keyboard: SHIFT released");
                }

                kbd_status &= ~KEY_STATUS_SHIFT;
            }
            break;

        case KEY_CAPS:
            if (key_pressed) {
//                kernel_log_trace("keyboard: CAPS pressed");
                kbd_status ^= KEY_STATUS_CAPS;
            } else {
//                kernel_log_trace("keyboard: CAPS released");
            }
            break;

        case KEY_NUMLOCK:
            if (key_pressed) {
                kbd_status ^= KEY_STATUS_NUMLOCK;
            }
            break;

        // Handle all other input
        default:
            // Ignore presses, only process releases
            if (!key_pressed) {
                break;
            }


            // Choose which map to use based upon the keyboard status
            if (c >= 0x47 && c <= 0x53) {
                if ((kbd_status & KEY_STATUS_NUMLOCK) != 0) {
                    c = keyboard_map_secondary[c];
                } else {
                    c = keyboard_map_primary[c];
                }
            } else if ((((kbd_status & KEY_STATUS_SHIFT) != 0)
               ^ ((kbd_status & KEY_STATUS_CAPS)  != 0)) != 0) {
                c = keyboard_map_secondary[c];
            } else {
                c = keyboard_map_primary[c];
            }

            if (kbd_status & KEY_STATUS_ALT) {
                if (c >= '0' && c <= '9') {
                    int n = c - '0';
                    tty_select(n);
                    return KEY_NULL;
                }
            }

            if (c == KEY_ESCAPE) {
                esc_status++;

                if (esc_status == 3) {
                    kernel_exit();
                }

                return KEY_NULL;
            } else if (c != KEY_NULL) {
                esc_status = 0;
            }

            if (kbd_status & KEY_STATUS_CTRL) {
                if (c == '+' || c == '=') {
                    kernel_set_log_level(kernel_get_log_level() + 1);
                    return KEY_NULL;
                } else if (c == '-' || c == '_') {
                    kernel_set_log_level(kernel_get_log_level() - 1);
                    return KEY_NULL;
                }

                if (c == 'n' || c == 'N') {
                    kproc_create(kproc_test, "test", PROC_TYPE_USER);
                    return KEY_NULL;
                }

                if (c == 'q' || c == 'Q') {
                    kproc_destroy(active_proc);
                    return KEY_NULL;
                }

                if (c == 'b' || c == 'B') {
                    breakpoint();
                    return KEY_NULL;
                }
            }

            if (c) {
                // For now, print to the host console
                if (c >= 0x20 && c <= 0x7f) {
//                    kernel_log_trace("keyboard: character [0x%02x] '%c'", c, c);
                } else {
//                    kernel_log_trace("keyboard: character [0x%02x]", c);
                }
                return c;
            }

            break;
    }

    return KEY_NULL;
}
