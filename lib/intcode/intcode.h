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

#define IC_OP__ADD 1
#define IC_OP__MUL 2
#define IC_OP__INP 3
#define IC_OP__OUT 4
#define IC_OP__JNZ 5
#define IC_OP__JEZ 6
#define IC_OP__LES 7
#define IC_OP__EQL 8
#define IC_OP__HLT 99

#define IC_PC__INC_ENA	0xFF
#define IC_PC__INC_DIS	0x0

/**
 * @brief create an intcode data instance
 * @param in_len Input buffer length
 * @param out_len Output buffer length
 * @return icd_t* new intcode data
 */
icd_t*	intcode_init(uint16_t in_len, uint16_t out_len);

/**
 * @brief link buffer's input at input_idx to point to source (used to create 'pipes')
 * @param buffer buffer to modify
 * @param input_idx index to set
 * @param source new address
 */
void	intcode_buffer_link(icb_t* buffer, uint16_t input_idx, int32_t* source);

/**
 * @brief load initial value pair
 * @param icdata intcode data
 * @param a value 1
 * @param b value 2
 */
void	intcode_load_init(icd_t* icdata, int32_t a, int32_t b);

/**
 * @brief initialize the computation system (use if re-running intcode_compute_step from beginning)
 * @param icdata intcode data
 */
void	intcode_init_comp(icd_t* icdata);

/**
 * @brief execute a single computational step (not to be used alternating with intcode_compute, they do not share program counters)
 * @param icdata intcode data
 * @param wrote 
 * @param has_data 
 * @return uint8_t Intcode exit code
 */
uint8_t intcode_compute_step(icd_t* icdata, uint8_t* wrote, uint8_t* has_data);

/**
 * @brief execute intcode memory
 * @param icdata intcode data
 * @return uint8_t Intcode exit code
 */
uint8_t	intcode_compute(icd_t* icdata);

/**
 * @brief after intcode_compute, get the result
 * @param icdata intcode data
 * @return int32_t result
 */
int32_t intcode_result(icd_t* icdata);

/**
 * @brief load intcode memory from an array of length icdata->memsize
 * @param in array input
 * @param icdata intcode data
 */
void	intcode_memory__load(int32_t* in, icd_t* icdata);

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

#endif
