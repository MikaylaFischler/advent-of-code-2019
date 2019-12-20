#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"
#include "../../lib/intcode/intcode.h"

#define OUTPUT_X 0
#define OUTPUT_Y 1
#define OUTPUT_D 2

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 13" WHITE "           |\n" RESET);
    printf(WHITE "x------------------------------------------------x\n\n" RESET);

	// file i/o variables
    FILE* fp;

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

	// timing
	clock_t time_start, time_end, parse_end, p1_start, p1_end, p2_start, p2_end;

	printf(YELLOW "starting...\n" RESET);
	printf(YELLOW ">" B_WHITE " parsing intcode and allocating memory...\n" RESET);

	time_start = clock();

	uint16_t blocks = 0;

	// create intcode program
	icd_t* icdata = intcode_init(1, 1, IC_QUIET);

	// load memory
	intcode_memory__load_file(fp, icdata);
	intcode_memory__backup(icdata);
	intcode_buffer__set_mode_stream(icdata->inbuf, 0);
	intcode_buffer__set_mode_stream(icdata->outbuf, 0);
	intcode_buffer__set_halt_on_stream_write(icdata->outbuf, 0);

	// free up file
	fclose(fp);

	// core running variables
	uint8_t exit_code = EXIT__OUTPUT_READY;
	uint8_t output_type = 0;

	// game variables
	int64_t val = 0;
	uint32_t score = 0;
	int8_t action = 0;
	int8_t tmp_x = 0;
	int8_t paddle_x = 0;
	int8_t ball_x = 0;

	parse_end = clock();
	printf(B_GREEN ">" B_WHITE " intcode parsed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (parse_end - time_start) * 100000.0 / CLOCKS_PER_SEC, parse_end - time_start);
	printf(YELLOW ">" B_WHITE " determining number of blocks present...\n" RESET);
	p1_start = clock();

	while (exit_code != EXIT__NORMAL) {
		exit_code = intcode_compute(icdata);
		if (output_type == OUTPUT_D && intcode_buffer__read_stream(icdata->outbuf, 0) == 2) {
			blocks++;
		}

		if (++output_type > OUTPUT_D) { output_type = OUTPUT_X; }
	}

	p1_end = clock();
	printf(B_GREEN ">" B_WHITE " blocks counted " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p1_end - p1_start) * 100000.0 / CLOCKS_PER_SEC, p1_end - p1_start);

	// restore intcode program...? nope! resetting the program counter seems to kill the program? (maybe I'm not supposed to...)
	// intcode_memory__restore(icdata);
	// intcode_compute__init(icdata);

	// insert coins
	icdata->memory[0] = 2;

	printf(YELLOW ">" B_WHITE " playing game...\n" RESET);
	p2_start = clock();
	
	// play game
	exit_code = EXIT__OUTPUT_READY;
	while (exit_code != EXIT__NORMAL) {
		exit_code = intcode_compute(icdata);
		if (exit_code == EXIT__MISSING_INPUT) {
			// determine where to move
			if (paddle_x < ball_x) {
				action = 1;
			} else if (paddle_x > ball_x) {
				action = -1;
			} else { action = 0; }

			// move the paddle
			intcode_buffer__set(icdata->inbuf, 0, action);
		} else if (exit_code == EXIT__OUTPUT_READY) {
			// get output
			val = intcode_buffer__read_stream(icdata->outbuf, 0);

			// interpret output
			if (output_type == OUTPUT_X) {
				tmp_x = val;
			} else if (output_type == OUTPUT_D) {
				if (tmp_x == -1) {
					score = val;
				} else if (val == 3) {
					paddle_x = tmp_x;
				} else if (val == 4) {
					ball_x = tmp_x;
				}
			}

			// keep track of what the output is for (x, y, or data)
			if (++output_type > OUTPUT_D) { output_type = OUTPUT_X; }
		}
	}

	p2_end = clock();
	printf(B_GREEN ">" B_WHITE " game won! " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (p2_end - p2_start) * 1000.0 / CLOCKS_PER_SEC);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "blocks" WHITE "\t\t: " CYAN "%d\n" RESET, blocks);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "winning score" WHITE "\t: " CYAN "%d\n" RESET, score);

	return 0;
}
