#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>

#include "main.h"

typedef struct asteroid {
	int16_t x;
	int16_t y;
	int16_t dx;
	int16_t dy;
	int16_t delta_hash;
	double angle;
	uint16_t dist;
	uint8_t destroyed;
} asteroid_t;

/**
 * @brief reduce a d_x and d_y using a common divisor
 * 
 * @param d_x Pointer to change in x
 * @param d_y Pointer to change in y
 */
void reduce_deltas(int16_t* d_x, int16_t* d_y) {
	int16_t gcd, r;
	int16_t d_x_ = *d_x;
	int16_t d_y_ = *d_y;

	// catch zero cases
	if (d_x_ == 0 && d_y_ == 0) { return; }
	if (d_x_ == 0) { *d_y = (d_y_ > 0) ? 1 : -1; return; }
	if (d_y_ == 0) { *d_x = (d_x_ > 0) ? 1 : -1; return; }

	// find gcd
	while (d_x_ != 0) {
		r = d_y_ % d_x_;
		d_y_ = d_x_;
		d_x_ = r;
	}

	gcd = abs(d_y_);

	// reduce deltas
	*d_x = *d_x / gcd;
	*d_y = *d_y / gcd;
}

uint16_t find_all_visible(uint8_t j, uint8_t i, uint8_t** map, uint8_t width, uint8_t height) {
	uint16_t count = 0;
	uint8_t blocked = 0;
	uint8_t run = 1;
	int16_t d_x, a;
	int16_t d_y, b;

	for (uint8_t y = 0; y < height; y++) {
		for (uint8_t x = 0; x < width; x++) {
			if (!(x == i && y == j) && map[y][x]) {
				blocked = 0;

				// find and simplify slope
				d_x = x - i;
				d_y = y - j;

				reduce_deltas(&d_x, &d_y);

				// start at next possible cell
				a = d_x;
				b = d_y;

				run = 1;

				while (run) {
					if (map[b + j][a + i]) {
						blocked = !(b + j == y && a + i == x);
						break;
					}

					a += d_x;
					b += d_y;

					run = !(b + j == y && a + i == x);
					run &= b + j >= 0 && b + j < height;
					run &= a + i >= 0 && a + i < height;
				}

				count += !blocked;
			}
		}
	}

	return count;
}

int __asteroid_compar_fn(const void* a, const void* b) {
	asteroid_t* _a = (asteroid_t*) a;
	asteroid_t* _b = (asteroid_t*) b;
	
	// break ties with distance
	if (_a->dx == _b->dx && _a->dy == _b->dy) {
		return _a->dist > _b->dist;
	} else {
		return _a->angle > _b->angle;
	}
}

void catalog_asteroids(uint8_t station_x, uint8_t station_y, uint8_t** map, uint8_t width, uint8_t height, asteroid_t* catalog) {
	uint16_t i = 0;

	for (uint8_t y = 0; y < height; y++) {
		for (uint8_t x = 0; x < width; x++) {
			if (!(x == station_x && y == station_y) && map[y][x]) {
				// found an asteroid, catalog it
				asteroid_t* a = &catalog[i];
				a->x = x;
				a->y = y;
				a->dx = x - station_x;
				a->dy = y - station_y;
				reduce_deltas(&(a->dx), &(a->dy));

				a->angle = atan2((double) a->dy, (double) a->dx);
				a->delta_hash = a->dy + height + (100 * (a->dx + width));

				a->dist = abs(station_x - x) + abs(station_y - y);
				a->destroyed = 0;

				// printf("new asteroid: { x = %d, y = %d, dx = %d, dy = %d, angle = %lf, dist = %d, delta_hash = %d }\n", catalog[i].x, catalog[i].y, catalog[i].dx, catalog[i].dy, catalog[i].angle, catalog[i].dist, a->delta_hash);

				i++;
			}
		}
	}

	qsort(catalog, i, sizeof(asteroid_t), __asteroid_compar_fn);
}

asteroid_t* destroy_next_asteroid(uint16_t* __last_delta_hash, asteroid_t* catalog, uint16_t count) {
	uint16_t last_delta_hash = *__last_delta_hash;

	if (last_delta_hash == (uint16_t) -1) {
		double target_theta = atan2(0.0, -1.0);

		for (uint16_t i = 0; i < count; i++) {
			asteroid_t* a = &catalog[i];

			if ((a->dx == 0 && a->dy == -1) || (a->angle >= target_theta)) {
				// definitely found first
				*__last_delta_hash = a->delta_hash;
				a->destroyed = 1;
				return a;
			}
		}
	} else {
		uint8_t wrap = 1;
		uint8_t search = 0;

		while (wrap) {
			for (uint16_t i = 0; i < count; i++) {
				asteroid_t* a = &catalog[i];

				if (search == 1) {
					if (a->delta_hash != last_delta_hash && a->destroyed == 0) {
						*__last_delta_hash = a->delta_hash;
						a->destroyed = 1;
						return a;
					}
				} else if (search == 2) {
					if (a->destroyed == 0) {
						*__last_delta_hash = a->delta_hash;
						a->destroyed = 1;
						return a;
					}
				} else if (a->delta_hash == last_delta_hash) {
					search++;
				}
			}

			if (search == 0) { wrap = 0; }
		}
	}

	return NULL;
}

uint16_t destroy_and_report(asteroid_t* catalog, uint16_t num_asteroids, uint16_t ith_destroyed) {
	asteroid_t* last_destroyed = NULL;
	uint16_t last_delta_hash = -1;

	for (uint16_t i = 0; i < ith_destroyed; i++) {
		last_destroyed = destroy_next_asteroid(&last_delta_hash, catalog, num_asteroids);
		if (last_destroyed == NULL) { return -1; }
	}

	return (last_destroyed->x * 100) + last_destroyed->y;
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 10" WHITE "           |\n" RESET);
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

	uint8_t** map = malloc(sizeof(uint8_t*) * 50);
	for (uint8_t i = 0; i < 50; i++) { map[i] = malloc(sizeof(uint8_t) * 50); }

	uint16_t** counts = calloc(sizeof(uint16_t*), 50);
	for (uint16_t i = 0; i < 50; i++) { counts[i] = calloc(sizeof(uint16_t), 50); }

	uint16_t p1 = 0;
	uint16_t p2;
	uint16_t i = 0;
	uint16_t j = 0;

	while ((num_read = getline(&line, &len, fp)) != -1) {
		for (i = 0; line[i] != '\n' && line[i] != 0; i++) { map[j][i] = line[i] == '#'; }
		if (line[0] != '\n') { j++; }
	}

	uint16_t height = j;
	uint16_t width = i;
	uint16_t num_asteroids = 0;

	// find all visible at each asteroid
	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if (map[j][i]) {
				// is an asteroid
				num_asteroids++;
				counts[j][i] += find_all_visible(j, i, map, width, height);
			}
		}
	}

	uint8_t station_x = 0;
	uint8_t station_y = 0;

	// find maximum visible from any point
	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if (counts[j][i] > p1) { 
				station_x = i;
				station_y = j;
				p1 = counts[j][i]; 
			}
		}
	}

	// catalog asteroids
	num_asteroids -= 1; // station does not count
	asteroid_t* catalog = malloc(sizeof(asteroid_t) * num_asteroids);
	catalog_asteroids(station_x, station_y, map, width, height, catalog);

	// start the destruction
	p2 = destroy_and_report(catalog, num_asteroids, 200);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "max visible" WHITE "\t: " CYAN "%d\n" RESET, p1);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "200th destroyed" WHITE ": " CYAN "%d\n" RESET, p2);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
