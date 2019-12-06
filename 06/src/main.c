#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>

#include "main.h"

typedef struct object_raw {
	char id[4];
	char orbits[4];
} object_r_t;

typedef struct object_cost {
	char id[4];	
	uint16_t cost;
} oc_t;

uint32_t indirect_orbits(object_r_t* objects, uint16_t num_objects, uint16_t id) {
	uint32_t count = 0;
	object_r_t b = objects[id];

	for (uint16_t i = 0; i < num_objects; i++) {
		if (strncmp(objects[i].orbits, b.id, 3) == 0) {
			count = 1 + indirect_orbits(objects, num_objects, i);
		}
	}

	return count;
}

void traverse_objects(object_r_t* objects, uint16_t num_objects, oc_t* list, uint16_t* list_pos, uint16_t cur_cost, uint16_t id) {
	for (uint16_t i = 0; i < num_objects; i++) {
		if (strncmp(objects[i].orbits, objects[id].id, 3) == 0) {
			strncpy(list[*list_pos].id, objects[i].id, 4);
			list[*list_pos].cost = cur_cost;
			list_pos[0]++;
			traverse_objects(objects, num_objects, list, list_pos, cur_cost + 1, i);
		}
	}

}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 06" WHITE "           |\n" RESET);
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

	uint16_t num_objects = 0;
	object_r_t* objects = calloc(sizeof(object_r_t), 1500);

	uint16_t you_id;
	uint16_t san_id;

	while ((num_read = getline(&line, &len, fp)) != -1) {
		// do something
		strncpy(objects[num_objects].id, line, 3);
		strncpy(objects[num_objects].orbits, line + 4, 3);

		if (strncmp(objects[num_objects].orbits, "YOU", 3) == 0) {
			you_id = num_objects;
		} else if (strncmp(objects[num_objects].orbits, "SAN", 3) == 0) {
			san_id = num_objects;
		}

		// printf("%s ) %s\n", objects[num_objects].id, objects[num_objects].orbits);

		num_objects++;
	}

	uint32_t count = 0;

	for (int i = 0; i < num_objects; i++) {
		count += indirect_orbits(objects, num_objects, i);
	}

	oc_t* you_objects = calloc(sizeof(oc_t), num_objects);
	oc_t* san_objects = calloc(sizeof(oc_t), num_objects);

	uint16_t list_pos_y = 0;
	uint16_t list_pos_s = 0;

	traverse_objects(objects, num_objects, you_objects, &list_pos_y, 1, you_id);
	traverse_objects(objects, num_objects, san_objects, &list_pos_s, 1, san_id);

	uint32_t least_cost = -1;

	for (uint16_t i = 0; i < list_pos_y; i++) {
		oc_t you_object = you_objects[i];
		for (uint16_t j = 0; j < list_pos_s; j++) {
			oc_t san_object = san_objects[j];
			if (strncmp(you_object.id, san_object.id, 3) == 0) {
				if (you_object.cost + san_object.cost < least_cost && you_object.cost + san_object.cost != 0) {
					least_cost = you_object.cost + san_object.cost;
				}
			}
		}
	}

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "direct + indirect orbits" WHITE "\t: " CYAN "%d\n" RESET, count + num_objects);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "min orbital transfers" WHITE "\t\t: " CYAN "%d\n" RESET, least_cost);

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	return 0;
}
