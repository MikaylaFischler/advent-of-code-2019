#ifndef INTCODE_TYPES_H_
#define INTCODE_TYPES_H_

#include <inttypes.h>

typedef struct intcode_buffer {
	int64_t** map;		// memory map; use this not buffer for access (allows pointing to arbitray memory for piping)
	uint8_t* dirty;		// whether position n has been written to. NOT AN ARRAY: each bit corresponds to an index
	int64_t* buffer;	// memory storage for this buffer
	uint16_t b_idx;		// next element to access
	uint16_t max;		// size of buffer
} icb_t;

typedef struct intcode_data {
	uint16_t memsize;	// size of memory
	int64_t* memory;	// memory contents
	int64_t* membkp;	// memory backup
	int16_t  rel_pos;
	icb_t*   inbuf;		// input buffer
	icb_t*   outbuf;	// output buffer
	uint16_t pc;		// program counter for step commands
	uint8_t  verbose;	// whether to print out in/out commands or not
} icd_t;

#endif
