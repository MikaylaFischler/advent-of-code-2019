#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include <string.h>

#include "main.h"

#define CHECK_NONE 0
#define CHECK_POINT 1
#define CHECK_TRAVERSE_X 2
#define CHECK_TRAVERSE_Y 3

typedef struct coord {
	int16_t x;
	int16_t y;
} coord_t;

typedef struct mpoint {
	coord_t point;
	int16_t m_dist;
} mpoint_t;

typedef struct vector {
	coord_t c1;
	coord_t c2;
	char dir;
} vect_t;

typedef struct isect {
	uint16_t w1_idx;
	uint16_t w2_idx;
	coord_t point;
} isect_t;

uint16_t decode_length(vect_t v) {
	if (v.dir == 'U' || v.dir == 'D') {
		return abs(v.c1.y - v.c2.y);
	} else {
		return abs(v.c1.x - v.c2.x);
	}
}

uint16_t manhatten_dist(coord_t c1, coord_t c2) {
	return abs(c2.x - c1.x) + abs(c2.y - c1.y);
}

uint16_t manhatten_dist_center(coord_t c1) {
	return abs(c1.x) + abs(c1.y);
}

uint32_t trace_wire_length(isect_t* isect) {

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
	clock_t time_start, time_end;

	printf(YELLOW "starting...\n" RESET);

	vect_t** vects;
	vects = calloc(sizeof(vect_t*), 1000);
	for (int i = 0; i < 1000; i++) {
		vects[i] = calloc(sizeof(vect_t), 1000);
	}

	uint16_t num_vects[2] = { 0, 0 };
	uint8_t wire = 0;

	// parse in vectors
	while ((num_read = getline(&line, &len, fp)) != -1) {
		int16_t cur_x = 0;
		int16_t cur_y = 0;
		uint16_t i = 0;

		char* token = strtok(line, ",");

		while (token != NULL) {
			char dir = token[0];
			int16_t val = atoi(token + 1);

			printf("%c %4d => ", dir, val);

			vects[wire][i].dir = dir;

			switch (dir) {
				case 'R':
					vects[wire][i].c1.x = cur_x;
					vects[wire][i].c1.y = cur_y;
					vects[wire][i].c2.x = cur_x + val;
					vects[wire][i].c2.y = cur_y;
					printf("%4d\n", vects[wire][i].c2.x - vects[wire][i].c1.x);
					printf("\t%d\n", decode_length(vects[wire][i]));
					cur_x += val;
					break;
				case 'D':
					vects[wire][i].c1.x = cur_x;
					vects[wire][i].c1.y = cur_y;
					vects[wire][i].c2.x = cur_x;
					vects[wire][i].c2.y = cur_y - val;
					printf("%4d\n", vects[wire][i].c1.y - vects[wire][i].c2.y);
					printf("\t%d\n", decode_length(vects[wire][i]));
					cur_y -= val;
					break;
				case 'L':
					vects[wire][i].c1.x = cur_x;
					vects[wire][i].c1.y = cur_y;
					vects[wire][i].c2.x = cur_x - val;
					vects[wire][i].c2.y = cur_y;
					printf("%4d\n", vects[wire][i].c1.x - vects[wire][i].c2.x);
					printf("\t%d\n", decode_length(vects[wire][i]));
					cur_x -= val;
					break;
				case 'U':
					vects[wire][i].c1.x = cur_x;
					vects[wire][i].c1.y = cur_y;
					vects[wire][i].c2.x = cur_x;
					vects[wire][i].c2.y = cur_y + val;
					printf("%4d\n", vects[wire][i].c2.y - vects[wire][i].c1.y);
					printf("\t%d\n", decode_length(vects[wire][i]));
					cur_y += val;
					break;
				default:
					printf("error\n");
					exit(0);
			}

			i++;
			token = strtok(NULL, ",");
		}

		num_vects[wire] = i;
		if (wire == 1) { break;}

		wire++;
	}


	time_start = clock();
	mpoint_t best_point = { .point = { .x = 0, .y = 0}, .m_dist = 10000 };

	isect_t* isects = malloc(sizeof(isect_t) * 1000);
	uint16_t num_isects = 0;

	// allocate working variables
	int16_t isect_start = 0;
	int16_t isect_end = 0;
	int16_t isect_swp = 0;
	int16_t x = 0;
	int16_t y = 0;
	int16_t _x1, _x2, _y1, _y2;

	uint8_t check_operation = 0;

	coord_t cur_point;
	uint16_t m_dist;

	for (uint16_t i = 0; i < num_vects[0]; i++) {
		for (uint16_t j = 0; j < num_vects[1]; j++) {
			vect_t a = vects[0][i];
			vect_t b = vects[1][j];

			check_operation = CHECK_NONE;

			switch (a.dir) {
				case 'R':
					if (b.dir == 'D' || b.dir == 'U') {
						// can have only one intersection, but can they?
						// does a cross b's x position?
						if (a.c1.x <= b.c1.x && a.c2.x >= b.c2.x) {
							// does b cross a's y position?
							_y1 = (b.dir == 'D') ? b.c1.y : b.c2.y;
							_y2 = (b.dir == 'D') ? b.c2.y : b.c1.y;

							if (_y1 >= a.c1.y && _y2 <= a.c1.y) {
								// calculate intersection
								x = b.c1.x;
								y = a.c1.y;
								check_operation = CHECK_POINT;
							}
						}
					} else {
						// // can have more than one intersection
						// // if on same y axis, otherwise they can't intersect
						// if (a.c1.y == b.c1.y) {
						// 	// start at overlap point
						// 	isect_start = (a.c1.x < b.c1.x) ? b.c1.x : a.c1.x;

						// 	// go till end of overlap point
						// 	isect_end = (a.c2.x < b.c2.x) ? b.c2.x : a.c2.x;

						// 	// if b is left, we need to swap
						// 	if (isect_start > isect_end) {
						// 		isect_swp = isect_start;
						// 		isect_start = isect_end;
						// 		isect_end = isect_swp;
						// 	}

						// 	// set traversal
						// 	// check_operation = CHECK_TRAVERSE_X;
						// 	y = a.c1.y;
						// }
					}
					break;
				case 'D':
					if (b.dir == 'L' || b.dir == 'R') {
						// can have only one intersection, but can they?
						// does a cross b's y position?
						if (a.c2.y <= b.c1.y && a.c1.y >= b.c1.y) {
							// does b cross a's x position?
							_x1 = (b.dir == 'L') ? b.c1.x : b.c2.x;
							_x2 = (b.dir == 'L') ? b.c2.x : b.c1.x;

							if (_x1 >= a.c1.x && _x2 <= a.c1.x) {
								// calculate intersection
								x = a.c1.x;
								y = b.c1.y;
								check_operation = CHECK_POINT;
							}
						}
					} else {
						// // can have more than one intersection
						// // if on same x axis, otherwise they can't intersect
						// if (a.c1.x == b.c1.x) {
						// 	// start at overlap point
						// 	isect_start = (a.c2.y < b.c1.y) ? b.c1.y : a.c2.y;

						// 	// go till end of overlap point
						// 	isect_end = (a.c1.y < b.c2.y) ? b.c2.y : a.c1.y;

						// 	// if b is left, we need to swap
						// 	if (isect_start > isect_end) {
						// 		isect_swp = isect_start;
						// 		isect_start = isect_end;
						// 		isect_end = isect_swp;
						// 	}

						// 	// set traversal
						// 	// check_operation = CHECK_TRAVERSE_Y;
						// 	x = b.c1.x;
						// }
					}
					break;
				case 'L':
					if (b.dir == 'D' || b.dir == 'U') {
						// can have only one intersection, but can they?
						// does a cross b's x position?
						if (a.c2.x <= b.c1.x && a.c1.x >= b.c2.x) {
							// does b cross a's y position?
							_y1 = (b.dir == 'D') ? b.c1.y : b.c2.y;
							_y2 = (b.dir == 'D') ? b.c2.y : b.c1.y;

							if (_y1 >= a.c1.y && _y2 <= a.c1.y) {
								// calculate intersection
								x = b.c1.x;
								y = a.c1.y;
								check_operation = CHECK_POINT;
							}
						}
					} else {
						// // can have more than one intersection
						// // if on same y axis, otherwise they can't intersect
						// if (a.c1.y == b.c1.y) {
						// 	// start at overlap point
						// 	isect_start = (a.c2.x < b.c1.x) ? b.c1.x : a.c2.x;

						// 	// go till end of overlap point
						// 	isect_end = (a.c1.x < b.c2.x) ? b.c2.x : a.c1.x;

						// 	// if b is left, we need to swap
						// 	if (isect_start > isect_end) {
						// 		isect_swp = isect_start;
						// 		isect_start = isect_end;
						// 		isect_end = isect_swp;
						// 	}

						// 	// set traversal
						// 	// check_operation = CHECK_TRAVERSE_X;
						// 	y = a.c1.y;
						// }
					}
					break;
				case 'U':
					if (b.dir == 'L' || b.dir == 'R') {
						// can have only one intersection, but can they?
						// does a cross b's y position?
						if (a.c1.y <= b.c1.y && a.c2.y >= b.c1.y) {
							// does b cross a's x position?
							_x1 = (b.dir == 'L') ? b.c1.x : b.c2.x;
							_x2 = (b.dir == 'L') ? b.c2.x : b.c1.x;

							if (_x1 >= a.c1.x && _x2 <= a.c1.x) {
								// calculate intersection
								x = a.c1.x;
								y = b.c1.y;
								check_operation = CHECK_POINT;
							}
						}
					} else {
						// // can have more than one intersection
						// // if on same x axis, otherwise they can't intersect
						// if (a.c1.x == b.c1.x) {
						// 	// start at overlap point
						// 	isect_start = (a.c1.y < b.c1.y) ? b.c1.y : a.c1.y;

						// 	// go till end of overlap point
						// 	isect_end = (a.c2.y < b.c2.y) ? b.c2.y : a.c2.y;

						// 	// if b is left, we need to swap
						// 	if (isect_start > isect_end) {
						// 		isect_swp = isect_start;
						// 		isect_start = isect_end;
						// 		isect_end = isect_swp;
						// 	}

						// 	// set traversal
						// 	// check_operation = CHECK_TRAVERSE_Y;
						// 	x = b.c1.x;
						// }
					}
					break;
			}

			switch (check_operation) {
				case CHECK_POINT:
					if (x == 0 && y == 0) { continue; }

					isects[num_isects].point.x = x;
					isects[num_isects].point.y = y;
					isects[num_isects].w1_idx = i;
					isects[num_isects].w2_idx = j;
					num_isects++;
					
					cur_point.x = x;
					cur_point.y = y;
					m_dist = manhatten_dist_center(cur_point);

					if (m_dist < best_point.m_dist) {
						printf("[POINT] found new best intersection (%d,%d) has dist (%d)\n", x, y, m_dist);
						best_point.point.x = x;
						best_point.point.y = y;
						best_point.m_dist = m_dist;
					}
					break;
				case CHECK_TRAVERSE_X:
					// go across line
					printf("%d, %d\n", isect_start, isect_end);
					for (int16_t m = isect_start; m <= isect_end; m++) {
						if (m == 0 && y == 0) { continue; }
						
						cur_point.x = m;
						cur_point.y = y;

						m_dist = manhatten_dist_center(cur_point);

						// printf("[TRAVERSE_X] found intersection (%d,%d) has dist (%d)\n", m, y, m_dist);

						if (m_dist < best_point.m_dist) {
							printf("[TRAVERSE_X] found new best intersection (%d,%d) has dist (%d)\n", m, y, m_dist);
							best_point.point.x = m;
							best_point.point.y = y;
							best_point.m_dist = m_dist;
						}
					}
					break;
				case CHECK_TRAVERSE_Y:
					// go across line
					printf("%d, %d\n", isect_start, isect_end);
					for (int16_t m = isect_start; m <= isect_end; m++) {
						if (m == 0 && x == 0) { continue; }

						cur_point.x = x;
						cur_point.y = m;

						m_dist = manhatten_dist_center(cur_point);

						// printf("[TRAVERSE_Y] found intersection (%d,%d) has dist (%d)\n", x, m, m_dist);

						if (m_dist < best_point.m_dist) {
							printf("[TRAVERSE_Y] found new best intersection (%d,%d) has dist (%d)\n", x, m, m_dist);
							best_point.point.x = x;
							best_point.point.y = m;
							best_point.m_dist = m_dist;
						}
					}
					break;
				case CHECK_NONE:
				default:
					break;
			}
		}
	}


	uint32_t* m_dist_cache = calloc(sizeof(uint32_t), num_isects);

	uint32_t min_len_sum = -1;
	
	for (uint16_t i = 0; i < num_isects; i++) {
		isect_t isect = isects[i];

		uint32_t w1_sum = manhatten_dist(isect.point, vects[0][isect.w1_idx].c1);
		for (int16_t j = isect.w1_idx - 1; j >= 0; j--) {
			w1_sum += decode_length(vects[0][j]);
		}

		uint32_t w2_sum = manhatten_dist(isect.point, vects[1][isect.w2_idx].c1);
		for (int16_t j = isect.w2_idx - 1; j >= 0; j--) {
			w2_sum += decode_length(vects[1][j]);
		}

		if (w1_sum + w2_sum < min_len_sum) {
			min_len_sum = w1_sum + w2_sum;
		}
	}

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "DESC" WHITE "\t\t: " CYAN "VAL\n" RESET);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "DESC" WHITE "\t\t: " CYAN "%d\n" RESET, min_len_sum);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
