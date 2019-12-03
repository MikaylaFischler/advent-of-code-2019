#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "../lib/intcode/intcode.h"

#include "main.h"

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
	printf(YELLOW ">" B_WHITE " initializing memory...\n" RESET);
	mem_start = clock();

	icd_t* icdata;
	intcode_memory__load_file(fp, icdata);
	intcode_memory__backup(icdata);

	mem_end = clock();
	printf(B_GREEN ">" B_WHITE " memory initialized " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (mem_end - mem_start) * 1000.0 / CLOCKS_PER_SEC);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	// load in values for part 1
	intcode_load_init(icdata, 12, 2);

	// calculate result for part 1
	printf(YELLOW ">" B_WHITE " executing part 1...\n" RESET);
	p1_start = clock();

	if (intcode_compute(icdata)) {
		p1_end = clock();
		printf(B_GREEN ">" B_WHITE " part 1 execution succeeded " WHITE "(" BLUE "%lu us" WHITE ")\n" RESET, (p1_end - p1_start) * 1000000 / CLOCKS_PER_SEC);
		p1_res = intcode_result(icdata);
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
			intcode_memory__restore(icdata);
			intcode_load_init(icdata, a, b);

			if (intcode_compute(icdata) == 0) {
				printf(RED ">" B_WHITE " intcode execution failed for (noun,verb) pair (%d,%d)", a, b);
			}

			if (intcode_result(icdata) == 19690720) {
				p2_end = clock();
				p2_res = a * 100 + b;
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
