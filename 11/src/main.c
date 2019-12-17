#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"
#include "../../lib/intcode/intcode.h"

#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3

/**
 * @brief paint the ship
 * @param icdata intcode data
 * @param tiles tiles of ship
 * @param min_x min x bound to update
 * @param max_x max x bound to update
 * @param min_y min y bound to update
 * @param max_y max y bound to update
 * @return uint16_t unique tiles painted
 */
uint16_t paint_ship(icd_t* icdata, uint8_t** tiles, uint8_t starting_tile, uint16_t* min_x, uint16_t* max_x, uint16_t* min_y, uint16_t* max_y) {
	uint16_t tiles_painted = 0;
	uint8_t run = EXIT__MISSING_INPUT;
	uint8_t dir = 0;
	uint8_t turn = 0;
	uint16_t x = 250;
	uint16_t y = 250;

	intcode_memory__restore(icdata);
	intcode_compute__init(icdata);
	intcode_buffer__set_mode_stream(icdata->inbuf, 0);
	intcode_buffer__set(icdata->inbuf, 0, starting_tile);
	// tiles[y][x] = starting_tile + 1;

	while (run == EXIT__MISSING_INPUT) {
		run = intcode_compute(icdata);
		
		// exit on completion if no new output
		if (run == EXIT__NORMAL && icdata->outbuf->b_idx == 0) { break; }

		// update bounds
		if (x < *min_x) { *min_x = x; }
		if (x > *max_x) { *max_x = x; }
		if (y < *min_y) { *min_y = y; }
		if (y > *max_y) { *max_y = y; }
	
		// update tiles painted
		tiles_painted += (tiles[y][x] == 0);
		tiles[y][x]	= intcode_buffer__get(icdata->outbuf, 0) + 1;
		turn 		= intcode_buffer__get(icdata->outbuf, 1);
		intcode_buffer__rewind(icdata->outbuf);

		// update direction
		switch (dir) {
			case DIR_UP:
				if (turn == 0) {
					dir = DIR_LEFT;
				} else { dir = DIR_RIGHT; }
				break;
			case DIR_RIGHT:
				if (turn == 0) {
					dir = DIR_UP;
				} else { dir = DIR_DOWN; }
				break;
			case DIR_DOWN:
				if (turn == 0) {
					dir = DIR_RIGHT;
				} else { dir = DIR_LEFT; }
				break;
			case DIR_LEFT:
				if (turn == 0) {
					dir = DIR_DOWN;
				} else { dir = DIR_UP; }
				break;
		}

		// move robot
		switch (dir) {
			case DIR_UP:
				y--;
				break;
			case DIR_RIGHT:
				x++;
				break;
			case DIR_DOWN:
				y++;
				break;
			case DIR_LEFT:
				x--;
				break;
		}

		// report current tile color
		if (tiles[y][x] > 0) {
			intcode_buffer__set(icdata->inbuf, 0, tiles[y][x] - 1);
		} else {
			intcode_buffer__set(icdata->inbuf, 0, tiles[y][x]);
		}
	}

	return tiles_painted;
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 11" WHITE "           |\n" RESET);
    printf(WHITE "x------------------------------------------------x\n\n" RESET);

	// file i/o variables
    FILE* fp;

	// open file
    fp = fopen("./input.txt", "r");
    if (fp == NULL) { return -1; }

	// timing
	clock_t time_start, time_end, parse_end, p1_start, p1_end, p2_start, p2_end;;

	printf(YELLOW "starting...\n" RESET);
	printf(YELLOW ">" B_WHITE " parsing intcode and allocating memory...\n" RESET);

	time_start = clock();

	int16_t p1;
	uint8_t** tiles = calloc(sizeof(uint8_t*), 500);
	for (uint16_t i = 0; i < 500; i++) { tiles[i] = calloc(sizeof(uint8_t), 500); }

	// create intcode program
	icd_t* icdata = intcode_init(1, 2, IC_QUIET);

	// load memory
	intcode_memory__load_file(fp, icdata);

	// free up file
	fclose(fp);

	// backup memory
	intcode_memory__backup(icdata);

	parse_end = clock();
	printf(B_GREEN ">" B_WHITE " intcode parsed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (parse_end - time_start) * 100000.0 / CLOCKS_PER_SEC, parse_end - time_start);
	printf(YELLOW ">" B_WHITE " determining number of squares to be painted...\n" RESET);
	p1_start = clock();

	// bounds
	uint16_t min_x = 500;
	uint16_t min_y = 500;
	uint16_t max_x = 0;
	uint16_t max_y = 0;

	// calculate part 1
	p1 = paint_ship(icdata, tiles, 0, &min_x, &max_x, &min_y, &max_y);

	p1_end = clock();
	printf(B_GREEN ">" B_WHITE " painted squares counted " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p1_end - p1_start) * 100000.0 / CLOCKS_PER_SEC, p1_end - p1_start);

	// reset tiles
	for (uint16_t j = 0; j < 500; j++) {
		for (uint16_t i = 0; i < 500; i++) {
			tiles[j][i] = 0;
		}
	}

	// reset bounds
	min_x = 500;
	min_y = 500;
	max_x = 0;
	max_y = 0;

	printf(YELLOW ">" B_WHITE " painting ship to specification...\n" RESET);
	p2_start = clock();

	// calculate part 2
	paint_ship(icdata, tiles, 1, &min_x, &max_x, &min_y, &max_y);

	p2_end = clock();
	printf(B_GREEN ">" B_WHITE " ship painted, printing result... " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (p2_end - p2_start) * 1000.0 / CLOCKS_PER_SEC);

	// print part 2
	for (uint16_t j = min_y - 2; j <= max_y + 2; j++) {
		for (uint16_t i = min_x - 8; i <= max_x + 8; i++) {
			if (tiles[j][i] == 1) {
				printf(BLACK_BG " " RESET);
			} else if (tiles[j][i] == 2) {
				printf(WHITE_BG " " RESET);
			} else { printf(" "); }
		}
		printf("\n");
	}

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "# painted" WHITE "\t: " CYAN "%d\n" RESET, p1);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "marking" WHITE "\t: " CYAN "see above\n" RESET);

	return 0;
}
