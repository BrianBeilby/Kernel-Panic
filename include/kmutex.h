/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Mutexes
 */
#ifndef KMUTEX_H
#define KMUTEX_H

#include "kproc.h"
#include "queue.h"

// Maximum number of mutexes supported
#ifndef MUTEX_MAX
#define MUTEX_MAX 16
#endif

typedef struct mutex_t {
    int allocated;          // Indicates that this mutex has been allocated
    int locks;              // The current number of locks held
    proc_t *owner;          // The process that currently holds the mutex
    queue_t wait_queue;     // The processes waiting on the mutex
} mutex_t;

/**
 * Initializes kernel mutex data structures
 * @return -1 on error, 0 on success
 */
int kmutexes_init(void);

/**
 * Allocates/Creates a mutex
 * @return -1 on error, otherwise the mutex id that was allocated
 */
int kmutex_init(void);

/**
 * Frees the specified mutex
 * @param id - the mutex id
 * @return 0 on success, -1 on error
 */
int kmutex_destroy(int id);

/**
 * Locks the specified mutex
 * @param id - the mutex id
 * @return -1 on error, otherwise the current lock count
 */
int kmutex_lock(int id);

/**
 * Unlocks the specified mutex
 * @param id - the mutex id
 * @return -1 on error, otherwise the current lock count
 */
int kmutex_unlock(int id);
#endif
