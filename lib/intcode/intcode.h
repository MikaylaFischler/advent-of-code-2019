#ifndef INTCODE_H_
#define INTCODE_H_

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>

#include "ic_defs.h"
#include "ic_types.h"
#include "exit_codes.h"
#include "src/color.h"
#include "src/util.h"

/**
 * @brief create an intcode data instance
 * @param in_len Input buffer length
 * @param out_len Output buffer length
 * @param verbose whether to print out in/out commands or not (IC_VERBOSE or IC_QUIET)
 * @return icd_t* new intcode data
 */
icd_t*	intcode_init(uint16_t in_len, uint16_t out_len, uint8_t verbose);

/**
 * @brief link buffer's input at input_idx to point to source (used to create 'pipes')
 * @note this also sets this buffer as clean (read)
 * @param buffer buffer to modify
 * @param input_idx index to set
 * @param source new address
 * @param attr_source new attribute address
 */
void	intcode_buffer__link(icb_t* buffer, uint16_t input_idx, int64_t* source, uint8_t* attr_source);

/**
 * @brief get a value from a buffer
 * @param buffer buffer
 * @param idx index to read
 * @return value
 */
int64_t intcode_buffer__get(icb_t* buffer, uint16_t idx);

/**
 * @brief set the value of an index in a buffer
 * @param buffer buffer
 * @param idx index to write to
 * @param inval value
 */
void	intcode_buffer__set(icb_t* buffer, uint16_t idx, int64_t inval);

/**
 * @brief set this buffer position to once mode; the default mode (this position will be read once then the index will be incremented)
 * @param buffer buffer
 * @param idx index to set mode for
 */
void	intcode_buffer__set_mode_once(icb_t* buffer, uint16_t idx);

/**
 * @brief set this buffer position to stream mode (once reached, all further reads will be done from here)
 * @param buffer buffer
 * @param idx index to set mode for
 */
void	intcode_buffer__set_mode_stream(icb_t* buffer, uint16_t idx);

/**
 * @brief load initial value pair
 * @param icdata intcode data
 * @param a value 1
 * @param b value 2
 */
void	intcode_compute__load2(icd_t* icdata, int64_t a, int64_t b);

/**
 * @brief initialize the computation system (use if re-running intcode_compute from beginning)
 * @param icdata intcode data
 */
void	intcode_compute__init(icd_t* icdata);

/**
 * @brief execute intcode memory
 * @param icdata intcode data
 * @return uint8_t Intcode exit code
 */
uint8_t	intcode_compute(icd_t* icdata);

/**
 * @brief after intcode_compute, get the result
 * @param icdata intcode data
 * @return int64_t result
 */
int64_t intcode_result(icd_t* icdata);

/**
 * @brief load intcode memory from an array of length icdata->memsize
 * @param in array input
 * @param icdata intcode data
 */
void	intcode_memory__load(int64_t* in, icd_t* icdata);

/**
 * @brief load intcode from an input file
 * @param in input file pointer
 * @param icdata intcode data
 * @return uint8_t 1 on success, 0 on failure (bad file)
 */
uint8_t	intcode_memory__load_file(FILE* in, icd_t* icdata);

/**
 * @brief backup intcode memory for restoration later<br/>
 * 	do not use this before memory__load's
 * @param icdata intcode data
 */
void	intcode_memory__backup(icd_t* icdata);

/**
 * @brief retore intcode from a memory backup
 * @param icdata intcode data
 * @return uint8_t 1 on success, 0 if no data to restore
 */
uint8_t	intcode_memory__restore(icd_t* icdata);


int64_t intcode_memory__safe_read(icd_t* icdata, uint16_t addr);
void intcode_memory__safe_write(icd_t* icdata, uint16_t addr, int64_t val);

#endif
