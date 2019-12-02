#include "../intcode.h"

icd_t* intcode_init(void) {
	icd_t* icd = (icd_t*) malloc(sizeof(icd_t));
	icd->memsize = 1000;
	icd->memory = malloc(sizeof(int32_t) * icd->memsize);
	icd->membkp = NULL;
	return icd;
}

void intcode_load_init(icd_t* icdata, int32_t a, int32_t b) {
	icdata->memory[1] = a;
	icdata->memory[2] = b;
}

uint8_t intcode_compute(icd_t* icdata) {
	uint8_t  halt 	= 0;
	uint16_t pc 	= 0;
	uint16_t pc_inc	= 0;

	do {
		int32_t* block = (icdata->memory + pc);

		switch (block[0]) {
			case IC_OP__ADD:
				icdata->memory[block[3]] = icdata->memory[block[1]] + icdata->memory[block[2]];
				pc_inc = 4;
				break;
			case IC_OP__MUL:
				icdata->memory[block[3]] = icdata->memory[block[1]] * icdata->memory[block[2]];
				pc_inc = 4;
				break;
			case IC_OP__HLT:
				halt = 1;
				break;
			default:
				return 0;
		}

		pc += pc_inc;
	} while (!halt && pc < icdata->memsize + pc_inc);

	return halt;
}

int32_t intcode_result(icd_t* icdata) {
	return icdata->memory[0];
}

void intcode_memory__load(int32_t* in, icd_t* icdata) {
	memcpy(icdata->memory, in, icdata->memsize);
}

uint8_t intcode_memory__load_file(FILE* in, icd_t* icdata) {
	// file i/o
    char* line = NULL;
    size_t len = 0;
	int num_read = 0;

	// memory index
	uint16_t i = 0;

	// read in memory
	while ((num_read = getline(&line, &len, in)) != -1) {
		// tokenize line, delimited by comma
		char* token = strtok(line, ",");
		while (token != NULL) {
			// grow memory if needed
			if (i == icdata->memsize) { __intcode_memory__grow(icdata); }

			// write value to memory
			icdata->memory[i++] = atoi(token);

			// check for bad input
			if (icdata->memory[i - 1] == 0 && token[0] != '0') { return 0; }

			token = strtok(NULL, ",");
		}
	}

	return 1;
}

void intcode_memory__backup(icd_t* icdata) {
	// allocate backup memory
	if (icdata->membkp == NULL) {
		icdata->membkp = malloc(sizeof(int32_t) * icdata->memsize);
	}

	// copy main to backup
	memcpy(icdata->membkp, icdata->memory, icdata->memsize);
}

uint8_t intcode_memory__restore(icd_t* icdata) {
	if (icdata->membkp != NULL) {
		// copy backup to main
		memcpy(icdata->memory, icdata->membkp, icdata->memsize);
	} else { return 0; }

	return 1;
}

static void __intcode_memory__grow(icd_t* icdata) {
	icd_t* mem_new = malloc(sizeof(int32_t) * (icdata->memsize + 100));
	memcpy(mem_new, icdata->memory, icdata->memsize);

	if (icdata->membkp != NULL) {
		icd_t* memb_new = malloc(sizeof(int32_t) * (icdata->memsize + 100));
		memcpy(memb_new, icdata->membkp, icdata->memsize);
	}

	icdata->memsize += 100;
}
