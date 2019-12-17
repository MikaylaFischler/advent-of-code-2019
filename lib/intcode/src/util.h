#ifndef INTCODE_UTIL_H_
#define INTCODE_UTIL_H_

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "../ic_defs.h"
#include "../ic_types.h"

/**
 * @brief create an intcode buffer
 * @param buffer reference to buffer pointer to populate
 * @param len buffer length
 */
void __intcode__buffer_create(icb_t** buffer, uint16_t len);

/**
 * @brief read a value from the buffer
 * @param buffer buffer
 * @return value
 */
int64_t __intcode__buffer_read(icb_t* buffer);

/**
 * @brief write a value to the buffer
 * @param buffer buffer
 * @param inval value
 */
void __intcode__buffer_write(icb_t* buffer, int64_t inval);

/**
 * @brief check if the next value to be read is dirty/unread or not
 * @param buffer buffer
 * @return uint8_t 0 if unchanged, non-zero if changed
 */
uint8_t __intcode__buffer_dirty(icb_t* buffer);

/**
 * @brief grow the intcode memory block (for use during loading)
 * @param icdata intcode data
 */
void __intcode_memory__grow(icd_t* icdata);

void __intcode_memory__grow_runtime(icd_t* icdata, uint16_t new_addr);

#endif