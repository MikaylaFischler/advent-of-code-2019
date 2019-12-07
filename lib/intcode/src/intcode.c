#include "../intcode.h"

/* ----- Creation ----- */

icd_t* intcode_init(uint16_t in_len, uint16_t out_len) {
	icd_t* icd = (icd_t*) malloc(sizeof(icd_t));

	icd->memsize = 1000;
	icd->memory = malloc(sizeof(int32_t) * icd->memsize);
	icd->membkp = NULL;
	icd->pc = 0;
	
	__intcode__buffer_create(&icd->inbuf, in_len);
	__intcode__buffer_create(&icd->outbuf, out_len);

	return icd;
}

/* ----- Buffer ----- */

void intcode_buffer_link(icb_t* buffer, uint16_t input_idx, int32_t* source) {
	buffer->map[input_idx] = source;
}

/* ----- Computation ----- */

void intcode_load_init(icd_t* icdata, int32_t a, int32_t b) {
	icdata->memory[1] = a;
	icdata->memory[2] = b;
}

void intcode_init_comp(icd_t* icdata) {
	// control data
	icdata->pc = 0;

	// buffer indices
	icdata->inbuf->b_idx = 0;
	icdata->outbuf->b_idx = 0;
}

uint8_t intcode_compute_step(icd_t* icdata, uint8_t* wrote, uint8_t* has_data) {
	// control data
	int32_t		mode_bits = 0;
	int32_t		op = 0;
	uint16_t	pc_inc = 0;
	uint8_t		pc_inc_mask = IC_PC__INC_ENA;
	int32_t*	block;
	int32_t		params[2];

	// memory
	int32_t* memory = icdata->memory;

	// buffers
	icb_t* inbuf = icdata->inbuf;
	icb_t* outbuf = icdata->outbuf;

	// interpret memory
	block = (memory + icdata->pc);
	mode_bits = (block[0] > IC_OP__HLT) ? block[0] / 100 : 0;
	op = block[0] % 100;

	// read parameters in immediate mode or position mode
	// ensure blocks are valid memory
	if (op != IC_OP__HLT) {
		params[0] = (mode_bits & 0x1) ? block[1] : memory[block[1]];

		if (op != IC_OP__INP && op != IC_OP__OUT) {
			params[1] = ((mode_bits & 0xFE) ^ 0xA) ? memory[block[2]] : block[2];
		} else { params[1] = 0; }
	}

	// execute operation
	switch (op) {
		case IC_OP__ADD:	// Add Parameters and Write
			memory[block[3]] = params[0] + params[1];
			pc_inc = 4;
			break;
		case IC_OP__MUL:	// Multiply Parameters and Write
			memory[block[3]] = params[0] * params[1];
			pc_inc = 4;
			break;
		case IC_OP__INP:	// Read Input
			if (inbuf->b_idx == 1) {
				if (*has_data == 0) {
					return EXIT__MISSING_INPUT;
				}

				*has_data = 0;
			}

			if (inbuf->b_idx == inbuf->max) {
				printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "INP" RESET "(%d)] Input failed, out of range. Halted with code 2.\n", (intptr_t) (memory + icdata->pc), icdata->pc, inbuf->b_idx);
				return EXIT__INPUT_EMPTY;
			} else {
				memory[block[1]] = __intcode__buffer_read(inbuf);
				printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "INP" RESET "(%d)] " CYAN "<<" RESET " %d\n", (intptr_t) (memory + icdata->pc), icdata->pc, inbuf->b_idx - 1, memory[block[1]]);
			}

			if (inbuf->b_idx == 2) { inbuf->b_idx = 1; }

			pc_inc = 2;
			break;
		case IC_OP__OUT:	// Write Output
			if (outbuf->b_idx == outbuf->max) {
				printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "OUT" RESET "(%d)] Output failed, out of range. Halted with code 3.\n", (intptr_t) (memory + icdata->pc), icdata->pc, outbuf->b_idx);
				return EXIT__OUTPUT_FULL;
			} else {
				printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "OUT" RESET "(%d)] " CYAN ">>" RESET " %d\n", (intptr_t) (memory + icdata->pc), icdata->pc, outbuf->b_idx, params[0]);
				__intcode__buffer_write(outbuf, params[0]);
			}

			if (outbuf->b_idx == 1) { outbuf->b_idx = 0; }

			*wrote = 1;

			pc_inc = 2;
			break;
		case IC_OP__JNZ:	// Jump on Non-Zero
			if (params[0]) {
				icdata->pc = params[1];
				pc_inc_mask = IC_PC__INC_DIS;
			} else { pc_inc = 3; }
			break;
		case IC_OP__JEZ:	// Jump on Zero
			if (params[0] == 0) {
				icdata->pc = params[1];
				pc_inc_mask = IC_PC__INC_DIS;
			} else { pc_inc = 3; }
			break;
		case IC_OP__LES:	// Compare Less Than and Write
			memory[block[3]] = params[0] < params[1];
			pc_inc = 4;
			break;
		case IC_OP__EQL:	// Compare Equal and Write
			memory[block[3]] = params[0] == params[1];
			pc_inc = 4;
			break;
		case IC_OP__HLT:	// Halt
			return EXIT__NORMAL;
			break;
		default:
			printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "HLT" RESET "(%d)] Illegal Operation Attempted. Halted with code 1.\n", (intptr_t) (memory + icdata->pc), icdata->pc, op);
			return EXIT__ABNORMAL;
	}

	// increment program counter
	icdata->pc += pc_inc & pc_inc_mask;
	pc_inc_mask = IC_PC__INC_ENA;

	return EXIT__STEP_COMPLETE;
}

