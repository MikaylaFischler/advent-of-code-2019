#include "util.h"

void __intcode__buffer_create(icb_t** buffer, uint16_t len) {
	*buffer = NULL;

	if (len) {
		*buffer = malloc(sizeof(icb_t) * len);
		(*buffer)->map = malloc(sizeof(int64_t*) * len);
		(*buffer)->dirty = calloc(sizeof(uint8_t), len / 8 + (len % 8 > 0));
		(*buffer)->buffer = malloc(sizeof(int64_t) * len);
		(*buffer)->b_idx = 0;
		(*buffer)->max = len;

		for (uint16_t i = 0; i < len; i++) {
			(*buffer)->map[i] = (*buffer)->buffer + i;
		}
	}
}

int64_t __intcode__buffer_read(icb_t* buffer) {
	buffer->dirty[buffer->b_idx / 8] ^= (uint8_t) 0x1 << buffer->b_idx % 8;
	return *(buffer->map[buffer->b_idx++]);
}

void __intcode__buffer_write(icb_t* buffer, int64_t inval) {
	(*buffer->dirty) |= 0x1 << buffer->b_idx;
	*(buffer->map[buffer->b_idx++]) = inval;
}

void __intcode_memory__grow(icd_t* icdata) {
	uint16_t new_size = icdata->memsize + 200;

	// expand main memory
	int64_t* mem_old = icdata->memory;
	icdata->memory = malloc(sizeof(int64_t) * new_size);
	memcpy(icdata->memory, mem_old, icdata->memsize * sizeof(int64_t));
		
	// clear the new memory; avoid using calloc if we are writing to most of it anyway
	for (uint16_t i = icdata->memsize; i < new_size; i++) { icdata->memory[i] = 0x0; }

	// free old memory and update size
	free(mem_old);
	icdata->memsize = new_size;
}

void __intcode_memory__grow_runtime(icd_t* icdata, uint16_t new_addr) {
	if (new_addr <= icdata->memsize) { return; }
	uint16_t new_size = new_addr + 200;

	// expand main memory
	int64_t* mem_old = icdata->memory;
	icdata->memory = malloc(sizeof(int64_t) * new_size);
	memcpy(icdata->memory, mem_old, icdata->memsize * sizeof(int64_t));
		
	// clear the new memory; avoid using calloc if we are writing to most of it anyway
	for (uint16_t i = icdata->memsize; i < new_size; i++) { icdata->memory[i] = 0x0; }

	// free old memory
	free(mem_old);

	if (icdata->membkp != NULL) {
		// expand backup memory
		int64_t* memb_old = icdata->membkp;
		icdata->membkp = malloc(sizeof(int64_t) * new_size);
		memcpy(icdata->membkp, memb_old, icdata->memsize * sizeof(int64_t));
		
		// clear the new memory; avoid using calloc if we are writing to most of it anyway
		for (uint16_t i = icdata->memsize; i < new_size; i++) { icdata->memory[i] = 0x0; }

		// free old memory
		free(memb_old);
	}

	// size
	icdata->memsize = new_size;
}
