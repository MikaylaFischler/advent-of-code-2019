#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"
#include "../../lib/intcode/intcode.h"

#define MAP_W 80
#define MAP_H 60

#define M_NORTH	1
#define M_SOUTH	2
#define M_WEST	3
#define M_EAST	4

#define S_WALL	0
#define S_MOVED	1
#define S_FOUND	2

#define C_UNKNOWN 0
#define C_VISITED 1
#define C_WALL 2
#define C_OXYGEN 3
#define C_START 4

uint8_t __move(icd_t* icdata, uint8_t direction) {
	intcode_buffer__set(icdata->inbuf, 0, direction);

	if (intcode_compute(icdata) != EXIT__OUTPUT_READY) { 
		printf(RED ">" B_WHITE " unexpected intcode exit status\n" RESET);
		exit(0);
	}

	return intcode_buffer__read_stream(icdata->outbuf, 0);
}

void print_map(uint8_t** map) {
	for (uint16_t y = 0; y < MAP_H; y++) {
		for (uint16_t x = 0; x < MAP_W; x++) {
			switch (map[y][x]) {
				case C_UNKNOWN:
					printf("?");
					break;
				case C_VISITED:
					printf(".");
					break;
				case C_WALL:
					printf("#");
					break;
				case C_OXYGEN:
					printf("O");
					break;
				case C_START:
					printf("X");
					break;
			}
		}
		printf("\n");
	}
	printf("\n");
}

void print_cost_map(uint8_t** map, uint16_t** cost_map) {
	for (uint16_t y = 0; y < MAP_H; y++) {
		for (uint16_t x = 0; x < MAP_W; x++) {
			if (cost_map[y][x] == UINT16_MAX) {
				if (map[y][x] == C_OXYGEN) {
					printf(GREEN "[ O ]" RESET);
				} else {
					printf("[   ]");
				}
			} else {
				if (map[y][x] == C_OXYGEN) {
					printf(GREEN "[%3d]" RESET, cost_map[y][x]);
				} else {
					printf("[%3d]", cost_map[y][x]);
				}
			}
		}
		printf("\n");
	}
	printf("\n");
}

void explore(icd_t* icdata, uint8_t** map, uint16_t x, uint16_t y) {
	// north
	if (map[y - 1][x] == C_UNKNOWN) {
		switch (__move(icdata, M_NORTH)) {
			case S_WALL:
				map[y - 1][x] = C_WALL;
				break;
			case S_MOVED:
				map[y - 1][x] = C_VISITED;
				explore(icdata, map, x, y - 1);
				__move(icdata, M_SOUTH);
				break;
			case S_FOUND:
				map[y - 1][x] = C_OXYGEN;
				__move(icdata, M_SOUTH);
				break;
		}
	}

	// south
	if (map[y + 1][x] == C_UNKNOWN) {
		switch (__move(icdata, M_SOUTH)) {
			case S_WALL:
				map[y + 1][x] = C_WALL;
				break;
			case S_MOVED:
				map[y + 1][x] = C_VISITED;
				explore(icdata, map, x, y + 1);
				__move(icdata, M_NORTH);
				break;
			case S_FOUND:
				map[y + 1][x] = C_OXYGEN;
				__move(icdata, M_NORTH);
				break;
		}
	}

	// west
	if (map[y][x - 1] == C_UNKNOWN) {
		switch (__move(icdata, M_WEST)) {
			case S_WALL:
				map[y][x - 1] = C_WALL;
				break;
			case S_MOVED:
				map[y][x - 1] = C_VISITED;
				explore(icdata, map, x - 1, y);
				__move(icdata, M_EAST);
				break;
			case S_FOUND:
				map[y][x - 1] = C_OXYGEN;
				__move(icdata, M_EAST);
				break;
		}
	}

	// east
	if (map[y][x + 1] == C_UNKNOWN) {
		switch (__move(icdata, M_EAST)) {
			case S_WALL:
				map[y][x + 1] = C_WALL;
				break;
			case S_MOVED:
				map[y][x + 1] = C_VISITED;
				explore(icdata, map, x + 1, y);
				__move(icdata, M_WEST);
				break;
			case S_FOUND:
				map[y][x + 1] = C_OXYGEN;
				__move(icdata, M_WEST);
				break;
		}
	}
}

