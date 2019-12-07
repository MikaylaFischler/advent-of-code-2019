#ifndef INTCODE_TYPES_H_
#define INTCODE_TYPES_H_

#include <inttypes.h>

typedef struct intcode_buffer {
	int32_t** map;		// memory map; use this not buffer for access (allows pointing to arbitray memory for piping)
	uint8_t* dirty;		// whether position n has been written to. NOT AN ARRAY: each bit corresponds to an index
	int32_t* buffer;	// memory storage for this buffer
	uint16_t b_idx;		// next element to access
	uint16_t max;		// size of buffer
} icb_t;

typedef struct intcode_data {
	uint16_t memsize;	// size of memory
	int32_t* memory;	// memory contents
	int32_t* membkp;	// memory backup
	icb_t*   inbuf;		// input buffer
	icb_t*   outbuf;	// output buffer
	uint16_t pc;
} icd_t;

#endif
