/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel System Call Handlers
 */
#include <spede/time.h>
#include <spede/string.h>
#include <spede/stdio.h>

#include "kernel.h"
#include "kproc.h"
#include "ksyscall.h"
#include "interrupts.h"
#include "scheduler.h"
#include "timer.h"
#include "ksem.h"
#include "kmutex.h"

/**
 * System call IRQ handler
 * Dispatches system calls to the function associate with the specified system call
 */
void ksyscall_irq_handler(void) {
    // Default return value
    int rc = -1;

    // System call number
    int syscall;

    // Arguments
    unsigned int arg1;
    unsigned int arg2;
    unsigned int arg3;

    if (!active_proc) {
        kernel_panic("Invalid process");
    }

    if (!active_proc->trapframe) {
        kernel_panic("Invalid trapframe");
    }

    // Get data from the trapframe registers
    // System call identifier is stored on the EAX register
    // Additional arguments should be stored on additional registers (ebx, ecx, etc.)
    syscall = active_proc->trapframe->eax;
    arg1 = active_proc->trapframe->ebx;
    arg2 = active_proc->trapframe->ecx;
    arg3 = active_proc->trapframe->edx;

    // Dispatch to the appropriate function
    // Cast parameters as necessary
    switch (syscall) {
        case SYSCALL_IO_WRITE:
            rc = ksyscall_io_write((int)arg1, (char *)arg2, (int)arg3);
            break;

        case SYSCALL_IO_READ:
            rc = ksyscall_io_read((int)arg1, (char *)arg2, (int)arg3);
            break;

        case SYSCALL_IO_FLUSH:
            rc = ksyscall_io_flush((int)arg1);
            break;

        case SYSCALL_SYS_GET_TIME:
            rc = ksyscall_sys_get_time();
            break;

        case SYSCALL_SYS_GET_NAME:
            rc = ksyscall_sys_get_name((char *)arg1);
            break;

        case SYSCALL_PROC_SLEEP:
            rc = ksyscall_proc_sleep((int)arg1);
            break;

        case SYSCALL_PROC_EXIT:
            rc = ksyscall_proc_exit();
            break;

        case SYSCALL_PROC_GET_PID:
            rc = ksyscall_proc_get_pid();
            break;

        case SYSCALL_PROC_GET_NAME:
            rc = ksyscall_proc_get_name((char *)arg1);
            break;

        case SYSCALL_MUTEX_INIT:
            rc = ksyscall_mutex_init();
            break;

        case SYSCALL_MUTEX_DESTROY:
            rc = ksyscall_mutex_destroy(arg1);
            break;

        case SYSCALL_MUTEX_LOCK:
            rc = ksyscall_mutex_lock(arg1);
            break;

        case SYSCALL_MUTEX_UNLOCK:
            rc = ksyscall_mutex_unlock(arg1);
            break;

        case SYSCALL_SEM_INIT:
            rc = ksyscall_sem_init(arg1);
            break;

        case SYSCALL_SEM_DESTROY:
            rc = ksyscall_sem_destroy(arg1);
            break;

        case SYSCALL_SEM_POST:
            rc = ksyscall_sem_post(arg1);
            break;

        case SYSCALL_SEM_WAIT:
            rc = ksyscall_sem_wait(arg1);
            break;

        default:
            kernel_panic("Invalid system call %d!", syscall);
    }

    // Ensure that the EAX register contains a return value (if appropriate)
    if (active_proc) {
        active_proc->trapframe->eax = (unsigned int)rc;
    }
}

/**
 * System Call Initialization
 */
void ksyscall_init(void) {
    // Register the IDT entry and IRQ handler for the syscall IRQ (IRQ_SYSCALL)
    interrupts_irq_register(IRQ_SYSCALL, isr_entry_syscall, ksyscall_irq_handler);
}

/**
 * Writes up to n bytes to the process' specified IO buffer
 * @param io - the IO buffer to write to
 * @param buf - the buffer to copy from
 * @param n - number of bytes to write
 * @return -1 on error or value indicating number of bytes copied
 */
