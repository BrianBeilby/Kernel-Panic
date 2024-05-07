/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * 
 *
 * VGA Definitions
 */
#ifndef VGA_H
#define VGA_H

#include <spede/stdio.h>
#include "tty.h"

#define VGA_BASE                ((unsigned short *)(0xB8000))
#define VGA_ATTR(bg, fg)        (((bg & 0xf) << 4) | (fg & 0xf))
#define VGA_CHAR(bg, fg, c)     (((VGA_ATTR((bg & 0xf), (fg & 0xf)) << 8)) | (c))

#define VGA_WIDTH               80
#define VGA_HEIGHT              25

#define VGA_COLOR_BLACK         0x0
#define VGA_COLOR_BLUE          0x1
#define VGA_COLOR_GREEN         0x2
#define VGA_COLOR_CYAN          0x3
#define VGA_COLOR_RED           0x4
#define VGA_COLOR_MAGENTA       0x5
#define VGA_COLOR_BROWN         0x6
#define VGA_COLOR_LIGHT_GREY    0x7

#define VGA_COLOR_DARK_GREY     0x8
#define VGA_COLOR_LIGHT_BLUE    0x9
#define VGA_COLOR_LIGHT_GREEN   0xA
#define VGA_COLOR_LIGHT_CYAN    0xB
#define VGA_COLOR_LIGHT_RED     0xC
#define VGA_COLOR_LIGHT_MAGENTA 0xD
#define VGA_COLOR_YELLOW        0xE
#define VGA_COLOR_WHITE         0xF

/**
 * Prints out a formatted string to the VGA display
 * @param fmt string format
 * @param ... variable list of parameters
 */
#define vga_printf(fmt, ...) { \
    char _vga_printf_buf[VGA_WIDTH * VGA_HEIGHT] = {0}; \
    snprintf(_vga_printf_buf, sizeof(_vga_printf_buf), (fmt), ##__VA_ARGS__); \
    vga_puts(_vga_printf_buf); \
}

/**
 * Initializes the VGA driver and configuration
 *  - Clears the screen
 *  - Enables or Disables the cursor to the default state
 */
void vga_init(void);

/**
 * Clears the VGA output and resets the x/y position
 */
void vga_clear(void);

/**
 * Sets the current X/Y (column/row) position
 *
 * @param x - x position (0 to VGA_WIDTH-1)
 * @param y - y position (0 to VGA_HEIGHT-1)
 * @notes If the input parameters exceed the valid range, the position
 *        will be set to the range boundary (min or max)
 */
void vga_set_xy(int x, int y);

/**
 * Gets the current X (column) position
 * @return integer value of the column (between 0 and VGA_WIDTH-1)
 */
int vga_get_x(void);

/**
 * Gets the current Y (row) position
 * @return integer value of the row (between 0 and VGA_HEIGHT-1)
 */
int vga_get_y(void);

/**
 * Sets the background color.
 *
 * Does not modify any existing background colors, only sets it for
 * new operations.
 *
 * @param bg - background color
 */
void vga_set_bg(int bg);

/**
 * Gets the current background color
 * @return background color value
 */
int vga_get_bg(void);

/**
 * Sets the foreground/text color.
 *
 * Does not modify any existing foreground colors, only sets it for
 * new operations.
 *
 * @param color - background color
 */
void vga_set_fg(int fg);

/**
 * Gets the current foreground color
 * @return foreground color value
 */
int vga_get_fg(void);

/**
 * Prints the character on the screen.
 *
 * Does not change the x/y position, simply sets the character
 * at the current x/y position using existing background and foreground
 * colors.
 *
 * @param c - Character to print
 */
void vga_setc(char c);

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
void vga_putc(char c);

/**
 * Prints a string on the screen.
 *
 * @param s - string to print
 */
void vga_puts(char *s);

/**
 * Prints a character on the screen at the specified x/y position and
 * with the specified background/foreground colors
 *
 * @param x - x position (0 to VGA_WIDTH-1)
 * @param y - y position (0 to VGA_HEIGHT-1)
 * @param bg - background color
 * @param fg - foreground color
 * @param c - character to print
 */
void vga_putc_at(int x, int y, int bg, int fg, char c);

/**
 * Prints a string on the screen at the specified x/y position and
 * with the specified background/foreground colors
 *
 * @param x - x position (0 to VGA_WIDTH-1)
 * @param y - y position (0 to VGA_HEIGHT-1)
 * @param bg - background color
 * @param fg - foreground color
 * @param s - string to print
 */
void vga_puts_at(int x, int y, int bg, int fg, char *s);

/**
 * Enables the VGA text mode cursor
 */
void vga_cursor_enable(void);

/**
 * Disables the VGA text mode cursor
 */
void vga_cursor_disable(void);

/**
 * Refresh the VGA for the given TTY
 */
void vga_tty_refresh(tty_t *tty);

#endif
