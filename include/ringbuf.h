/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Simple ring buffer implementation
 */

#ifndef RINGBUF_H
#define RINGBUF_H

#include <spede/stdbool.h>    // For bool type
#include <spede/stddef.h>     // For size_t

#ifndef RINGBUF_SIZE
#define RINGBUF_SIZE 2048
#endif

typedef struct ringbuf_t {
    int head;                   // Head of the buffer
    int tail;                   // Tail of the buffer
    int size;                   // Current size of the buffer
    char data[RINGBUF_SIZE];   // Data in buffer
} ringbuf_t;

/**
 * Initializes an empty ring buffer
 * Sets the empty data to 0
 *
 * @param  buf - pointer to the ring buffer data structure
 * @return -1 on error; 0 on success
 */
int ringbuf_init(ringbuf_t *buf);

/**
 * Writes a byte to the buffer
 * @param  buf   - pointer to the ring buffer structure
 * @param  byte  - the byte to write
 * @return -1 on error; 0 on success
 */
int ringbuf_write(ringbuf_t *buf, char byte);

/**
 * Reads a byte from the buffer
 * @param  buf - pointer to the ring buffer structure
 * @param  byte - pointer to the byte to be stored
 * @return -1 on error; 0 on success
 */
int ringbuf_read(ringbuf_t *buf, char *byte);

/**
 * Copies multiple bytes to the buffer from the specified memory
 * @param buf - pointer to the ring buffer structure
 * @param mem - pointer to the memory location to copy from
 * @param size - number of bytes to copy
 * @return -1 on error, 0 on success
 * @note Should return an error if the number of bytes
 *       cannot be copied - i.e. the buffer would overflow
 */
int ringbuf_write_mem(ringbuf_t *buf, char *mem, size_t size);

/**
 * Copies multiple bytes from the buffer to the specified memory
 * @param buf - pointer to the ring buffer structure
 * @param mem - pointer to the memory location to copy to
 * @param size - number of bytes to copy
 * @return -1 on error, positive value to indicate number of bytes
 *         copied
 */
int ringbuf_read_mem(ringbuf_t *buf, char *mem, size_t size);

/**
 * Flushes (empties) the buffer
 * @param buf - pointer to the ring buffer structure
 * @return -1 on error, 0 on success
 */
int ringbuf_flush(ringbuf_t *buf);

/**
 * Indicates if the buffer is empty
 * @param buf - pointer to the ring buffer structure
 * @return true if empty, false if not empty
 */
bool ringbuf_is_empty(ringbuf_t *buf);

/**
 * Indicates if the buffer if full
 * @param buf - pointer to the ring buffer structure
 * @return true if full, false if not full
 */
bool ringbuf_is_full(ringbuf_t *buf);

#endif
