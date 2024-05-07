/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Process Handling
 */
#ifndef KPROC_H
#define KPROC_H

#include "trapframe.h"
#include "ringbuf.h"
#include "queue.h"

#ifndef PROC_MAX
#define PROC_MAX        20   // maximum number of processes to support
#endif

#define PROC_IO_MAX     4    // Maximum process I/O buffers

#define PROC_NAME_LEN   32   // Maximum length of a process name
#define PROC_STACK_SIZE 8192 // Process stack size

// Process types
typedef enum proc_type_t {
    PROC_TYPE_NONE,     // Undefined/none
    PROC_TYPE_KERNEL,   // Kernel process
    PROC_TYPE_USER      // User process
} proc_type_t;


// Process States
typedef enum state_t {
    NONE,               // Process has no state (doesn't exist)
    IDLE,               // Process is idle (not scheduled)
    ACTIVE,             // Process is active (scheduled)
    SLEEPING,           // Process is sleeping (not scheduled)
    WAITING,            // Process is waiting (not scheduled)
} state_t;


// Process control block
// Contains all details to describe a process
typedef struct proc_t {
    int pid;                        // Process id
    state_t state;                  // Process state
    proc_type_t type;               // Process type (kernel or user)

    char name[PROC_NAME_LEN];       // Process name

    int start_time;                 // Time started
    int run_time;                   // Total run time of the process
    int cpu_time;                   // Current CPU time the process has used
    int sleep_time;                 // Time that a process should be sleeping

    queue_t *scheduler_queue;       // Pointer to the queue where the process resides

    ringbuf_t *io[PROC_IO_MAX];     // Process input/output buffers

    unsigned char *stack;           // Pointer to the process stack
    trapframe_t *trapframe;         // Pointer to the trapframe
} proc_t;


/**
 * Process functions
 */

/**
 * Initializes all process related data structures
 * Additionall, performs the following:
 *  - Creates the first process (kernel_idle)
 *  - Registers a timer callback to display the process table/status
 */
void kproc_init(void);

/**
 * Creates a new process
 * @param proc_ptr - address of process to execute
 * @param proc_name - "friendly" process name
 * @param proc_type - process type (kernel or user)
 * @return process id of the created process, -1 on error
 */
int kproc_create(void *proc_ptr, char *proc_name, proc_type_t proc_type);

/**
 * Destroys a process
 * If the process is currently scheduled it must be unscheduled
 * @param proc - process entry
 * @return 0 on success, -1 on error
 */
int kproc_destroy(proc_t *proc);

/**
 * Looks up a process in the process table via the process id
 * @param pid - process id
 * @return pointer to the process entry, NULL or error or if not found
 */
proc_t *pid_to_proc(int pid);

/**
 * Looks up a process in the process table via the entry/index into the table
 * @param entry - entry/index value
 * @return pointer to the process entry, NULL or error or if not found
 */
proc_t *entry_to_proc(int entry);

/**
 * Test process
 */
void kproc_test(void);
#endif
