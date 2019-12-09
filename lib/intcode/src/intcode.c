#include "../intcode.h"

/* ----- Creation ----- */

icd_t* intcode_init(uint16_t in_len, uint16_t out_len, uint8_t verbose) {
	icd_t* icd = (icd_t*) malloc(sizeof(icd_t));

	icd->memsize = 5000;
	icd->memory = calloc(sizeof(int64_t), icd->memsize);
	icd->membkp = NULL;
	icd->rel_pos = 0;
	icd->pc = 0;
	icd->verbose = verbose;
	
	__intcode__buffer_create(&icd->inbuf, in_len);
	__intcode__buffer_create(&icd->outbuf, out_len);

	return icd;
}

/* ----- Buffer ----- */

void intcode_buffer__link(icb_t* buffer, uint16_t input_idx, int64_t* source) {
	buffer->map[input_idx] = source;
}

int64_t intcode_buffer__get(icb_t* buffer, uint16_t idx) {
	return *(buffer->map[idx]);
}

void intcode_buffer__set(icb_t* buffer, uint16_t idx, int64_t inval) {
	*(buffer->map[idx]) = inval;
}

/* ----- Computation ----- */

void intcode_compute__load2(icd_t* icdata, int64_t a, int64_t b) {
	icdata->memory[1] = a;
	icdata->memory[2] = b;
}

void intcode_compute__init(icd_t* icdata) {
	// control data
	icdata->pc = 0;
	icdata->rel_pos = 0;

	// buffer indices
	icdata->inbuf->b_idx = 0;
	icdata->outbuf->b_idx = 0;
}

