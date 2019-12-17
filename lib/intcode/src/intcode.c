#include "../intcode.h"

/* ----- Creation ----- */

icd_t* intcode_init(uint16_t in_len, uint16_t out_len, uint8_t verbose) {
	icd_t* icd = (icd_t*) malloc(sizeof(icd_t));

	icd->memsize = 1000;
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

void intcode_buffer__link(icb_t* buffer, uint16_t input_idx, int64_t* source, uint8_t* attr_source) {
	buffer->map[input_idx] = source;
	buffer->attr_map[input_idx] = attr_source;
	*(buffer->attr_map[input_idx]) &= ~IC_BF__DRT;
}

int64_t intcode_buffer__get(icb_t* buffer, uint16_t idx) {
	return *(buffer->map[idx]);
}

void intcode_buffer__set(icb_t* buffer, uint16_t idx, int64_t inval) {
	*(buffer->map[idx]) = inval;
	*(buffer->attr_map[idx]) |= IC_BF__DRT;
}

void intcode_buffer__set_mode_once(icb_t* buffer, uint16_t idx) {
	*(buffer->attr_map[idx]) |= IC_BF__NCE;
}

void intcode_buffer__set_mode_stream(icb_t* buffer, uint16_t idx) {
	*(buffer->attr_map[idx]) &= ~IC_BF__NCE;
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

	// mark inbuf as unread
	for (uint16_t i = 0; i <= icdata->inbuf->b_idx; i++) {
		if (icdata->inbuf->attr[i] & IC_BF__NCE) {
			icdata->inbuf->attr[i] |= IC_BF__DRT;
		}
	}

	// mark outbuf as unread
	for (uint16_t i = 0; i <= icdata->outbuf->b_idx; i++) {
		if (icdata->outbuf->attr[i] & IC_BF__NCE) {
			icdata->outbuf->attr[i] |= IC_BF__DRT;
		}
	}

	// buffer indices
	icdata->inbuf->b_idx = 0;
	icdata->outbuf->b_idx = 0;
}

uint8_t intcode_compute(icd_t* icdata) {
	// control data
	uint8_t		run = 1;
	int64_t		op = 0;
	uint16_t	pc = icdata->pc;
	uint16_t	pc_inc = 0;
	uint8_t		pc_inc_mask = IC_PC__INC_ENA;
	int64_t*	block;
	int64_t		params[2] = { 0, 0 };
	int64_t		output = 0;
	char		cmd[6];

	// memory
	int64_t*	memory = icdata->memory;

	// buffers
	icb_t*		inbuf = icdata->inbuf;
	icb_t*		outbuf = icdata->outbuf;

	// to allow printing input buffer behavior properly
	uint16_t __idx;
	int64_t __inp;

	do {
		// interpret memory
		block = (memory + pc);

		sprintf(cmd, "%05ld", block[0]);

		// get operation
		op = ((cmd[3] - 0x30) * 10) + (cmd[4] - 0x30);

		// decode and set parameters
		switch (op) {
			case IC_OP__ADD:
			case IC_OP__MUL:
			case IC_OP__JNZ:
			case IC_OP__JEZ:
			case IC_OP__LES:
			case IC_OP__EQL:
				// all if not input, output, or set relative location
				if (cmd[1] == '1') {
					params[1] = block[2];
				} else if (cmd[1] == '2') {
					params[1] = intcode_memory__safe_read(icdata, icdata->rel_pos + block[2]);
				} else {
					params[1] = intcode_memory__safe_read(icdata, block[2]);
				}

				// all if not input, output, or set relative location
				if (cmd[0] == '1') {
					output = intcode_memory__safe_read(icdata, block[3]);
				} else if (cmd[0] == '2') {
					output = icdata->rel_pos + block[3];
				} else {
					output = block[3];
				}
			case IC_OP__OUT:
			case IC_OP__SRL:
				// all except input
				if (cmd[2] == '1') {
					params[0] = block[1];
				} else if (cmd[2] == '2') {
					params[0] = intcode_memory__safe_read(icdata, icdata->rel_pos + block[1]);
				} else {
					params[0] = intcode_memory__safe_read(icdata, block[1]);
				}

				// only out and set relative location
				if (op == IC_OP__OUT || op == IC_OP__SRL) {
					if (cmd[1] == '1') {
						output = intcode_memory__safe_read(icdata, block[2]);
					} else if (cmd[1] == '2') {
						output = icdata->rel_pos + block[2];
					} else {
						output = block[2];
					}
				}
				break;
			case IC_OP__INP:
				// only for input
				if (cmd[2] == '1') {
					output = intcode_memory__safe_read(icdata, block[1]);
				} else if (cmd[2] == '2') {
					output = icdata->rel_pos + block[1];
				} else {
					output = block[1];
				}
				break;
			case IC_OP__HLT:
				break;
		}

		// execute operation
		switch (op) {
			case IC_OP__ADD:	// Add Parameters and Write
				intcode_memory__safe_write(icdata, output, params[0] + params[1]);
				pc_inc = 4;
				break;
			case IC_OP__MUL:	// Multiply Parameters and Write
				intcode_memory__safe_write(icdata, output, params[0] * params[1]);
				pc_inc = 4;
				break;
			case IC_OP__INP:	// Read Input
				if (inbuf->b_idx == inbuf->max) {
					printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "INP" RESET "(%d)] Input failed, out of range. Halted with code 2.\n", (intptr_t) (memory + pc), pc, inbuf->b_idx);
					return EXIT__INPUT_EMPTY;
				} else {
					if (__intcode__buffer_dirty(inbuf)) {
						if (icdata->verbose) {
							__idx = inbuf->b_idx;
							__inp = __intcode__buffer_read(inbuf);
							printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "INP" RESET "(%d)] " CYAN "<<" RESET " %ld\n", (intptr_t) (memory + pc), pc, __idx, __inp);
							intcode_memory__safe_write(icdata, output, __inp);
						} else {
							intcode_memory__safe_write(icdata, output, __intcode__buffer_read(inbuf));
						}
					} else {
						// printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "INP" RESET "(%d)] Input failed, no new data. Halted with code 4.\n", (intptr_t) (memory + pc), pc, inbuf->b_idx);
						return EXIT__MISSING_INPUT;
					}
				}

				pc_inc = 2;
				break;
			case IC_OP__OUT:	// Write Output
				if (outbuf->b_idx == outbuf->max) {
					printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "OUT" RESET "(%d)] Output failed, out of range. Halted with code 3.\n", (intptr_t) (memory + pc), pc, outbuf->b_idx);
					return EXIT__OUTPUT_FULL;
				} else {
					if (icdata->verbose) {
						printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET ":" RED "OUT" RESET "(%d)] " CYAN ">>" RESET " %ld\n", (intptr_t) (memory + pc), pc, outbuf->b_idx, params[0]);
					}

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
				intcode_memory__safe_write(icdata, output, params[0] < params[1]);
				pc_inc = 4;
				break;
			case IC_OP__EQL:	// Compare Equal and Write
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
		icdata->pc = pc;
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
	memcpy(icdata->membkp, icdata->memory, sizeof(int64_t) * icdata->memsize);
}

uint8_t intcode_memory__restore(icd_t* icdata) {
	if (icdata->membkp != NULL) {
		// copy backup to main
		memcpy(icdata->memory, icdata->membkp, sizeof(int64_t) * icdata->memsize);
	} else { return 0; }

	return 1;
}
