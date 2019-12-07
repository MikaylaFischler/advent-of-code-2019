#ifndef INTCODE_UTIL_H_
#define INTCODE_UTIL_H_

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

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
int32_t __intcode__buffer_read(icb_t* buffer);

/**
 * @brief write a value to the buffer
 * @param buffer buffer
 * @param inval value
 */
void __intcode__buffer_write(icb_t* buffer, int32_t inval);

/**
 * @brief grow the intcode memory block (for use during loading)
 * @param icdata intcode data
 */
void __intcode_memory__grow(icd_t* icdata);

#endif