#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"
#include "../../lib/intcode/intcode.h"

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 05" WHITE "           |\n" RESET);
    printf(WHITE "x------------------------------------------------x\n\n" RESET);

	// file i/o variables
    FILE* fp;

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

	// timing
	clock_t time_start, time_end, ic_end, p1_start, p1_end, p2_start, p2_end;

	printf(YELLOW "starting...\n" RESET);
	printf(YELLOW ">" B_WHITE " parsing intcode...\n" RESET);

	time_start = clock();

	int32_t p1 = 0;
	int32_t p2 = 0;

	// create intcode processor
	icd_t* icdata = intcode_init(1, 15, IC_VERBOSE);
	intcode_memory__load_file(fp, icdata);
	intcode_memory__backup(icdata);

	intcode_buffer__set(icdata->inbuf, 0, 1);

	ic_end = clock();
	printf(B_GREEN ">" B_WHITE " intcode parsed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (ic_end - time_start) * 100000.0 / CLOCKS_PER_SEC, ic_end - time_start);
	printf(YELLOW ">" B_WHITE " executing part 1...\n" RESET);
	p1_start = clock();

	// compute part 1
	intcode_compute(icdata);
	p1 = intcode_buffer__get(icdata->outbuf, icdata->outbuf->b_idx - 1);

	p1_end = clock();
	printf(B_GREEN ">" B_WHITE " intcode executed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p1_end - p1_start) * 100000.0 / CLOCKS_PER_SEC, p1_end - p1_start);

	// setup memory
	intcode_buffer__set(icdata->inbuf, 0, 5);
	intcode_memory__restore(icdata);

	printf(YELLOW ">" B_WHITE " executing part 2...\n" RESET);
	p2_start = clock();
	
	// compute part 2
	intcode_init_comp(icdata);
	intcode_compute(icdata);
	p2 = intcode_buffer__get(icdata->outbuf, icdata->outbuf->b_idx - 1);

	p2_end = clock();
	printf(B_GREEN ">" B_WHITE " intcode executed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p2_end - p2_start) * 100000.0 / CLOCKS_PER_SEC, p2_end - p2_start);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "diagnostic_code(1)" WHITE "\t: " CYAN "%d\n" RESET, p1);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "diagnostic_code(5)" WHITE "\t: " CYAN "%d\n" RESET, p2);

	// free up resource
	fclose(fp);

	return 0;
}
