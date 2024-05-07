#include <spede/machine/io.h>
#include <spede/stdarg.h>
#include <spede/stdio.h>

#include "kernel.h"
#include "tty.h"
#include "vga.h"

// VGA Address Port -> Set the register address to write data into
#define VGA_PORT_ADDR 0x3D4
// VGA Data Port -> The data to be written into the register
#define VGA_PORT_DATA 0x3D5


// Current x position (column)
int vga_pos_x = 0;

// Current y position (row)
int vga_pos_y = 0;

// Current background color, default to black
int vga_color_bg = VGA_COLOR_BLACK;

// Current foreground color, default to light grey
int vga_color_fg = VGA_COLOR_LIGHT_GREY;

// VGA text mode cursor status
int vga_cursor = 0;

// Optionally enable/disable scrolling
int vga_scroll = 0;

/**
 * Initializes the VGA driver and configuration
 *  - Defaults variables
 *  - Clears the screen
 */
void vga_init(void) {
    kernel_log_info("Initializing VGA driver");

    if (vga_cursor) {
        // Enable the cursor
        vga_cursor_enable();
    } else {
        // Disable the cursor
        vga_cursor_disable();
    }

    // Clear the screen
    vga_clear();
}

/**
 * Sets the cursor position to the current VGA row/column (x/y)
 * position if the cursor is enabled.
 */
void vga_cursor_update(void) {
    if (vga_cursor) {
        unsigned short pos = vga_pos_x + vga_pos_y * VGA_WIDTH;

        outportb(VGA_PORT_ADDR, 0x0F);
        outportb(VGA_PORT_DATA, (unsigned char) (pos & 0xFF));
        outportb(VGA_PORT_ADDR, 0x0E);
        outportb(VGA_PORT_DATA, (unsigned char) ((pos >> 8) & 0xFF));
    }
}

/**
 * Clears the VGA output and sets the background and foreground colors
 */
void vga_clear(void) {
    unsigned short *vga_buf = VGA_BASE;

    for (unsigned int i = 0; i < (VGA_WIDTH * VGA_HEIGHT); i++) {
        vga_buf[i] = VGA_CHAR(vga_color_bg, vga_color_fg, 0x00);
    }

    vga_set_xy(0, 0);
}

/**
 * Sets the current X/Y (column/row) position
 *
 * @param x - x position (0 to VGA_WIDTH-1)
 * @param y - y position (0 to VGA_HEIGHT-1)
 * @notes If the input parameters exceed the valid range, the position
 *        will be set to the range boundary (min or max)
 */
void vga_set_xy(int x, int y) {
    if (x < 0) {
        vga_pos_x = 0;
    } else if (x >= VGA_WIDTH) {
        vga_pos_x = VGA_WIDTH - 1;
    } else {
        vga_pos_x = x;
    }

    if (y < 0) {
        vga_pos_y = 0;
    } else if (y >= VGA_HEIGHT) {
        vga_pos_y = VGA_HEIGHT- 1;
    } else {
        vga_pos_y = y;
    }

    vga_cursor_update();
}

/**
 * Gets the current X (column) position
 * @return integer value of the column (between 0 and VGA_WIDTH-1)
 */
int vga_get_x(void) {
    return vga_pos_x;
}

/**
 * Gets the current Y (row) position
 * @return integer value of the row (between 0 and VGA_HEIGHT-1)
 */
int vga_get_y(void) {
    return vga_pos_y;
}

/**
 * Sets the background color.
 *
 * Does not modify any existing background colors, only sets it for
 * new operations.
 *
 * @param bg - background color
 */
void vga_set_bg(int bg) {
    if (bg >= 0 && bg <= 0xF) {
        vga_color_bg = bg;
    }
}

int vga_get_bg(void) {
    return vga_color_bg;
}

/**
 * Sets the foreground/text color.
 *
 * Does not modify any existing foreground colors, only sets it for
 * new operations.
 *
 * @param color - background color
 */
void vga_set_fg(int fg) {
    if (fg >= 0 && fg <= 0xF) {
        vga_color_fg = fg;
    }
}

int vga_get_fg(void) {
    return vga_color_fg;
}

/**
 * Prints the character on the screen.
 *
 * Does not change the x/y position, simply sets the character
 * at the current x/y position using existing background and foreground
 * colors.
 *
 * @param c - Character to print
 */
void vga_setc(char c) {
    unsigned short *vga_buf = VGA_BASE;
    vga_buf[vga_pos_x + vga_pos_y * VGA_WIDTH] = VGA_CHAR(vga_color_bg, vga_color_fg, c);
}

/**
 * Prints a character on the screen.
 *
 * When a character is printed, will do the following:
 *  - Update the x and y positions
 *  - If needed, will wrap from the end of the current line to the
 *    start of the next line
 *  - If the last line is reached, will ensure that all text is
 *    scrolled up
 *  - Special characters are handled as such:
 *    - tab character (\t) prints 'tab_stop' spaces
 *    - backspace (\b) character moves the character back one position,
 *      prints a space, and then moves back one position again
 *
 * @param c - character to print
 */
