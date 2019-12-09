#ifndef INTCODE_H_
#define INTCODE_H_

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>

#include "ic_types.h"
#include "exit_codes.h"
#include "src/color.h"
#include "src/util.h"

#define IC_VERBOSE	1
#define IC_QUIET	0

#define IC_OP__ADD	1
#define IC_OP__MUL	2
#define IC_OP__INP	3
#define IC_OP__OUT	4
#define IC_OP__JNZ	5
#define IC_OP__JEZ	6
#define IC_OP__LES	7
#define IC_OP__EQL	8
#define IC_OP__SRL	9
#define IC_OP__HLT	99

#define IC_PC__INC_ENA	0xFF
#define IC_PC__INC_DIS	0x0

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
 * @param buffer buffer to modify
 * @param input_idx index to set
 * @param source new address
 */
void	intcode_buffer__link(icb_t* buffer, uint16_t input_idx, int64_t* source);

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
void intcode_buffer__set(icb_t* buffer, uint16_t idx, int64_t inval);

/**
 * @brief load initial value pair
 * @param icdata intcode data
 * @param a value 1
 * @param b value 2
 */
void	intcode_compute__load2(icd_t* icdata, int64_t a, int64_t b);

/**
 * @brief initialize the computation system (use if re-running intcode_compute or intcode_compute__step from beginning)
 * @param icdata intcode data
 */
void	intcode_compute__init(icd_t* icdata);

/**
 * @brief execute a single computational step (not to be used alternating with intcode_compute, they do not share program counters)
 * @param icdata intcode data
 * @param wrote 
 * @param has_data 
 * @return uint8_t Intcode exit code
 */
uint8_t intcode_compute__step(icd_t* icdata, uint8_t* wrote, uint8_t* has_data);

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
 * @brief backup intcode memory for restoration later
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