int ksyscall_io_write(int io, char *buf, int size) {
    if (!active_proc) {
        return -1;
    }

    if (!active_proc) {
        return -1;
    }

    if (io < 0 || io >= PROC_IO_MAX) {
        return -1;
    }

    if (!active_proc->io[io]) {
        return -1;
    }

    return ringbuf_write_mem(active_proc->io[io], buf, size);
}

/**
 * Reads up to n bytes from the process' specified IO buffer
 * @param io - the IO buffer to read from
 * @param buf - the buffer to copy to
 * @param n - number of bytes to read
 * @return -1 on error or value indicating number of bytes copied
 */
int ksyscall_io_read(int io, char *buf, int size) {
    if (!active_proc) {
        return -1;
    }

    if (io < 0 || io >= PROC_IO_MAX) {
        return -1;
    }

    if (!active_proc->io[io]) {
        return -1;
    }

    return ringbuf_read_mem(active_proc->io[io], buf, size);

}

/**
 * Flushes (clears) the specified IO buffer
 * @param io - the IO buffer to flush
 * @return -1 on error or 0 on success
 */
int ksyscall_io_flush(int io) {
    if (!active_proc) {
        return -1;
    }

    if (io < 0 || io >= PROC_IO_MAX) {
        return -1;
    }

    if (!active_proc->io[io]) {
        return -1;
    }

    ringbuf_flush(active_proc->io[io]);

    return 0;
}

/**
 * Gets the current system time (in seconds)
 * @return system time in seconds
 */
int ksyscall_sys_get_time(void) {
    return timer_get_ticks() / 100;
}

/**
 * Gets the operating system name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int ksyscall_sys_get_name(char *name) {
    if (!name) {
        return -1;
    }

    strncpy(name, OS_NAME, sizeof(OS_NAME));
    return 0;
}

/**
 * Puts the active process to sleep for the specified number of seconds
 * @param seconds - number of seconds the process should sleep
 */
int ksyscall_proc_sleep(int seconds) {
    scheduler_sleep(active_proc, seconds * 100);
    return 0;
}

/**
 * Exits the current process
 */
int ksyscall_proc_exit(void) {
    return kproc_destroy(active_proc);
}

/**
 * Gets the active process pid
 * @return process id or -1 on error
 */
int ksyscall_proc_get_pid(void) {
    if (!active_proc) {
        return -1;
    }

    return active_proc->pid;
}

/**
 * Gets the active process' name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int ksyscall_proc_get_name(char *name) {
    if (!name) {
        return -1;
    }

    strncpy(name, active_proc->name, PROC_NAME_LEN);
    return 0;
}

/**
 * Allocates a mutex from the kernel
 * @return -1 on error, all other values indicate the mutex id
 */
int ksyscall_mutex_init(void) {
    return kmutex_init();
}

/**
 * Detroys a mutex
 * @return -1 on error, 0 on sucecss
 */
int ksyscall_mutex_destroy(int mutex) {
    return kmutex_destroy(mutex);
}

/**
 * Locks the mutex
 * @param mutex - mutex id
 * @return -1 on error, 0 on sucecss
 * @note If the mutex is already locked, process will block/wait.
 */
int ksyscall_mutex_lock(int mutex) {
    return kmutex_lock(mutex);
}

/**
 * Unlocks the mutex
 * @param mutex - mutex id
 * @return -1 on error, 0 on sucecss
 */
int ksyscall_mutex_unlock(int mutex) {
    return kmutex_unlock(mutex);
}

/**
 * Allocates a semaphore from the kernel
 * @param value - initial semaphore value
 * @return -1 on error, all other values indicate the semaphore id
 */
int ksyscall_sem_init(int value) {
    return ksem_init(value);
}

/**
 * Destroys a semaphore
 * @param sem - semaphore id
 * @return -1 on error, 0 on success
 */
int ksyscall_sem_destroy(int sem) {
    return ksem_destroy(sem);
}

/**
 * Waits on a semaphore
 * @param sem - semaphore id
 * @return -1 on error, otherwise the current semaphore count
 */
int ksyscall_sem_wait(int sem) {
    return ksem_wait(sem);
}

/**
 * Posts a semaphore
 * @param sem - semaphore id
 * @return -1 on error, otherwise the current semaphore count
 */
int ksyscall_sem_post(int sem) {
    return ksem_post(sem);
}

