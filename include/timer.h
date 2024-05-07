/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Timer Definitions
 */
#ifndef TIMER_H
#define TIMER_H

#ifndef TIMERS_MAX
#define TIMERS_MAX 32
#endif

/**
 * Registers a new callback to be called at the specified interval
 * @param func_ptr - function pointer to be called
 * @param interval - number of ticks before the callback is performed
 * @param repeat   - Indicate how many intervals to repeat (-1 should repeat forever)
 *
 * @return the allocated timer id or -1 for errors
 */
int timer_callback_register(void (*func_ptr)(), int interval, int repeat);

/**
 * Unregisters the specified callback
 * @param id
 *
 * @return 0 on success, -1 on error
 */
int timer_callback_unregister(int id);

/**
 * Returns the number of ticks that have occurred since startup
 *
 * @return timer_ticks
 */
int timer_get_ticks(void);

/**
 * Initializes timer related data structures and variables
 */
void timer_init(void);

#endif
