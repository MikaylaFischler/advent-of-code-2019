#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>

#include "main.h"

/**
 * @brief Recursively calculate fuel needs for fuel
 * 
 * @param f Amount of fuel
 * @return int32_t Total fuel needed to launch f amount of fuel
 */
int32_t calc_fuel(int32_t f) {
	int32_t _f = f / 3 - 2;
	if (_f > 0) { return _f + calc_fuel(_f); }
	return 0;
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 01" WHITE "           |\n" RESET);
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

	int32_t sum = 0;
	int32_t sum_p2 = 0;

	while ((num_read = getline(&line, &len, fp)) != -1) {
		// read in module
		int32_t val = atoi(line);

		// calc fuel requirements
		val = val / 3 - 2;
		sum += val;

		// part 2, calc fuel needed for fuel
		sum_p2 += val;
		sum_p2 += calc_fuel(val);
	}

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "fuel(modules)" WHITE "\t\t: " CYAN "%d\n" RESET, sum);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "fuel(modules + fuel)" WHITE "\t: " CYAN "%u\n" RESET, sum_p2);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
