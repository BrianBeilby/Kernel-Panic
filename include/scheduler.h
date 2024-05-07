/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Process Scheduler
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "kproc.h"

#ifndef SCHEDULER_TIMESLICE
#define SCHEDULER_TIMESLICE 10
#endif


/**
 * Initializes the scheduler, data structures, etc.
 */
void scheduler_init(void);

/**
 * Executes the scheduler
 * Should ensure that `active_proc` is set to a valid process entry
 */
void scheduler_run(void);

/**
 * Adds a process to the scheduler
 * @param proc - pointer to the process entry
 */
void scheduler_add(proc_t *proc);

/**
 * Removes a process from the scheduler
 * @param proc - pointer to the process entry
 */
void scheduler_remove(proc_t *proc);

/**
 * Puts a process to sleep
 * @param proc - pointer to the process entry
 * @param seconds - number of seconds to sleep
 */
void scheduler_sleep(proc_t *proc, int seconds);

#endif
