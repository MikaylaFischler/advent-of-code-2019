#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "main.h"

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 08" WHITE "           |\n" RESET);
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
	clock_t time_start, time_end, parse_end, p1_start, p1_end, p2_start, p2_end;

	printf(YELLOW "starting...\n" RESET);
	printf(YELLOW ">" B_WHITE " parsing file...\n" RESET);

	time_start = clock();

	char*** layers = malloc(sizeof(char**) * 200);
	for (uint8_t i = 0; i < 200; i++) {
		layers[i] = malloc(sizeof(char*) * 6);
		for (uint8_t j = 0; j < 6; j++) {
			layers[i][j] = malloc(sizeof(char) * 25);
		}
	}

	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t l = 0;

	uint16_t min_zeros = -1;
	uint16_t layer_zeros = 0;
	uint16_t min_layer = 0;

	while ((num_read = getline(&line, &len, fp)) != -1) {
		for (uint16_t x = 0; x < len && line[x] != '\0'; x++) {
			layers[l][j][i++] = line[x];
			layer_zeros += (line[x] == '0');

			if (i == 25) { i = 0; j++; }
			if (j == 6) { 
				if (layer_zeros < min_zeros) {
					min_zeros = layer_zeros;
					min_layer = l;
				}

				j = layer_zeros = 0; 
				l++; 
			}
		}
	}

	int8_t num_layers = l;

	parse_end = clock();
	printf(B_GREEN ">" B_WHITE " file parsed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (parse_end - time_start) * 100000.0 / CLOCKS_PER_SEC, parse_end - time_start);
	printf(YELLOW ">" B_WHITE " analyzing data...\n" RESET);
	p1_start = clock();

	/* ===== Part 1 ===== */

	uint16_t num_ones = 0;
	uint16_t num_twos = 0;

	for (j = 0; j < 6; j++) {
		for (i = 0; i < 25; i++) {
			num_ones += layers[min_layer][j][i] == '1';
			num_twos += layers[min_layer][j][i] == '2';
		}
	}

	p1_end = clock();
	printf(B_GREEN ">" B_WHITE " data analyzed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p1_end - p1_start) * 100000.0 / CLOCKS_PER_SEC, p1_end - p1_start);
	printf(YELLOW ">" B_WHITE " rendering image...\n" RESET);
	p2_start = clock();

	/* ===== Part 2 ===== */

	printf("\n");

	for (j = 0; j < 6; j++) {
		for (i = 0; i < 25; i++) {
			for (l = 0; l < num_layers; l++) {
				if (layers[l][j][i] != '2') {
					if (layers[l][j][i] == '1') {
							printf(WHITE_BG " " RESET);
					} else { printf(BLACK_BG " " RESET); }
					break;
				}
			}
		}

		printf("\n");
	}

	printf("\n");

	p2_end = clock();
	printf(B_GREEN ">" B_WHITE " image rendered " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p2_end - p2_start) * 100000.0 / CLOCKS_PER_SEC, p2_end - p2_start);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "1s * 2s" WHITE "\t: " CYAN "%d\n" RESET, num_ones * num_twos);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "image" WHITE "\t\t: " CYAN "see above\n" RESET);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
