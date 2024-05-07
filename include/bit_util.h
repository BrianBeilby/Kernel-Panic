/**
 * CPE/CSC 159 Operating System Pragmatics
 * California State University, Sacramento
 *
 * Bit Utilities
 */
#ifndef BIT_UTIL_H
#define BIT_UTIL_H

/**
 * Counts the number of bits that are set
 * @param value - the integer value to count bits in
 * @return number of bits that are set
 */
int bit_count(int value);

/**
 * Checks if the given bit is set
 * @param value - the integer value to test
 * @param bit - which bit to check
 * @return 1 if set, 0 if not set
 */
int bit_test(int value, int bit);

/**
 * Sets the specified bit in the given integer value
 * @param value - the integer value to modify
 * @param bit - which bit to set
 */
int bit_set(int value, int bit);

/**
 * Clears the specified bit in the given integer value
 * @param value - the integer value to modify
 * @param bit - which bit to clear
 */
int bit_clear(int value, int bit);

/**
 * Toggles the specified bit in the given integer value
 * @param value - the integer value to modify
 * @param bit - which bit to toggle
 */
int bit_toggle(int value, int bit);

#endif
