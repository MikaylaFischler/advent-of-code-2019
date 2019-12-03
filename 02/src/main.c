#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "main.h"

#define MEM_SIZE 1000

#define IC_OP__ADD 1
#define IC_OP__MUL 2
#define IC_OP__HLT 99

#define IC_INC_PC(pc) (pc = pc + 4)

typedef struct instruction {
	int32_t op;
	int32_t a;
	int32_t b;
	int32_t dest;
} instr_t;

/**
 * @brief Execute a series of intcode instructions in provided memory
 *
 * @param memory Initial memory to be modified by execution
 * @return uint8_t 1 if halted, 0 if error
 */
uint8_t intcode_compute(int32_t* memory) {
	uint8_t  halt 	= 0;
	uint16_t pc 	= 0;

	do {
		instr_t* instr = (instr_t*) (memory + pc);

		switch (instr->op) {
			case IC_OP__ADD:
				memory[instr->dest] = memory[instr->a] + memory[instr->b];
				break;
			case IC_OP__MUL:
				memory[instr->dest] = memory[instr->a] * memory[instr->b];
				break;
			case IC_OP__HLT:
				halt = 1;
				break;
			default:
				printf(B_RED ">" RESET " Intcode: illegal OP code at instruction %d\n", pc);
				return 0;
		}

		IC_INC_PC(pc);
	} while (!halt && pc < MEM_SIZE - 4);

	return halt;
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 02" WHITE "           |\n" RESET);
    printf(WHITE "x------------------------------------------------x\n\n" RESET);

	// file i/o variables
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
	int num_read = 0;

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

	// timing
	clock_t time_start, time_end, mem_start, mem_end, p1_start, p1_end, p2_start, p2_end;

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	// results variables
	int32_t p1_res = 0;
	int32_t p2_res = 0;

	// setup intcode computer
	uint16_t pc = 0;
	int32_t* memory 	= malloc(sizeof(int32_t) * MEM_SIZE);
	int32_t* memory_bkp	= malloc(sizeof(int32_t) * MEM_SIZE);

	printf(YELLOW ">" B_WHITE " initializing memory...\n" RESET);
	mem_start = clock();

	// read in memory
	while ((num_read = getline(&line, &len, fp)) != -1) {
		// tokenize line, delimited by comma
		char* token = strtok(line, ",");
		while (token != NULL) {
			memory[pc++] = atoi(token);

			if (memory[pc - 1] == 0 && token[0] != '0') {
				// bad input, skip
				pc--;
				printf(B_YELLOW ">" RESET " failed to parse \"%s\" as integer input\n", token);
			}

			token = strtok(NULL, ",");
		}
	}

	// backup initial configuration
	memcpy(memory_bkp, memory, sizeof(int32_t) * MEM_SIZE);

	mem_end = clock();
	printf(B_GREEN ">" B_WHITE " memory initialized " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (mem_end - mem_start) * 1000.0 / CLOCKS_PER_SEC);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	// load in values for part 1
	memory[1] = 12;
	memory[2] = 2;

	// calculate result for part 1
	printf(YELLOW ">" B_WHITE " executing part 1...\n" RESET);
	p1_start = clock();

	if (intcode_compute(memory)) {
		p1_end = clock();
		printf(B_GREEN ">" B_WHITE " part 1 execution succeeded " WHITE "(" BLUE "%lu us" WHITE ")\n" RESET, (p1_end - p1_start) * 1000000 / CLOCKS_PER_SEC);
		p1_res = memory[0];
	} else {
		p1_end = clock();
		printf(B_RED ">" B_WHITE " part 1 execution failed " WHITE "(" BLUE "%lu us" WHITE ")\n" RESET, (p1_end - p1_start) * 1000000 / CLOCKS_PER_SEC);
		p1_res = -1;
	}

	// calculate result for part 2
	printf(YELLOW ">" B_WHITE " executing part 2...\n" RESET);
	p2_start = clock();

	uint8_t run = 1;
	for (int a = 0; a <= 99 && run; a++) {
		for (int b = 0; b <= 99 && run; b++) {
			memcpy(memory, memory_bkp, 1000 * sizeof(int));

			memory[1] = a;
			memory[2] = b;

			if (intcode_compute(memory) == 0) {
				printf(RED ">" B_WHITE " intcode execution failed for (noun,verb) pair (%d,%d)", a, b);
			}

			if (memory[0] == 19690720) {
				p2_end = clock();
				p2_res = memory[1] * 100 + memory[2];
				run = 0;
			}
		}
	}

	printf(B_GREEN ">" B_WHITE " part 2 execution completed " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (p2_end - p2_start) * 1000.0 / CLOCKS_PER_SEC);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "value after halt" WHITE "\t: " CYAN "%d\n" RESET, p1_res);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "gravity assist" WHITE "\t\t: " CYAN "%d\n" RESET, p2_res);

	return 0;
}
