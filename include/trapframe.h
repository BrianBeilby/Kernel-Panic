/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * 
 *
 * Process Trapframe
 */
#ifndef TRAPFRAME_H
#define TRAPFRAME_H

#ifndef ASSEMBLER

// This structure corresponds to the state of user registers
// as saved upon kernel trap/interrupt entry.

typedef unsigned short seg_type_t;  // 16-bit segment value

typedef struct {
    // Saved segment registers
    seg_type_t gs;      // unsigned short, 2 bytes
    seg_type_t _notgs;  // filler, 2 bytes

    seg_type_t fs;
    seg_type_t _notfs;

    seg_type_t es;
    seg_type_t _notes;

    seg_type_t ds;
    seg_type_t _notds;

    // register states
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int esp;  // Push: before PUSHA, Pop: skipped
    unsigned int ebx;
    unsigned int edx;
    unsigned int ecx;
    unsigned int eax;

    // Indicate the type of interrupt that has happened
    unsigned int interrupt;

    // CPU state
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
} trapframe_t;

#endif
#endif
