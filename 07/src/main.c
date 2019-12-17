#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"
#include "permuatations.h"
#include "../../lib/intcode/intcode.h"

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 07" WHITE "           |\n" RESET);
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
	clock_t time_start, time_end, parse_end, p1_start, p1_end, p2_start, p2_end;

	printf(YELLOW "starting...\n" RESET);
	printf(YELLOW ">" B_WHITE " parsing intcode...\n" RESET);

	time_start = clock();

	int32_t p1 = 0;
	int32_t p2 = 0;

	// create intcode cores
	icd_t* icdata_a = intcode_init(2, 1, IC_QUIET);
	icd_t* icdata_b = intcode_init(2, 1, IC_QUIET);
	icd_t* icdata_c = intcode_init(2, 1, IC_QUIET);
	icd_t* icdata_d = intcode_init(2, 1, IC_QUIET);
	icd_t* icdata_e = intcode_init(2, 1, IC_QUIET);

	// load memory
	intcode_memory__load_file(fp, icdata_a);
	intcode_memory__load(icdata_a->memory, icdata_b);
	intcode_memory__load(icdata_a->memory, icdata_c);
	intcode_memory__load(icdata_a->memory, icdata_d);
	intcode_memory__load(icdata_a->memory, icdata_e);

	// backup memory
	intcode_memory__backup(icdata_a);
	intcode_memory__backup(icdata_b);
	intcode_memory__backup(icdata_c);
	intcode_memory__backup(icdata_d);
	intcode_memory__backup(icdata_e);

	parse_end = clock();
	printf(B_GREEN ">" B_WHITE " intcode parsed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (parse_end - time_start) * 100000.0 / CLOCKS_PER_SEC, parse_end - time_start);
	printf(YELLOW ">" B_WHITE " running part 1...\n" RESET);
	p1_start = clock();
	
	// compute part 1
	for (uint8_t i = 0; i < 120; i++) {
		intcode_buffer__set(icdata_a->inbuf, 0, permu_0_4[i][0]);
		intcode_buffer__set(icdata_a->inbuf, 1, 0);
		intcode_compute(icdata_a);

		intcode_buffer__set(icdata_b->inbuf, 0, permu_0_4[i][1]);
		intcode_buffer__set(icdata_b->inbuf, 1, icdata_a->outbuf->buffer[0]);
		intcode_compute(icdata_b);

		intcode_buffer__set(icdata_c->inbuf, 0, permu_0_4[i][2]);
		intcode_buffer__set(icdata_c->inbuf, 1, icdata_b->outbuf->buffer[0]);
		intcode_compute(icdata_c);

		intcode_buffer__set(icdata_d->inbuf, 0, permu_0_4[i][3]);
		intcode_buffer__set(icdata_d->inbuf, 1, icdata_c->outbuf->buffer[0]);
		intcode_compute(icdata_d);

		intcode_buffer__set(icdata_e->inbuf, 0, permu_0_4[i][4]);
		intcode_buffer__set(icdata_e->inbuf, 1, icdata_d->outbuf->buffer[0]);
		intcode_compute(icdata_e);

		if (icdata_e->outbuf->buffer[0] > p1) {
			p1 = icdata_e->outbuf->buffer[0];
		}

		intcode_memory__restore(icdata_a);
		intcode_memory__restore(icdata_b);
		intcode_memory__restore(icdata_c);
		intcode_memory__restore(icdata_d);
		intcode_memory__restore(icdata_e);

		intcode_compute__init(icdata_a);
		intcode_compute__init(icdata_b);
		intcode_compute__init(icdata_c);
		intcode_compute__init(icdata_d);
		intcode_compute__init(icdata_e);
	}

	// setup for part 2

	p1_end = clock();
	printf(B_GREEN ">" B_WHITE " part 1 completed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p1_end - p1_start) * 100000.0 / CLOCKS_PER_SEC, p1_end - p1_start);
	printf(YELLOW ">" B_WHITE " running part 2...\n" RESET);
	p2_start = clock();

	// create 'pipes'
	intcode_buffer__link(icdata_a->inbuf, 1, icdata_e->outbuf->buffer, icdata_e->outbuf->attr);
	intcode_buffer__link(icdata_b->inbuf, 1, icdata_a->outbuf->buffer, icdata_a->outbuf->attr);
	intcode_buffer__link(icdata_c->inbuf, 1, icdata_b->outbuf->buffer, icdata_b->outbuf->attr);
	intcode_buffer__link(icdata_d->inbuf, 1, icdata_c->outbuf->buffer, icdata_c->outbuf->attr);
	intcode_buffer__link(icdata_e->inbuf, 1, icdata_d->outbuf->buffer, icdata_d->outbuf->attr);

	// set 'pipes' as streams
	intcode_buffer__set_mode_stream(icdata_a->inbuf, 1);
	intcode_buffer__set_mode_stream(icdata_b->inbuf, 1);
	intcode_buffer__set_mode_stream(icdata_c->inbuf, 1);
	intcode_buffer__set_mode_stream(icdata_d->inbuf, 1);
	intcode_buffer__set_mode_stream(icdata_e->inbuf, 1);

	// compute part 2
	for (uint8_t i = 0; i < 120; i++) {
		icdata_a->inbuf->buffer[0] = permu_5_9[i][0];
		icdata_b->inbuf->buffer[0] = permu_5_9[i][1];
		icdata_c->inbuf->buffer[0] = permu_5_9[i][2];
		icdata_d->inbuf->buffer[0] = permu_5_9[i][3];
		icdata_e->inbuf->buffer[0] = permu_5_9[i][4];

		intcode_compute__init(icdata_a);
		intcode_compute__init(icdata_b);
		intcode_compute__init(icdata_c);
		intcode_compute__init(icdata_d);
		intcode_compute__init(icdata_e);

		intcode_buffer__set(icdata_a->inbuf, 1, 0);

		uint8_t run = EXIT__MISSING_INPUT;
		while (run == EXIT__MISSING_INPUT) {
			intcode_compute(icdata_a);
			intcode_compute(icdata_b);
			intcode_compute(icdata_c);
			intcode_compute(icdata_d);
			run = intcode_compute(icdata_e);
		}

		if (icdata_e->outbuf->buffer[0] > p2) {
			p2 = icdata_e->outbuf->buffer[0];
		}

		intcode_memory__restore(icdata_a);
		intcode_memory__restore(icdata_b);
		intcode_memory__restore(icdata_c);
		intcode_memory__restore(icdata_d);
		intcode_memory__restore(icdata_e);

		intcode_compute__init(icdata_a);
		intcode_compute__init(icdata_b);
		intcode_compute__init(icdata_c);
		intcode_compute__init(icdata_d);
		intcode_compute__init(icdata_e);
	}

	p2_end = clock();
	printf(B_GREEN ">" B_WHITE " part 2 completed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p2_end - p2_start) * 100000.0 / CLOCKS_PER_SEC, p2_end - p2_start);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "max val" WHITE "\t: " CYAN "%d\n" RESET, p1);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "feedback" WHITE "\t: " CYAN "%d\n" RESET, p2);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
