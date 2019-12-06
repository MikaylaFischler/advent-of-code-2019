#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>

#include "main.h"

/**
 * @brief a semi-optimized incrementor for this specific problem
 * @param num input number (string of length 6)
 */
void inc(char* num) {
	// if it is invalid because of something decreasing, just fix that
	char last_val = 0;
	for (uint8_t i = 0; i < 6; i++) {
		if (num[i] < last_val) { 
			// overwrite all following values since they cannot be allowed to decrease
			for (; i < 6; i++) { num[i] = last_val; } 
			return;
		}

		last_val = num[i];
	}

	// if we didn't change stuff up there, just increment
	sprintf(num, "%6d", atoi(num) + 1);
}

/**
 * @brief check if a numeric string is a valid password
 * @param num input number (string of length 6)
 * @param is_part_2 if this should be done for part 2
 * @return uint8_t if num is a valid password
 */
uint8_t is_valid(char* num, uint8_t is_part_2) {
	uint8_t seen[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	char last_val = 0;

	// validate that no numbers decrease
	for (uint8_t i = 0; i < 6; i++) {
		seen[num[i] - 0x30]++; // mark seen
		if (num[i] < last_val) { return 0; }
		last_val = num[i];
	}

	// check if we have seen numbers enough
	// no need to check if adjacent ones are equal, because if non are decreasing
	// it is impossible for repeats to not be adjacent
	for (uint8_t i = 0; i < 10; i++) {
		if (is_part_2) {
			if (seen[i] == 2) { return 1; }
		} else {
			if (seen[i] > 1) { return 1; }
		}
	}

	return 0;
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 04" WHITE "           |\n" RESET);
    printf(WHITE "x------------------------------------------------x\n\n" RESET);

	// timing
	clock_t time_start, time_end, p1_start, p1_end, p2_start, p2_end;

	printf(YELLOW "starting...\n" RESET);

	time_start = clock();

	// input numbers
	uint32_t num_a = 128392;
	uint32_t num_b = 643281;

	// answers
	uint16_t p1_count = 0;
	uint16_t p2_count = 0;

	// string representation of number
	char* num = malloc(sizeof(char) * 7);

	// load number into string
	sprintf(num, "%d", num_a);

	p1_start = clock();

	// find valid passwords for part 1
	while (atoi(num) < num_b) {
		if (is_valid(num, 0)) { p1_count++; }
		inc(num);
	}

	p1_end = clock();
	printf(B_GREEN ">" B_WHITE " all valid passwords found for part 1 " WHITE "(" BLUE "%.3f us, %ld clock cycles" WHITE ")\n" RESET, (p1_end - p1_start) * 100000.0 / CLOCKS_PER_SEC, p1_end - p1_start);

	// restore number into string
	sprintf(num, "%d", num_a);

	p2_start = clock();

	// find valid passwords for part 2
	while (atoi(num) < num_b) {
		if (is_valid(num, 1)) { p2_count++; }
		inc(num);
	}

	p2_end = clock();
	printf(B_GREEN ">" B_WHITE " all valid passwords found for part 2 " WHITE "(" BLUE "%.3f us, %ld clock cycles" WHITE ")\n" RESET, (p2_end - p2_start) * 100000.0 / CLOCKS_PER_SEC, p2_end - p2_start);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "valid passwords" WHITE "\t: " CYAN "%d\n" RESET, p1_count);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "at least 1 pair" WHITE "\t: " CYAN "%d\n" RESET, p2_count);

	return 0;
}
