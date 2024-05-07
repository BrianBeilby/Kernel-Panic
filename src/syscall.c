/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * System call APIs
 */
#include "syscall.h"

/**
 * Executes a system call without any arguments
 * @param syscall - the system call identifier
 * @return return code from the the system call
 */
int _syscall0(int syscall) {
    int rc = -1;

    asm("movl %1, %%eax;"
        "int $0x80;"
        "movl %%eax, %0;"
        : "=g"(rc)
        : "g"(syscall)
        : "%eax");

    return rc;
}

/**
 * Executes a system call with one argument
 * @param syscall - the system call identifier
 * @param arg1 - first argument
 * @return return code from the the system call
 */
int _syscall1(int syscall, int arg1) {
    int rc = -1;

    asm("movl %1, %%eax;"
        "movl %2, %%ebx;"
        "int $0x80;"
        "movl %%eax, %0;"
        : "=g"(rc)
        : "g"(syscall), "g"(arg1)
        : "%eax", "%ebx");

    return rc;
}

/**
 * Executes a system call with two arguments
 * @param syscall - the system call identifier
 * @param arg1 - first argument
 * @param arg2 - second argument
 * @return return code from the the system call
 */
int _syscall2(int syscall, int arg1, int arg2) {
    int rc = -1;

    asm("movl %1, %%eax;"
        "movl %2, %%ebx;"
        "movl %3, %%ecx;"
        "int $0x80;"
        "movl %%eax, %0;"
        : "=g"(rc)
        : "g"(syscall), "g"(arg1), "g"(arg2)
        : "%eax", "%ebx", "%ecx");

    return rc;
}

/**
 * Executes a system call with three arguments
 * @param syscall - the system call identifier
 * @param arg1 - first argument
 * @param arg2 - second argument
 * @param arg3 - third argument
 * @return return code from the the system call
 */
int _syscall3(int syscall, int arg1, int arg2, int arg3) {
    int rc = -1;

    asm("movl %1, %%eax;"
        "movl %2, %%ebx;"
        "movl %3, %%ecx;"
        "movl %4, %%edx;"
        "int $0x80;"
        "movl %%eax, %0;"
        : "=g"(rc)
        : "g"(syscall), "g"(arg1), "g"(arg2), "g"(arg3)
        : "%eax", "%ebx", "%ecx", "%edx");

    return rc;
}

/**
 * Gets the current system time (in seconds)
 * @return system time in seconds
 */
int sys_get_time(void) {
    return _syscall0(SYSCALL_SYS_GET_TIME);
}

/**
 * Gets the operating system name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int sys_get_name(char *name) {
    return _syscall1(SYSCALL_SYS_GET_NAME, (int)name);
}

/**
 * Puts the current process to sleep for the specified number of seconds
 * @param seconds - number of seconds the process should sleep
 */
void proc_sleep(int secs) {
    _syscall1(SYSCALL_PROC_SLEEP, secs);
}

/**
 * Exits the current process
 * @param exitcode An exit code to return to the parent process
 */
void proc_exit(int exitcode) {
    _syscall1(SYSCALL_PROC_EXIT, exitcode);
}

/**
 * Gets the current process' id
 * @return process id
 */
int proc_get_pid(void) {
    return _syscall0(SYSCALL_PROC_GET_PID);
}

/**
 * Gets the current process' name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int proc_get_name(char *name) {
    return _syscall1(SYSCALL_PROC_GET_NAME, (int)name);
}

/**
 * Writes up to n bytes to the process' specified IO buffer
 * @param io - the IO buffer to write to
 * @param buf - the buffer to copy from
 * @param n - number of bytes to write
 * @return -1 on error or value indicating number of bytes copied
 */
int io_write(int io, char *buf, int n) {
    return _syscall3(SYSCALL_IO_WRITE, io, (int)buf, n);
}

/**
 * Reads up to n bytes from the process' specified IO buffer
 * @param io - the IO buffer to read from
 * @param buf - the buffer to copy to
 * @param n - number of bytes to read
 * @return -1 on error or value indicating number of bytes copied
 */
int io_read(int io, char *buf, int n) {
    return _syscall3(SYSCALL_IO_READ, io, (int)buf, n);
}

/**
 * Flushes (clears) the specified IO buffer
 * @param io - the IO buffer to flush
 * @return -1 on error or 0 on success
 */
int io_flush(int io) {
    return _syscall1(SYSCALL_IO_FLUSH, io);
}

/**
 * Allocates a mutex from the kernel
 * @return -1 on error, all other values indicate the mutex id
 */
int mutex_init(void) {
    return _syscall0(SYSCALL_MUTEX_INIT);
}

/**
 * Destroys a mutex
 * @return -1 on error, 0 on sucecss
 */
int mutex_destroy(int mutex) {
    return _syscall1(SYSCALL_MUTEX_DESTROY, mutex);
}

/**
 * Locks the mutex
 * @param mutex - mutex id
 * @return -1 on error, 0 on sucecss
 * @note If the mutex is already locked, process will block/wait.
 */
int mutex_lock(int mutex) {
    return _syscall1(SYSCALL_MUTEX_LOCK, mutex);
}

/**
 * Unlocks the mutex
 * @param mutex - mutex id
 * @return -1 on error, 0 on sucecss
 */
int mutex_unlock(int mutex) {
    return _syscall1(SYSCALL_MUTEX_UNLOCK, mutex);
}

/**
 * Allocates a semaphore from the kernel
 * @param value - initial semaphore value
 * @return -1 on error, all other values indicate the semaphore id
 */
int sem_init(int value) {
    return _syscall1(SYSCALL_SEM_INIT, value);
}

/**
 * Destroys a semaphore
 * @param sem - semaphore id
 * @return -1 on error, 0 on success
 */
int sem_destroy(int sem) {
    return _syscall1(SYSCALL_SEM_DESTROY, sem);
}

/**
 * Waits on a semaphore
 * @param sem - semaphore id
 * @return -1 on error, otherwise the current semaphore count
 */
int sem_wait(int sem) {
    return _syscall1(SYSCALL_SEM_WAIT, sem);
}

/**
 * Posts a semaphore
 * @param sem - semaphore id
 * @return -1 on error, otherwise the current semaphore count
 */
int sem_post(int sem) {
    return _syscall1(SYSCALL_SEM_POST, sem);
}

