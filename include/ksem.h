/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Semaphores
 */
#ifndef KSEM_H
#define KSEM_H

#include "kproc.h"
#include "queue.h"

// Maximum number of semaphores supported
#ifndef SEM_MAX
#define SEM_MAX 16
#endif

typedef struct sem_t {
    int allocated;          // Indicates that this semaphore has been allocated
    int count;              // The current semaphore count
    queue_t wait_queue;     // The processes waiting on the semaphore
} sem_t;

/**
 * Initializes kernel semaphore data structures
 * @return -1 on error, 0 on success
 */
int ksemaphores_init(void);

/**
 * Allocates / creates a semaphore from the kernel
 * @param value - initial semaphore value
 * @return -1 on error, otherwise the semaphore id that was allocated
 */
int ksem_init(int value);

/**
 * Destroys / releases the specified semaphore
 * @param id - the semaphore identifier
 * @return 0 on success, -1 on error
 */
int ksem_destroy(int id);

/**
 * Waits on a semaphore to be posted
 * @param id - the semaphore identifier
 * @return -1 on error, otherwise the current semaphore count
 */
int ksem_wait(int id);

/**
 * Posts the semaphore
 * @param id - the semaphore identifier
 * @return -1 on error, otherwise the current semaphore count
 */
int ksem_post(int id);
#endif