void vga_putc(char c) {
    unsigned short *vga_buf = VGA_BASE;

    // Handle scecial characters
    switch (c) {
        case '\b':
            if (vga_pos_x != 0) {
                vga_pos_x--;
            } else if (vga_pos_y != 0) {
                vga_pos_y--;
                vga_pos_x = VGA_WIDTH-1;
            }

            vga_buf[vga_pos_x + vga_pos_y * VGA_WIDTH] = VGA_CHAR(vga_color_bg, vga_color_fg, 0x00);
            break;

        case '\t':
            vga_pos_x += 4 - vga_pos_x % 4;
            break;

        case '\r':
            vga_pos_x = 0;
            break;

        case '\n':
            vga_pos_x = 0;
            vga_pos_y++;
            break;

        default:
            vga_buf[vga_pos_x + vga_pos_y * VGA_WIDTH] = VGA_CHAR(vga_color_bg, vga_color_fg, c);
            vga_pos_x++;
            break;
    }

    // Handle end of lines
    if (vga_pos_x >= VGA_WIDTH) {
        vga_pos_x = 0;
        vga_pos_y++;
    }

    if (vga_scroll) {
        // Handle end of rows
        if (vga_pos_y >= VGA_HEIGHT) {
            // Scroll the screen up (copy each row to the previous)
            for (unsigned int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
                vga_buf[i] = vga_buf[VGA_WIDTH + i];
            }

            // Clear the last line
            for (unsigned int i = 0; i < VGA_WIDTH; i++) {
                vga_buf[i + (VGA_WIDTH * (VGA_HEIGHT-1))] = VGA_CHAR(vga_color_bg, vga_color_fg, ' ');
            }

            vga_pos_y = VGA_HEIGHT - 1;
        }

    }

    vga_cursor_update();
}

/**
 * Prints a string on the screen.
 *
 * @param s - string to print
 */
void vga_puts(char *s) {
    /* Handle a null pointer */
    if (s == NULL) {
        return;
    }

    while (*s != '\0') {
        vga_putc(*s);
        s++;
    }
}

/**
 * Prints a character on the screen at the specified x/y position and
 * with the specified background/foreground colors
 *
 * Does not change the "current" x/y position
 * Does not change the "current" background/foreground colors
 *
 * @param x - x position (0 to VGA_WIDTH-1)
 * @param y - y position (0 to VGA_HEIGHT-1)
 * @param bg - background color
 * @param fg - foreground color
 * @param c - character to print
 */
void vga_putc_at(int x, int y, int bg, int fg, char c) {
    int cur_x = vga_pos_x;
    int cur_y = vga_pos_y;
    int cur_bg = vga_color_bg;
    int cur_fg = vga_color_fg;
    int cur_cursor = vga_cursor;

    if (x < 0) {
        x = 0;
    } else if (x >= VGA_WIDTH) {
        x = VGA_WIDTH - 1;
    } else {
        x = x;
    }

    if (y < 0) {
        y = 0;
    } else if (y >= VGA_HEIGHT) {
        y = VGA_HEIGHT - 1;
    } else {
        y = y;
    }

    vga_pos_x = x;
    vga_pos_y = y;
    vga_color_bg = bg & 0x7;
    vga_color_fg = fg & 0xf;
    vga_cursor = 0;

    vga_putc(c);

    vga_pos_x = cur_x;
    vga_pos_y = cur_y;
    vga_color_bg = cur_bg;
    vga_color_fg = cur_fg;
    vga_cursor = cur_cursor;
}

/**
 * Prints a string on the screen at the specified x/y position and
 * with the specified background/foreground colors
 *
 * Does not change the "current" x/y position or background/foreground colors
 *
 * @param x - x position (0 to VGA_WIDTH-1)
 * @param y - y position (0 to VGA_HEIGHT-1)
 * @param bg - background color
 * @param fg - foreground color
 * @param c - character to print
 */
void vga_puts_at(int x, int y, int bg, int fg, char *s) {
    int cur_x = vga_pos_x;
    int cur_y = vga_pos_y;
    int cur_bg = vga_color_bg;
    int cur_fg = vga_color_fg;
    int cur_cursor = 0;

    if (x < 0) {
        x = 0;
    } else if (x >= VGA_WIDTH) {
        x = VGA_WIDTH - 1;
    } else {
        x = x;
    }

    if (y < 0) {
        y = 0;
    } else if (y >= VGA_HEIGHT) {
        y = VGA_HEIGHT- 1;
    } else {
        y = y;
    }

    vga_pos_x = x;
    vga_pos_y = y;
    vga_color_bg = bg & 0x7;
    vga_color_fg = fg & 0xf;
    vga_cursor = 0;

    while (*s != '\0') {
        vga_putc(*s);
        s++;
    }

    vga_pos_x = cur_x;
    vga_pos_y = cur_y;
    vga_color_bg = cur_bg;
    vga_color_fg = cur_fg;
    vga_cursor = cur_cursor;
}

/**
 * Enables the VGA text mode cursor
 */
void vga_cursor_enable(void) {
    vga_cursor = 1;

    // The cursor will be drawn between the scanlines defined
    // in the "Cursor Start Register" (0x0A) and the
    // "Cursor End Register" (0x0B)

    // To ensure that we do not change bits we are not intending,
    // read the current register state and mask off the bits we
    // want to save

    // Set the cursor starting scanline
    outportb(VGA_PORT_ADDR, 0x0A);
    outportb(VGA_PORT_DATA, (inportb(VGA_PORT_DATA) & 0xC0) | 0xE);

    // Set the cursor ending scanline
    // Ensure that bit 5 is not set so the cursor will be enabled
    outportb(VGA_PORT_ADDR, 0x0B);
    outportb(VGA_PORT_DATA, (inportb(VGA_PORT_DATA) & 0xE0) | 0xF);
}

/**
 * Disables the VGA text mode cursor
 */
void vga_cursor_disable(void) {
    vga_cursor = 0;

    // The cursor can be disabled by setting bit 5 in the "Cursor Start Register" (0xA)
    outportb(VGA_PORT_ADDR, 0x0A);
    outportb(VGA_PORT_DATA, 0x20);
}

