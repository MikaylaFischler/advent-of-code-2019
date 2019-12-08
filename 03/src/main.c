#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>

#include "main.h"

#define CHECK_NONE 0
#define CHECK_POINT 1

typedef struct coord {
	int16_t x;
	int16_t y;
} coord_t;

typedef struct mpoint {
	coord_t point;
	uint32_t m_dist;
} mpoint_t;

typedef struct vector {
	coord_t c1;
	coord_t c2;
	uint16_t len;
	char dir;
} vect_t;

typedef struct isect {
	uint16_t w1_idx;
	uint16_t w2_idx;
	coord_t point;
} isect_t;

/**
 * @brief calculate the manhatten distance between c1 and c2
 * @param c1 coordinate 1
 * @param c2 coordinate 2
 * @return uint16_t manhatten distance (absolute value)
 */
uint16_t manhatten_dist(coord_t c1, coord_t c2) {
	return abs(c2.x - c1.x) + abs(c2.y - c1.y);
}

/**
 * @brief calculate the manhatten distance between c1 and (0, 0)
 * @param c1 coordinate
 * @return uint16_t manhatten distance (absolute value)
 */
uint16_t manhatten_dist_center(coord_t c1) {
	return abs(c1.x) + abs(c1.y);
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 03" WHITE "           |\n" RESET);
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
	clock_t time_start, time_end, init_end, p1_start, p1_end, p2_start, p2_end;

	printf(YELLOW "starting...\n" RESET);

	printf(YELLOW ">" B_WHITE " loading and translating file contents into data structure...\n" RESET);

	time_start = clock();

	// allocate array of vectors
	uint16_t num_vects[2] = { 0, 0 };
	vect_t** vects = malloc(sizeof(vect_t*) * 500);
	for (uint16_t i = 0; i < 500; i++) {
		vects[i] = malloc(sizeof(vect_t) * 500);
	}

	// allocate array to store intersections for use in part 2
	isect_t* isects = malloc(sizeof(isect_t) * 500);
	uint16_t num_isects = 0;

	// solution variables
	mpoint_t best_point = { .point = { .x = 0, .y = 0 }, .m_dist = -1 };
	uint32_t min_len_sum = -1;

	/* --- parse in vectors --- */
	
	uint16_t i;
	int16_t cur_x, cur_y, val;
	uint8_t wire = 0;
	char* token;
	char dir;

	while ((num_read = getline(&line, &len, fp)) != -1) {
		i = cur_x = cur_y = 0;

		token = strtok(line, ",");

		while (token != NULL) {
			dir = token[0];
			val = atoi(token + 1);

			vects[wire][i].len = val;
			vects[wire][i].dir = dir;

			vects[wire][i].c1.x = cur_x;
			vects[wire][i].c1.y = cur_y;

			switch (dir) {
				case 'R':
					vects[wire][i].c2.x = cur_x + val;
					vects[wire][i].c2.y = cur_y;
					cur_x += val;
					break;
				case 'D':
					vects[wire][i].c2.x = cur_x;
					vects[wire][i].c2.y = cur_y - val;
					cur_y -= val;
					break;
				case 'L':
					vects[wire][i].c2.x = cur_x - val;
					vects[wire][i].c2.y = cur_y;
					cur_x -= val;
					break;
				case 'U':
					vects[wire][i].c2.x = cur_x;
					vects[wire][i].c2.y = cur_y + val;
					cur_y += val;
					break;
				default:
					printf(B_RED ">" RESET " unknown direction \"%c\", exiting...\n", dir);
					exit(0);
			}

			i++;
			token = strtok(NULL, ",");
		}

		num_vects[wire] = i;
		wire++;
	}

	init_end = clock();
	printf(B_GREEN ">" B_WHITE " data structure loaded " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (init_end - time_start) * 1000.0 / CLOCKS_PER_SEC);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	/* --- find intersections --- */

	// allocate working variables
	uint8_t found_isect = 0;
	int16_t x = 0;
	int16_t y = 0;
	int16_t _x1, _x2, _y1, _y2;
	coord_t cur_point;
	uint16_t m_dist;

	printf(YELLOW ">" B_WHITE " searching for wire intersections...\n" RESET);

	p1_start = clock();

	for (uint16_t i = 0; i < num_vects[0]; i++) {
		for (uint16_t j = 0; j < num_vects[1]; j++) {
			vect_t a = vects[0][i];
			vect_t b = vects[1][j];
			found_isect = 0;

			if (a.dir == 'R' || a.dir == 'L') {
				// swap for right or left because >= and <=
				_x1 = (a.dir == 'R') ? a.c1.x : a.c2.x;
				_x2 = (a.dir == 'R') ? a.c2.x : a.c1.x;

				if (b.dir == 'D' || b.dir == 'U') {
					// can have an intersection
					// does a cross b's x position?
					if (_x1 <= b.c1.x && _x2 >= b.c2.x) {
						// does b cross a's y position?
						_y1 = (b.dir == 'D') ? b.c1.y : b.c2.y;
						_y2 = (b.dir == 'D') ? b.c2.y : b.c1.y;

						if (_y1 >= a.c1.y && _y2 <= a.c1.y) {
							// calculate intersection
							x = b.c1.x;
							y = a.c1.y;
							found_isect = 1;
						}
					}
				}
			} else {
				// swap for up or down because >= and <=
				_y1 = (a.dir == 'U') ? a.c1.y : a.c2.y;
				_y2 = (a.dir == 'U') ? a.c2.y : a.c1.y;

				if (b.dir == 'L' || b.dir == 'R') {
					// can have an intersection
					// does a cross b's y position?
					if (_y1 <= b.c1.y && _y2 >= b.c1.y) {
						// does b cross a's x position?
						_x1 = (b.dir == 'L') ? b.c1.x : b.c2.x;
						_x2 = (b.dir == 'L') ? b.c2.x : b.c1.x;

						if (_x1 >= a.c1.x && _x2 <= a.c1.x) {
							// calculate intersection
							x = a.c1.x;
							y = b.c1.y;
							found_isect = 1;
						}
					}
				}
			}

			// if we found an intersection
			if (found_isect) {
				// ignore start
				if (x == 0 && y == 0) { continue; }

				// record intersection
				isects[num_isects].point.x = x;
				isects[num_isects].point.y = y;
				isects[num_isects].w1_idx = i;
				isects[num_isects].w2_idx = j;
				num_isects++;
				
				// create point to calc with
				cur_point.x = x;
				cur_point.y = y;
				m_dist = manhatten_dist_center(cur_point);

				// compare against current closest
				if (m_dist < best_point.m_dist) {
					printf(BLUE ">" RESET " found new best intersection (%d,%d) with distance (%d)\n", x, y, m_dist);
					best_point.point.x = x;
					best_point.point.y = y;
					best_point.m_dist = m_dist;
				}
			}
		}
	}

	p1_end = clock();
	printf(B_GREEN ">" B_WHITE " all intersections found " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p1_end - p1_start) * 100000.0 / CLOCKS_PER_SEC, p1_end - p1_start);

	/* --- compute part 2 --- */

	uint32_t len_sum = 0;

	printf(YELLOW ">" B_WHITE " searching for shortest combined wire path to intersection...\n" RESET);

	p2_start = clock();
	
	for (uint16_t i = 0; i < num_isects; i++) {
		isect_t isect = isects[i];

		// add up all of wire 1 up untill the intersection
		len_sum = manhatten_dist(isect.point, vects[0][isect.w1_idx].c1);
		for (int16_t j = isect.w1_idx - 1; j >= 0; j--) {
			len_sum += vects[0][j].len;
		}

		// add up all of wire 2 up untill the intersection
		len_sum += manhatten_dist(isect.point, vects[1][isect.w2_idx].c1);
		for (int16_t j = isect.w2_idx - 1; j >= 0; j--) {
			len_sum += vects[1][j].len;
		}

		// compare against shortest sum
		if (len_sum < min_len_sum) {
			min_len_sum = len_sum;
		}
	}
	
	p2_end = clock();
	printf(B_GREEN ">" B_WHITE " shortest path found " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p2_end - p2_start) * 100000.0 / CLOCKS_PER_SEC, p2_end - p2_start);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "closest intersection" WHITE "\t: " CYAN "%d\n" RESET, best_point.m_dist);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "shortest path" WHITE "\t\t: " CYAN "%d\n" RESET, min_len_sum);

	return 0;
}
