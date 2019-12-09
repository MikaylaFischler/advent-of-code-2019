#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"
#include "../../lib/intcode/intcode.h"

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 09" WHITE "           |\n" RESET);
    printf(WHITE "x------------------------------------------------x\n\n" RESET);

	// file i/o variables
    FILE* fp;

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

	// timing
	clock_t time_start, time_end, parse_end, p1_start, p1_end, p2_start, p2_end;;

	printf(YELLOW "starting...\n" RESET);
	printf(YELLOW ">" B_WHITE " parsing intcode...\n" RESET);

	time_start = clock();

	int64_t p1, p2;

	// create intcode program
	icd_t* icdata = intcode_init(100, 100, IC_VERBOSE);

	// load memory
	intcode_memory__load_file(fp, icdata);

	// free up file
	fclose(fp);

	// backup memory
	intcode_memory__backup(icdata);

	parse_end = clock();
	printf(B_GREEN ">" B_WHITE " intcode parsed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (parse_end - time_start) * 100000.0 / CLOCKS_PER_SEC, parse_end - time_start);
	printf(YELLOW ">" B_WHITE " executing in test mode...\n" RESET);
	p1_start = clock();

	// compute part 1
	intcode_buffer__set(icdata->inbuf, 0, 1);
	intcode_compute(icdata);

	p1 = intcode_buffer__get(icdata->outbuf, 0);

	p1_end = clock();
	printf(B_GREEN ">" B_WHITE " test mode executed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p1_end - p1_start) * 100000.0 / CLOCKS_PER_SEC, p1_end - p1_start);
	printf(YELLOW ">" B_WHITE " executing in sensor boost mode...\n" RESET);
	p2_start = clock();

	// compute part 2
	intcode_buffer__set(icdata->inbuf, 0, 2);
	intcode_memory__restore(icdata);
	intcode_compute__init(icdata);
	intcode_compute(icdata);

	p2 = intcode_buffer__get(icdata->outbuf, 0);

	p2_end = clock();
	printf(B_GREEN ">" B_WHITE " sensor boost executed " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (p2_end - p2_start) * 1000.0 / CLOCKS_PER_SEC);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "test mode" WHITE "\t: " CYAN "%ld\n" RESET, p1);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "boost mode" WHITE "\t: " CYAN "%ld\n" RESET, p2);

	return 0;
}
