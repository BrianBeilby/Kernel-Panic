/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * TTY Definitions
 */
#ifndef TTY_H
#define TTY_H

#include "kproc.h"

#ifndef TTY_MAX
#define TTY_MAX         10  // Maximum number of TTYs to support
#endif

#ifndef TTY_SCROLLBACK
#define TTY_SCROLLBACK  0   // Number of lines in the scrollback buffer
#endif

#define TTY_WIDTH       80  // Width of the TTY
#define TTY_HEIGHT      25  // Height of the TTY

#define TTY_BUF_SIZE (TTY_WIDTH * (TTY_HEIGHT + TTY_SCROLLBACK))


// TTY data structure
// Describes the virtual TTY
typedef struct tty_t {
    int id;                     // Numerical tty identifier
    char buf[TTY_BUF_SIZE];     // Screen buffer + scrollback

    int refresh;                // TTY needs to refresh

    /* Additional options where supported */
    int color_bg;               // Background Color
    int color_fg;               // Foreground Color

    int pos_x;                  // current x position in the screen
    int pos_y;                  // current y position in the screen

    int pos_scroll;             // Current scrollback position in the buffer

    int echo;                   // If the TTY should echo or not

    ringbuf_t io_input;         // Input buffer
    ringbuf_t io_output;        // Output buffer
} tty_t;

/**
 * Initializes all TTY data structures and memory
 * Selects TTY 0 to be the default
 */
void tty_init(void);

/**
 * Sets the active TTY to the selected TTY number
 * @param tty - TTY number
 */
void tty_select(int tty);

/**
 * Returns the active TTY id
 * @return TTY id or -1 on error
 */
int tty_get_active(void);


/**
 * Returns the tty structure for the given tty number
 * @param tty_number - tty number/identifier
 * @return NULL on error or pointer to entry in the tty table
 */
struct tty_t *tty_get(int tty);

/**
 * Write a character into the TTY process input buffer
 * If the echo flag is set, will also write the character into the TTY
 * process output buffer
 * @param c - character to write into the input buffer
 */
void tty_input(char c);

/**
 * Updates the TTY with the given character
 * @param c - character to update on the TTY screen output
 */
void tty_update(char c);

/**
 * Scrolls the TTY up one line into the scrollback buffer
 * If the buffer is at the top, it will not scroll up further
 */
void tty_scroll_up(void);

/**
 * Scrolls the TTY down one line into the scrollback buffer
 * If the buffer is at the end, it will not scroll down further
 */
void tty_scroll_down(void);

/**
 * Scrolls to the top of the buffer
 */
void tty_scroll_top(void);

/**
 * Scrolls to the bottom of the buffer
 */
void tty_scroll_bottom(void);

#endif