uint8_t intcode_compute(icd_t* icdata) {
	// control data
	uint8_t		run = 1;
	int32_t		mode_bits = 0;
	int32_t		op = 0;
	uint16_t	pc = 0;
	uint16_t	pc_inc = 0;
	uint8_t		pc_inc_mask = IC_PC__INC_ENA;
	int32_t*	block;
	int32_t		params[2];

	// memory
	int32_t*	memory = icdata->memory;

	// buffers
	icb_t* inbuf = icdata->inbuf;
	icb_t* outbuf = icdata->outbuf;

	do {
		// interpret memory
		block = (memory + pc);
		mode_bits = (block[0] > IC_OP__HLT) ? block[0] / 100 : 0;
		op = block[0] % 100;

		// read parameters in immediate mode or position mode
		// ensure blocks are valid memory
		if (op != IC_OP__HLT) {
			params[0] = (mode_bits & 0x1) ? block[1] : memory[block[1]];

			if (op != IC_OP__INP && op != IC_OP__OUT) {
				params[1] = ((mode_bits & 0xFE) ^ 0xA) ? memory[block[2]] : block[2];
			} else { params[1] = 0; }
		}

		// execute operation
		switch (op) {
			case IC_OP__ADD:	// Add Parameters and Write
				memory[block[3]] = params[0] + params[1];
				pc_inc = 4;
				break;
			case IC_OP__MUL:	// Multiply Parameters and Write
				memory[block[3]] = params[0] * params[1];
				pc_inc = 4;
				break;
			case IC_OP__INP:	// Read Input
				if (inbuf->b_idx == inbuf->max) {
					printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "INP" RESET "(%d)] Input failed, out of range. Halted with code 2.\n", (intptr_t) (memory + pc), pc, inbuf->b_idx);
					return EXIT__INPUT_EMPTY;
				} else {
					memory[block[1]] = __intcode__buffer_read(inbuf);
					printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "INP" RESET "(%d)] " CYAN "<<" RESET " %d\n", (intptr_t) (memory + pc), pc, inbuf->b_idx - 1, memory[block[1]]);
				}

				pc_inc = 2;
				break;
			case IC_OP__OUT:	// Write Output
				if (outbuf->b_idx == outbuf->max) {
					printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "OUT" RESET "(%d)] Output failed, out of range. Halted with code 3.\n", (intptr_t) (memory + pc), pc, outbuf->b_idx);
					return EXIT__OUTPUT_FULL;
				} else {
					printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "OUT" RESET "(%d)] " CYAN ">>" RESET " %d\n", (intptr_t) (memory + pc), pc, outbuf->b_idx, params[0]);
					__intcode__buffer_write(outbuf, params[0]);
				}

				pc_inc = 2;
				break;
			case IC_OP__JNZ:	// Jump on Non-Zero
				if (params[0]) {
					pc = params[1];
					pc_inc_mask = IC_PC__INC_DIS;
				} else { pc_inc = 3; }
				break;
			case IC_OP__JEZ:	// Jump on Zero
				if (params[0] == 0) {
					pc = params[1];
					pc_inc_mask = IC_PC__INC_DIS;
				} else { pc_inc = 3; }
				break;
			case IC_OP__LES:	// Compare Less Than and Write
				memory[block[3]] = params[0] < params[1];
				pc_inc = 4;
				break;
			case IC_OP__EQL:	// Compare Equal and Write
				memory[block[3]] = params[0] == params[1];
				pc_inc = 4;
				break;
			case IC_OP__HLT:	// Halt
				run = 0;
				break;
			default:
				printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "HLT" RESET "(%d)] Illegal Operation Attempted. Halted with code 1.\n", (intptr_t) (memory + pc), pc, op);
				return EXIT__ABNORMAL;
		}

		// increment program counter
		pc += pc_inc & pc_inc_mask;
		pc_inc_mask = IC_PC__INC_ENA;
	} while (run && pc < icdata->memsize + pc_inc);

	return run ? EXIT__ABNORMAL : EXIT__NORMAL;
}

int32_t intcode_result(icd_t* icdata) {
	return icdata->memory[0];
}

/* ----- Memory Management ----- */

void intcode_memory__load(int32_t* in, icd_t* icdata) {
	memcpy(icdata->memory, in, sizeof(int32_t) * icdata->memsize);
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
