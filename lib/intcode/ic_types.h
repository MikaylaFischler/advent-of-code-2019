#ifndef INTCODE_TYPES_H_
#define INTCODE_TYPES_H_

#include <inttypes.h>

typedef struct intcode_buffer {
	int64_t** map;		// memory map; use this not buffer for access (allows pointing to arbitrary memory for piping)
	uint8_t** attr_map;	// attribute map; use this not attr for access (allows pointing to arbitrary memory for piping)
	int64_t* buffer;	// memory storage for this buffer
	uint8_t* attr;		// buffer attributes (array of attributes per memory block)
	uint16_t b_idx;		// next element to access
	uint16_t max;		// size of buffer
} icb_t;

typedef struct intcode_data {
	uint16_t memsize;	// size of memory
	int64_t* memory;	// memory contents
	int64_t* membkp;	// memory backup
	int16_t  rel_pos;	// current relative position base
	icb_t*   inbuf;		// input buffer
	icb_t*   outbuf;	// output buffer
	uint16_t pc;		// program counter for step commands
	uint8_t  verbose;	// whether to print out in/out commands or not
} icd_t;

#endif
