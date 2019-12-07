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
	clock_t time_start, time_end;

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	int32_t p1 = 0;
	int32_t p2 = 0;

	// create intcode cores
	icd_t* icdata_a = intcode_init(20, 1, IC_QUIET);
	icd_t* icdata_b = intcode_init(20, 1, IC_QUIET);
	icd_t* icdata_c = intcode_init(20, 1, IC_QUIET);
	icd_t* icdata_d = intcode_init(20, 1, IC_QUIET);
	icd_t* icdata_e = intcode_init(20, 1, IC_QUIET);

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

	// create 'pipes'
	intcode_buffer_link(icdata_b->inbuf, 1, icdata_a->outbuf->buffer);
	intcode_buffer_link(icdata_c->inbuf, 1, icdata_b->outbuf->buffer);
	intcode_buffer_link(icdata_d->inbuf, 1, icdata_c->outbuf->buffer);
	intcode_buffer_link(icdata_e->inbuf, 1, icdata_d->outbuf->buffer);
	intcode_buffer_link(icdata_a->inbuf, 1, icdata_e->outbuf->buffer);
	
	int32_t max = 0;

	// for (uint8_t i = 0; i < 120; i++) {
	// 	inbuf_a[0] = permu_0_4[i][0];
	// 	inbuf_a[1] = 0;
	// 	intcode_compute(icdata_a);
	// 	inbuf_b[0] = permu_0_4[i][1];
	// 	inbuf_b[1] = outbuf_a;
	// 	intcode_compute(icdata_b);
	// 	inbuf_c[0] = permu_0_4[i][2];
	// 	inbuf_c[1] = outbuf_b;
	// 	intcode_compute(icdata_c);
	// 	inbuf_d[0] = permu_0_4[i][3];
	// 	inbuf_d[1] = outbuf_c;
	// 	intcode_compute(icdata_d);
	// 	inbuf_e[0] = permu_0_4[i][4];
	// 	inbuf_e[1] = outbuf_d;
	// 	intcode_compute(icdata_e);

	// 	if (outbuf_e > max) {
	// 		max = outbuf_e;
	// 		max_idx = i;
	// 	}

	// 	intcode_memory__restore(icdata_a);
	// 	intcode_memory__restore(icdata_b);
	// 	intcode_memory__restore(icdata_c);
	// 	intcode_memory__restore(icdata_d);
	// 	intcode_memory__restore(icdata_e);

	// }

	uint8_t a_wrote = 0;
	uint8_t b_wrote = 0;
	uint8_t c_wrote = 0;
	uint8_t d_wrote = 0;
	uint8_t e_wrote = 0;


	for (uint8_t i = 50; i < 120; i++) {
		icdata_a->inbuf->buffer[0] = permu_5_9[i][0];
		icdata_b->inbuf->buffer[0] = permu_5_9[i][1];
		icdata_c->inbuf->buffer[0] = permu_5_9[i][2];
		icdata_d->inbuf->buffer[0] = permu_5_9[i][3];
		icdata_e->inbuf->buffer[0] = permu_5_9[i][4];
		// icdata_a->inbuf->buffer[0] = 9;
		// icdata_b->inbuf->buffer[0] = 7;
		// icdata_c->inbuf->buffer[0] = 8;
		// icdata_d->inbuf->buffer[0] = 5;
		// icdata_e->inbuf->buffer[0] = 6;

		intcode_init_comp(icdata_a);
		intcode_init_comp(icdata_b);
		intcode_init_comp(icdata_c);
		intcode_init_comp(icdata_d);
		intcode_init_comp(icdata_e);

		a_wrote = 0;
		b_wrote = 0;
		c_wrote = 0;
		d_wrote = 0;
		e_wrote = 1;

		icdata_a->inbuf->buffer[1] = 0;
		icdata_e->outbuf->buffer[0] = 0;

		uint8_t run = EXIT__STEP_COMPLETE;
		while (run == EXIT__STEP_COMPLETE) {
			while (run && !a_wrote) {
				run = intcode_compute_step(icdata_a, &a_wrote, &e_wrote);
				if (run != EXIT__STEP_COMPLETE) { e_wrote = 0; }
			}
			run = EXIT__STEP_COMPLETE;
			// a_wrote = 0;
			while (run && !b_wrote) {
				// printf("b\n");
				run = intcode_compute_step(icdata_b, &b_wrote, &a_wrote);
			}
			run = EXIT__STEP_COMPLETE;
			// b_wrote = 0;
			while (run && !c_wrote) {
				// printf("c\n");
				run = intcode_compute_step(icdata_c, &c_wrote, &b_wrote);
			}
			run = EXIT__STEP_COMPLETE;
			// c_wrote = 0;
			while (run && !d_wrote) {
				// printf("d\n");
				run = intcode_compute_step(icdata_d, &d_wrote, &c_wrote);
			}
			run = EXIT__STEP_COMPLETE;
			// d_wrote = 0;
			while (run && !e_wrote) {
				// printf("e %d %d\n", e_wrote, d_wrote);
				run = intcode_compute_step(icdata_e, &e_wrote, &d_wrote);
	// printf("[" YELLOW "0x%lx" RESET ":" MAGENTA "%04d" RESET "] Running step...!\n", (intptr_t) (icdata_e->memory + icdata_e->pc), icdata_e->pc);
				// printf("e end %d, %d (%d)\n", e_wrote, d_wrote, run);
			}
		}

		if (icdata_e->outbuf->buffer[0] > max) {
			max = icdata_e->outbuf->buffer[0];
		}

		intcode_memory__restore(icdata_a);
		intcode_memory__restore(icdata_b);
		intcode_memory__restore(icdata_c);
		intcode_memory__restore(icdata_d);
		intcode_memory__restore(icdata_e);

	}



	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "DESC" WHITE "\t\t: " CYAN "%d\n" RESET, max);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "DESC" WHITE "\t\t: " CYAN "VAL\n" RESET);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
