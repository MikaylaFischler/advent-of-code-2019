#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>

#include "main.h"
#include "moons.h"

/**
 * @brief find the least common multiple of two positive integers
 * @param a a
 * @param b b
 * @return uint64_t least common multiple
 */
uint64_t lcm(uint64_t a, uint64_t b) {
	uint64_t r, gcd, _a, _b;
	_a = a;
	_b = b;

	// find gcd
	while (_a != 0) {
		r = _b % _a;
		_b = _a;
		_a = r;
	}

	gcd = _b;

	// find lcm
	return a * b / gcd;
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 12" WHITE "           |\n" RESET);
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
	clock_t time_start, time_end, parse_end, p1_start, p1_end, p2_start, p2_x_end, p2_y_start, p2_y_end, p2_z_start, p2_z_end, p2_f_start, p2_f_end;

	printf(YELLOW "starting...\n" RESET);
	printf(YELLOW ">" B_WHITE " parsing moons and allocating memory...\n" RESET);

	time_start = clock();

	uint32_t p1;
	uint64_t p2;

	moon_t* moons = calloc(sizeof(moon_t), NUM_MOONS);

	char parse_tmp[10];
	uint8_t reading = 0;
	uint8_t m = 0;
	uint8_t c = 0;
	uint8_t axis = 0;

	// parse in moons
	while ((num_read = getline(&line, &len, fp)) != -1) {
		for (int i = 0; i < num_read; i++) {
			if (line[i] == '=') {
				reading = 1;
			} else if (line[i] != ',' && line[i] != '>') {
				if (reading) {
					parse_tmp[c++] = line[i];
					parse_tmp[c] = '\0';
				}
			} else if (reading) {
				reading = c = 0;
				switch (axis) {
					case 0:
						moons[m].pos.x = atoi(parse_tmp);
						axis++;
						break;
					case 1:
						moons[m].pos.y = atoi(parse_tmp);
						axis++;
						break;
					case 2:
						moons[m++].pos.z = atoi(parse_tmp);
						axis = 0;
						break;
				}
			}
		}
	}

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	// store initial moon states
	moon_t* moons_initial = malloc(sizeof(moon_t) * NUM_MOONS);
	memcpy(moons_initial, moons, sizeof(moon_t) * 4);

	parse_end = clock();
	printf(B_GREEN ">" B_WHITE " moons parsed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (parse_end - time_start) * 100000.0 / CLOCKS_PER_SEC, parse_end - time_start);
	printf(YELLOW ">" B_WHITE " finding energy after 1000 steps...\n" RESET);
	p1_start = clock();

	for (uint16_t i = 0; i < 1000; i++) {
		apply_gravity(moons);
		update_positions(moons);
	}

	p1 = total_system_energy(moons);

	p1_end = clock();
	printf(B_GREEN ">" B_WHITE " total energy found " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p1_end - p1_start) * 100000.0 / CLOCKS_PER_SEC, p1_end - p1_start);

	// restore positions
	memcpy(moons, moons_initial, sizeof(moon_t) * 4);

	uint64_t x_cycle = 0;
	uint64_t y_cycle = 0;
	uint64_t z_cycle = 0;

	printf(YELLOW ">" B_WHITE " finding steps until full system repeat...\n" RESET);
	printf(YELLOW ">" B_WHITE " finding steps until x dimension repeats...\n" RESET);
	p2_start = clock();

	// find x axis repitition
	while (1) {
		apply_gravity_x(moons);
		update_positions_x(moons);
		x_cycle++;
		if (is_initial_x(moons, moons_initial)) { break; }
	}

	p2_x_end = clock();
	printf(B_GREEN ">" B_WHITE " x dimension repeat found " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (p2_x_end - p2_start) * 1000.0 / CLOCKS_PER_SEC);

	// restore positions
	memcpy(moons, moons_initial, sizeof(moon_t) * 4);
	
	printf(YELLOW ">" B_WHITE " finding steps until y dimension repeats...\n" RESET);
	p2_y_start = clock();

	// find y axis repitition
	while (1) {
		apply_gravity_y(moons);
		update_positions_y(moons);
		y_cycle++;
		if (is_initial_y(moons, moons_initial)) { break; }
	}

	p2_y_end = clock();
	printf(B_GREEN ">" B_WHITE " y dimension repeat found " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (p2_y_end - p2_y_start) * 1000.0 / CLOCKS_PER_SEC);

	// restore positions
	memcpy(moons, moons_initial, sizeof(moon_t) * 4);
	
	printf(YELLOW ">" B_WHITE " finding steps until z dimension repeats...\n" RESET);
	p2_z_start = clock();

	// find z axis repitition
	while (1) {
		apply_gravity_z(moons);
		update_positions_z(moons);
		z_cycle++;
		if (is_initial_z(moons, moons_initial)) { break; }
	}

	p2_z_end = clock();
	printf(B_GREEN ">" B_WHITE " z dimension repeat found " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (p2_z_end - p2_z_start) * 1000.0 / CLOCKS_PER_SEC);
	printf(YELLOW ">" B_WHITE " calculating total steps until full system repititon...\n" RESET);
	p2_f_start = clock();

	// find full repitition
	uint64_t _lcm_ab = lcm(x_cycle, y_cycle);
	p2 = lcm(_lcm_ab, z_cycle);

	p2_f_end = clock();
	printf(B_GREEN ">" B_WHITE " total steps until full system repititon found " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p2_f_end - p2_f_start) * 100000.0 / CLOCKS_PER_SEC, p2_f_end - p2_f_start);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "total energy" WHITE "\t: " CYAN "%d\n" RESET, p1);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "repitition" WHITE "\t: " CYAN "%ld\n" RESET, p2);

	return 0;
}
