#include "util.h"

void __intcode__buffer_create(icb_t** buffer, uint16_t len) {
	*buffer = NULL;

	if (len) {
		*buffer = malloc(sizeof(icb_t) * len);
		(*buffer)->map = malloc(sizeof(int32_t*) * len);
		(*buffer)->dirty = calloc(sizeof(uint8_t), len / 8 + (len % 8 > 0));
		(*buffer)->buffer = malloc(sizeof(int32_t) * len);
		(*buffer)->b_idx = 0;
		(*buffer)->max = len;

		for (uint16_t i = 0; i < len; i++) {
			(*buffer)->map[i] = (*buffer)->buffer + i;
		}
	}
}

int32_t __intcode__buffer_read(icb_t* buffer) {
	buffer->dirty[buffer->b_idx / 8] ^= (uint8_t) 0x1 << buffer->b_idx % 8;
	return *(buffer->map[buffer->b_idx++]);
}

void __intcode__buffer_write(icb_t* buffer, int32_t inval) {
	(*buffer->dirty) |= 0x1 << buffer->b_idx;
	*(buffer->map[buffer->b_idx++]) = inval;
}

void __intcode_memory__grow(icd_t* icdata) {
	icd_t* mem_new = malloc(sizeof(int32_t) * (icdata->memsize + 100));
	memcpy(mem_new, icdata->memory, icdata->memsize);

	if (icdata->membkp != NULL) {
		icd_t* memb_new = malloc(sizeof(int32_t) * (icdata->memsize + 100));
		memcpy(memb_new, icdata->membkp, icdata->memsize);
	}

	icdata->memsize += 100;
}
