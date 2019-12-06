#ifndef INTCODE_H_
#define INTCODE_H_

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>

#include "src/color.h"

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

typedef struct intcode_data {
	uint16_t memsize;	// size of memory
	int32_t* memory;	// memory contents
	int32_t* membkp;	// memory backup
	int32_t* inbuf;		// input buffer
	uint16_t inbuflen;	// input buffer length
	uint16_t inlen;		// length of buffer used
	int32_t* outbuf;	// output buffer
	uint16_t outbuflen;	// output buffer length
	uint16_t outlen;	// length of buffer used
} icd_t;

/**
 * @brief create an intcode data instance
 * @param in Input buffer
 * @param out Output buffer
 * @param in_len Input buffer length
 * @param out_len Output buffer length
 * @return icd_t* new intcode data
 */
icd_t*	intcode_init(int32_t* in, int32_t* out, uint16_t in_len, uint16_t out_len);

/**
 * @brief load initial value pair
 * @param icdata intcode data
 * @param a value 1
 * @param b value 2
 */
void	intcode_load_init(icd_t* icdata, int32_t a, int32_t b);

/**
 * @brief execute intcode memory
 * @param icdata intcode data
 * @return uint8_t 1 on succesful halt, 0 on unexpected termination
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
