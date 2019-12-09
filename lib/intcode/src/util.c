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
	icd_t* mem_new = malloc(sizeof(int64_t) * (icdata->memsize + 100));
	memcpy(mem_new, icdata->memory, icdata->memsize * sizeof(int64_t));

	if (icdata->membkp != NULL) {
		icd_t* memb_new = malloc(sizeof(int64_t) * (icdata->memsize + 100));
		memcpy(memb_new, icdata->membkp, icdata->memsize * sizeof(int64_t));
	}

	icdata->memsize += 100;
}

void __intcode_memory__grow_runtime(icd_t* icdata, uint16_t new_addr) {
	if (new_addr <= icdata->memsize) { return; }

	printf("growing during runtime... (needed addr %d)\n", new_addr);

	uint16_t new_size = new_addr - icdata->memsize + 200;

	icd_t* mem_new = calloc(sizeof(int64_t), (new_addr - icdata->memsize + 200));
	memcpy(mem_new, icdata->memory, icdata->memsize * sizeof(int64_t));

	if (icdata->membkp != NULL) {
		icd_t* memb_new = calloc(sizeof(int64_t), (new_addr - icdata->memsize + 200));
		memcpy(memb_new, icdata->membkp, icdata->memsize * sizeof(int64_t));
	}

	icdata->memsize = new_addr - icdata->memsize + 200;
}