void populate_costs(uint8_t** map, uint16_t** cost_map, uint16_t x, uint16_t y, uint16_t c) {
	c++;

	// north
	if (map[y - 1][x] == C_VISITED) {
		if (c < cost_map[y - 1][x]) {
			cost_map[y - 1][x] = c;
			populate_costs(map, cost_map, x, y - 1, c);
		}
	} else if (map[y - 1][x] == C_OXYGEN) {
		if (c < cost_map[y - 1][x]) { cost_map[y - 1][x] = c; }
	}

	// south
	if (map[y + 1][x] == C_VISITED) {
		if (c < cost_map[y + 1][x]) {
			cost_map[y + 1][x] = c;
			populate_costs(map, cost_map, x, y + 1, c);
		}
	} else if (map[y + 1][x] == C_OXYGEN) {
		if (c < cost_map[y + 1][x]) { cost_map[y + 1][x] = c; }
	}

	// west
	if (map[y][x - 1] == C_VISITED) {
		if (c < cost_map[y][x - 1]) {
			cost_map[y][x - 1] = c;
			populate_costs(map, cost_map, x - 1, y, c);
		}
	} else if (map[y][x - 1] == C_OXYGEN) {
		if (c < cost_map[y][x - 1]) { cost_map[y][x - 1] = c; }
	}
	
	// east
	if (map[y][x + 1] == C_VISITED) {
		if (c < cost_map[y][x + 1]) {
			cost_map[y][x + 1] = c;
			populate_costs(map, cost_map, x + 1, y, c);
		}
	} else if (map[y][x + 1] == C_OXYGEN) {
		if (c < cost_map[y][x + 1]) { cost_map[y][x + 1] = c; }
	}
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 15" WHITE "           |\n" RESET);
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

	// allocate maps
	uint8_t** map = malloc(sizeof(uint8_t*) * MAP_H);
	uint16_t** cost_map = malloc(sizeof(uint16_t*) * MAP_H);

	for (uint16_t i = 0; i < MAP_H; i++) { 
		map[i] = calloc(sizeof(uint8_t), MAP_W);
		cost_map[i] = malloc(sizeof(uint16_t) * MAP_W);

		for (uint16_t j = 0; j < MAP_W; j++) { cost_map[i][j] = UINT16_MAX; }
	}

	// create intcode program
	icd_t* icdata = intcode_init(1, 1, IC_QUIET);

	// load memory and configure program
	intcode_memory__load_file(fp, icdata);
	intcode_memory__backup(icdata);
	intcode_buffer__set_mode_stream(icdata->inbuf, 0);
	intcode_buffer__set_mode_stream(icdata->outbuf, 0);
	intcode_buffer__set_halt_on_stream_write(icdata->outbuf, 0);
	
	// free up memory
	fclose(fp);

	parse_end = clock();
	printf(B_GREEN ">" B_WHITE " intcode parsed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (parse_end - time_start) * 100000.0 / CLOCKS_PER_SEC, parse_end - time_start);
	printf(YELLOW ">" B_WHITE " determining minimum steps to O2 system...\n" RESET);

	p1_start = clock();

	uint16_t start_x = MAP_W/2;
	uint16_t start_y = MAP_H/2;

	// explore
	printf(YELLOW ">" WHITE " exploring map...\n" RESET);
	map[start_y][start_x] = C_START;
	explore(icdata, map, start_x, start_y);
	// print_map(map);

	// find costs
	printf(YELLOW ">" WHITE " finding costs...\n" RESET);
	map[start_y][start_x] = 0;
	populate_costs(map, cost_map, start_x, start_y, 0);
	// print_cost_map(map, cost_map);

	// find cost to O2 system
	uint16_t min_steps = 0;
	for (uint8_t y = 0; y < MAP_H; y++) {
		for (uint8_t x = 0; x < MAP_W; x++) {
			if (map[y][x] == C_OXYGEN) {
				min_steps = cost_map[y][x];
			}
		}
	}

	p1_end = clock();
	printf(B_GREEN ">" B_WHITE " found minimum steps to oxygen system " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (p1_end - p1_start) * 100.0 / CLOCKS_PER_SEC);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "steps to O2" WHITE "\t: " CYAN "%d\n" RESET, min_steps);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "DESC" WHITE "\t\t: " CYAN "VAL\n" RESET);

	return 0;
}