uint8_t intcode_compute__step(icd_t* icdata, uint8_t* wrote, uint8_t* has_data) {
	// control data
	int64_t		mode_bits = 0;
	int64_t		op = 0;
	uint16_t	pc_inc = 0;
	uint8_t		pc_inc_mask = IC_PC__INC_ENA;
	int64_t*	block;
	int64_t		params[2];

	// memory
	int64_t* memory = icdata->memory;

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
				if (icdata->verbose) {
					printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "INP" RESET "(%d)] " CYAN "<<" RESET " %ld\n", (intptr_t) (memory + icdata->pc), icdata->pc, inbuf->b_idx, memory[block[1]]);
				}

				memory[block[1]] = __intcode__buffer_read(inbuf);
			}

			if (inbuf->b_idx == 2) { inbuf->b_idx = 1; }

			pc_inc = 2;
			break;
		case IC_OP__OUT:	// Write Output
			if (outbuf->b_idx == outbuf->max) {
				printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "OUT" RESET "(%d)] Output failed, out of range. Halted with code 3.\n", (intptr_t) (memory + icdata->pc), icdata->pc, outbuf->b_idx);
				return EXIT__OUTPUT_FULL;
			} else {
				if (icdata->verbose) {
					printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "OUT" RESET "(%d)] " CYAN ">>" RESET " %ld\n", (intptr_t) (memory + icdata->pc), icdata->pc, outbuf->b_idx, params[0]);
				}

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
			printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "HLT" RESET "(%ld)] Illegal Operation Attempted. Halted with code 1.\n", (intptr_t) (memory + icdata->pc), icdata->pc, op);
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
	int64_t		mode_bits = 0;
	int64_t		op = 0;
	uint16_t	pc = 0;
	uint16_t	pc_inc = 0;
	uint8_t		pc_inc_mask = IC_PC__INC_ENA;
	int64_t*	block;
	int64_t		params[2] = { 0, 0 };
	int64_t		output = 0;

	// memory
	int64_t*	memory = icdata->memory;

	// buffers
	icb_t* inbuf = icdata->inbuf;
	icb_t* outbuf = icdata->outbuf;

	do {
		// interpret memory
		block = (memory + pc);
		// mode_bits = (block[0] > IC_OP__HLT) ? block[0] / 100 : 0;
		// op = block[0] % 100;

		char __cmd[6];
		sprintf(__cmd, "%05ld", block[0]);

		op = ((__cmd[3] - 0x30) * 10) + (__cmd[4] - 0x30);

		if (op != IC_OP__HLT) {

			if (op != IC_OP__INP) {
				if (__cmd[2] == '1') {
					params[0] = block[1];
				} else if (__cmd[2] == '2') {
					params[0] = intcode_memory__safe_read(icdata, icdata->rel_pos + block[1]);
				} else {
					params[0] = intcode_memory__safe_read(icdata, block[1]);
				}
			} else {
				if (__cmd[2] == '1') {
					output = intcode_memory__safe_read(icdata, block[1]);
				} else if (__cmd[2] == '2') {
					output = icdata->rel_pos + block[1];
				} else {
					output = block[1];
				}
			}


			if (op != IC_OP__INP && op != IC_OP__OUT && op != IC_OP__SRL) {
				if (__cmd[1] == '1') {
					params[1] = block[2];
				} else if (__cmd[1] == '2') {
					params[1] = intcode_memory__safe_read(icdata, icdata->rel_pos + block[2]);
				} else {
					params[1] = intcode_memory__safe_read(icdata, block[2]);
				}

				if (__cmd[0] == '1') {
					output = intcode_memory__safe_read(icdata, block[3]);
				} else if (__cmd[0] == '2') {
					output = icdata->rel_pos + block[3];
				} else {
					output = block[3];
				}

			} else if (op != IC_OP__INP) {
				if (__cmd[1] == '1') {
					output = intcode_memory__safe_read(icdata, block[2]);
				} else if (__cmd[1] == '2') {
					output = icdata->rel_pos + block[2];
				} else {
					output = block[2];
				}
			}
		}

		// read parameters in immediate mode or position mode
		// ensure blocks are valid memory
		// if (op != IC_OP__HLT) {
		// 	params[0] = (mode_bits & 0x1) ? block[1] : memory[block[1]];

		// 	if (op != IC_OP__INP && op != IC_OP__OUT) {
		// 		params[1] = ((mode_bits & 0xFE) ^ 0xA) ? memory[block[2]] : block[2];
		// 	} else { params[1] = 0; }
		// }

		// execute operation
		switch (op) {
			case IC_OP__ADD:	// Add Parameters and Write
				// memory[block[3]] = params[0] + params[1];
				intcode_memory__safe_write(icdata, output, params[0] + params[1]);
				pc_inc = 4;
				break;
			case IC_OP__MUL:	// Multiply Parameters and Write
				// memory[block[3]] = params[0] * params[1];
				intcode_memory__safe_write(icdata, output, params[0] * params[1]);
				pc_inc = 4;
				break;
			case IC_OP__INP:	// Read Input
				if (inbuf->b_idx == inbuf->max) {
					printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "INP" RESET "(%d)] Input failed, out of range. Halted with code 2.\n", (intptr_t) (memory + pc), pc, inbuf->b_idx);
					return EXIT__INPUT_EMPTY;
				} else {
					intcode_memory__safe_write(icdata, output, __intcode__buffer_read(inbuf));
					// memory[block[1]] = __intcode__buffer_read(inbuf);
					printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "INP" RESET "(%d)] " CYAN "<<" RESET " %ld\n", (intptr_t) (memory + pc), pc, inbuf->b_idx - 1, memory[output]);
				}

				pc_inc = 2;
				break;
			case IC_OP__OUT:	// Write Output
				if (outbuf->b_idx == outbuf->max) {
					printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "OUT" RESET "(%d)] Output failed, out of range. Halted with code 3.\n", (intptr_t) (memory + pc), pc, outbuf->b_idx);
					return EXIT__OUTPUT_FULL;
				} else {
					printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "OUT" RESET "(%d)] " CYAN ">>" RESET " %ld\n", (intptr_t) (memory + pc), pc, outbuf->b_idx, params[0]);
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
				// memory[block[3]] = params[0] < params[1];
				intcode_memory__safe_write(icdata, output, params[0] < params[1]);
				pc_inc = 4;
				break;
			case IC_OP__EQL:	// Compare Equal and Write
				// memory[block[3]] = params[0] == params[1];
				intcode_memory__safe_write(icdata, output, params[0] == params[1]);
				pc_inc = 4;
				break;
			case IC_OP__SRL:	// Set relative location
				icdata->rel_pos += params[0];
				pc_inc = 2;
				break;
			case IC_OP__HLT:	// Halt
				run = 0;
				break;
			default:
				printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "HLT" RESET "(%ld)] Illegal Operation Attempted. Halted with code 1.\n", (intptr_t) (memory + pc), pc, op);
				return EXIT__ABNORMAL;
		}

		// increment program counter
		pc += pc_inc & pc_inc_mask;
		pc_inc_mask = IC_PC__INC_ENA;
	} while (run && pc < icdata->memsize + pc_inc);

	return run ? EXIT__ABNORMAL : EXIT__NORMAL;
}

int64_t intcode_result(icd_t* icdata) {
	return icdata->memory[0];
}

/* ----- Memory Management ----- */

void intcode_memory__load(int64_t* in, icd_t* icdata) {
	memcpy(icdata->memory, in, sizeof(int64_t) * icdata->memsize);
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
			icdata->memory[i++] = atol(token);

			// check for bad input
			// if (icdata->memory[i - 1] == 0 && token[0] != '0') { return 0; }

			token = strtok(NULL, ",");
		}
	}

	return 1;
}

int64_t intcode_memory__safe_read(icd_t* icdata, uint16_t addr) {
	if (addr >= icdata->memsize) {
		__intcode_memory__grow_runtime(icdata, addr);
	}

	return icdata->memory[addr];
}

void intcode_memory__safe_write(icd_t* icdata, uint16_t addr, int64_t val) {
	if (addr >= icdata->memsize) {
		__intcode_memory__grow_runtime(icdata, addr);
	}

	icdata->memory[addr] = val;
}

void intcode_memory__backup(icd_t* icdata) {
	// allocate backup memory
	if (icdata->membkp == NULL) {
		icdata->membkp = malloc(sizeof(int64_t) * icdata->memsize);
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
