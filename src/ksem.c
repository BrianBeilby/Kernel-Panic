/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Semaphores
 */

#include <spede/string.h>

#include "kernel.h"
#include "ksem.h"
#include "queue.h"
#include "scheduler.h"

// Table of all semephores
sem_t semaphores[SEM_MAX];

// semaphore ids to be allocated
queue_t sem_queue;

/**
 * Initializes kernel semaphore data structures
 * @return -1 on error, 0 on success
 */
int ksemaphores_init() {
    kernel_log_info("Initializing kernel semaphores");

    // Initialize the semaphore table
    memset(&semaphores, 0, sizeof(semaphores));

    for (int i = 0; i < SEM_MAX; i++) {
        semaphores[i].allocated = 0;
    }

    // Initialize the semaphore queue
    memset(&sem_queue, 0, sizeof(sem_queue));

    // Fill the semaphore queue
    for (int i = 0; i < SEM_MAX; i++) {
        if (queue_in(&sem_queue, i) != 0) {
            kernel_log_warn("Unable to Queue semaphore: %d", i);
        }
    }

    return 0;
}

/**
 * Allocates a semaphore
 * @param value - initial semaphore value
 * @return -1 on error, otherwise the semaphore id that was allocated
 */
int ksem_init(int value) {
    // Obtain a semaphore id from the semaphore queue
    int id = -1; //offset

    if (queue_out(&sem_queue, &id) != 0) {
        id++;
    }

    // Ensure that the id is within the valid range
    if(id > SEM_MAX || id < 0) {
        return -1;
    }

    // Initialize the semaphore data structure
    // sempohare table + all members (wait queue, allocated, count)
        // set count to initial value
    for (int i = 0; i < SEM_MAX; i++) {
        memset(&semaphores[i].wait_queue, 0, sizeof(semaphores[i].wait_queue));
        semaphores[i].allocated = 1;
        semaphores[i].count = 0;
    }
    return id;
}

/**
 * Frees the specified semaphore
 * @param id - the semaphore id
 * @return 0 on success, -1 on error
 */
int ksem_destroy(int id) {
    // look up the sempaphore in the semaphore table
    if(id > SEM_MAX || id < 0) {
        return -1;
    }
    sem_t *sem = &semaphores[id];

    // If the semaphore is locked, prevent it from being destroyed
    if (sem->count > 0) {
        return -1;
    }

    // Add the id back into the semaphore queue to be re-used later
    queue_in(&sem_queue, id);

    // Clear the memory for the data structure
    sem->allocated = 0;
    sem->count = 0;
    memset(&sem->wait_queue, 0, sizeof(sem->wait_queue));

    return 0;
}

/**
 * Waits on the specified semaphore if it is held
 * @param id - the semaphore id
 * @return -1 on error, otherwise the current semaphore count
 */
int ksem_wait(int id) {
    // look up the sempaphore in the semaphore table
    sem_t *sem = &semaphores[id];

    // If the semaphore count is 0, then the process must wait
        // Set the state to WAITING
        // add to the semaphore's wait queue
        // remove from the scheduler
    if (sem->count <= 0) {
        active_proc->state = WAITING;
        queue_in(&sem->wait_queue, active_proc->pid);
        scheduler_remove(active_proc);
    }

    // If the semaphore count is > 0
        // Decrement the count
    if (sem->count > 0) {
        sem->count = sem->count - 1;
    }

    // Return the current semaphore count
    return sem->count;
}

/**
 * Posts the specified semaphore
 * @param id - the semaphore id
 * @return -1 on error, otherwise the current semaphore count
 */
int ksem_post(int id) {

    // look up the semaphore in the semaphore table
    sem_t *sem = &semaphores[id];
    // incrememnt the semaphore count
    sem->count = sem->count + 1;
    // check if any processes are waiting on the semaphore (semaphore wait queue)
        // if so, queue out and add to the scheduler
        // decrement the semaphore count
    for (int i = 0; i < SEM_MAX; i++) {
        if (queue_out(&sem->wait_queue, &i)) {
            break;
        }
        else {
            scheduler_add(pid_to_proc(i));
            sem->count = sem->count - 1;
            break;
        }
    }
    // return current semaphore count
    return sem->count;
}