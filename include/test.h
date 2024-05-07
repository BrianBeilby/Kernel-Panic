/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Test functions
 */
#ifndef TEST_H
#define TEST_H

#include "timer.h"
#include "kernel.h"
#include "vga.h"
#include "tty.h"
#include "kproc.h"

/**
 * Displays a "spinner" to show activity at the top-right corner of the
 * VGA output
 */
void test_spinner(void) {
    static char spin[] = { '|', '/', '-', '\\' };
    static int count = 0;

    vga_putc_at(VGA_WIDTH-1, 0, VGA_COLOR_BLACK, VGA_COLOR_GREEN,
                spin[count++ % sizeof(spin)]);
}

/**
 * Displays the number of seconds that have passed since startup
 * Obtains the number of timer ticks and converts to seconds
 */
void test_timer(void) {
    vga_set_xy(73, 0);
    vga_printf("%5d", timer_get_ticks() / 100);
}

/**
 * Displays a table with the status of all processes
 */
void test_proc_list(void) {
    char buf[VGA_WIDTH+1] = {0};
    char state = '?';
    int bg_color = VGA_COLOR_BLACK;
    int  fg_color = VGA_COLOR_LIGHT_GREY;
    int row = 1;

    if (tty_get_active() != 0) {
        return;
    }

    // Periodically clear the screen to handle processes exiting
    if ((timer_get_ticks() % 100) == 0) {
        for (int r = 1; r < VGA_HEIGHT; r++) {
            for (int c = 0; c < VGA_WIDTH; c++) {
                vga_putc_at(c, r, bg_color, fg_color, ' ');
            }
        }
    }

    snprintf(buf, VGA_WIDTH, "Entry    PID   State    Time     CPU    Name");
    vga_puts_at(0, 0, bg_color, fg_color, buf);

    for (int i = 0; i < PROC_MAX; i++) {
        snprintf(buf, VGA_WIDTH, "%*s", VGA_WIDTH, " ");

        proc_t *proc = entry_to_proc(i);

        if (!proc) {
            continue;
        }

        if (proc->state == NONE) {
            vga_puts_at(0, row, bg_color, fg_color, buf);
            continue;
        }

        fg_color = VGA_COLOR_WHITE;
        switch (proc->state) {
            case IDLE:
                state = 'I';
                break;

            case ACTIVE:
                state = 'A';
                fg_color = VGA_COLOR_GREEN;
                break;

            case SLEEPING:
                state = 'S';
                fg_color = VGA_COLOR_YELLOW;
                break;

            case WAITING:
                state = 'W';
                fg_color = VGA_COLOR_BROWN;
                break;

            default:
                state = '?';
                fg_color = VGA_COLOR_DARK_GREY;
                break;
        }

        snprintf(buf, VGA_WIDTH, "%5d  %5d  %4c  %8d  %6d    %s",
                 i, proc->pid, state, proc->run_time, proc->cpu_time, proc->name);

        vga_puts_at(0, row, bg_color, fg_color, buf);

        row++;
    }

}

/**
 * Initializes all tests
 */
void test_init(void) {
    kernel_log_info("Initializing test functions");

    // Register the spinner to update at a rate of 10 times per second
    timer_callback_register(&test_spinner, 10, -1);

    // Register the timer to update at a rate of 4 times per second
    timer_callback_register(&test_timer, 25, -1);

    // Register the process list to update at a rate of 10 times per second
    timer_callback_register(&test_proc_list, 10, -1);
}

#endif
