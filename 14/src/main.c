#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>

#include "main.h"

#define PARSE_COMP_QUANT	0
#define PARSE_COMP_NAME		1
#define PARSE_DIVIDER		2
#define PARSE_PROD_QUANT	3
#define PARSE_PROD_NAME		4

typedef struct chemical {
	char name[10];
	uint64_t quant;
} chemical_t;

typedef struct reaction {
	chemical_t prod;
	uint8_t num_comps;
	chemical_t comps[20];
} reaction_t;

void inventory_clear(chemical_t* inventory, uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		inventory[i].quant = 0;
		inventory[i].name[0] = 0;
	}
}

void inventory_insert(chemical_t* inventory, uint8_t num_reactions, char* chem, uint16_t quantity) {
	for (uint8_t i = 0; i < num_reactions; i++) {
		if (inventory[i].name) {
			if (strcmp(inventory[i].name, chem) == 0) { inventory[i].quant += quantity; }
		} else {
			// insert here
			strcpy(inventory[i].name, chem);
			inventory[i].quant = quantity;
		 }
	}
}

chemical_t* inventory_check(chemical_t* inventory, uint8_t num_reactions, char* chem) {
	for (uint8_t i = 0; i < num_reactions; i++) {
		if (inventory[i].name[0]) {
			if (strcmp(inventory[i].name, chem) == 0) { return inventory + i; }
		} else {
			// insert here
			strcpy(inventory[i].name, chem);
			return inventory + i;
		}
	}

	return NULL;
}

uint64_t trace_ore_use(reaction_t* reactions, chemical_t* inventory, uint8_t num_reactions, char* chem, uint64_t amount) {
	chemical_t* inv = NULL;
	uint64_t count = 0;
	uint64_t needed = 0;

	for (uint8_t i = 0; i < num_reactions; i++) {
		if (strcmp(reactions[i].prod.name, chem) == 0) {
			needed = 1;

			// determine how much chemcial we need after taking into account inventory
			if ((inv = inventory_check(inventory, num_reactions, reactions[i].prod.name))) {
				if (inv->quant >= amount) {
					inv->quant -= amount;
					continue;
				} else if (inv->quant < amount) {
					amount -= inv->quant;
					inv->quant = 0;
				}
			}

			// determine number of reactions needed
			if (amount > reactions[i].prod.quant) {
				needed = amount / reactions[i].prod.quant;
				needed += (amount % reactions[i].prod.quant > 0);
			}

			// inventory the spare that we will generate
			inventory_insert(inventory, num_reactions, reactions[i].prod.name, needed * reactions[i].prod.quant - amount);
			
			// determine ore needed for this product
			for (uint8_t c = 0; c < reactions[i].num_comps; c++) {
				if (strcmp(reactions[i].comps[c].name, "ORE") == 0) {
					count += needed * reactions[i].comps[c].quant;
				} else {
					count += trace_ore_use(reactions, inventory, num_reactions, reactions[i].comps[c].name, needed * reactions[i].comps[c].quant);
				}
			}

			break;
		}
	}

	return count;
}

int main(int argc, char** argv) {
    printf(WHITE "x------------------------------------------------x\n" RESET);
    printf(WHITE "|           " BLUE "2019 " B_BLUE "advent of code" BLUE " day 14" WHITE "           |\n" RESET);
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
	printf(YELLOW ">" B_WHITE " parsing reaction equations...\n" RESET);

	time_start = clock();

	reaction_t* reactions = malloc(sizeof(reaction_t) * 100);
	chemical_t* inventory = calloc(sizeof(chemical_t), 100);

	uint32_t p1 = 0;
	uint32_t p2 = 0;

	uint16_t i = 0;
	uint8_t name_len = 0;
	uint8_t num_comps = 0;
	uint8_t parse_state = PARSE_COMP_QUANT;
	chemical_t raw_comps[20];
	chemical_t raw_prod;

	while ((num_read = getline(&line, &len, fp)) != -1) {
		parse_state = PARSE_COMP_QUANT;
		num_comps = 0;
		char* token = strtok(line, " ");

		while (token != NULL) {
			if (strcmp(token, "=>") == 0) { parse_state = PARSE_DIVIDER; }

			switch (parse_state) {
				case PARSE_COMP_QUANT:
					raw_comps[num_comps].quant = atoi(token);
					parse_state++;
					break;
				case PARSE_COMP_NAME:
					if (token[strlen(token) - 1] == ',') {
						name_len = strlen(token) - 1;
					} else {
						name_len = strlen(token);
					}

					strncpy(raw_comps[num_comps].name, token, name_len);
					raw_comps[num_comps].name[name_len] = '\0';
					num_comps++;
					parse_state = PARSE_COMP_QUANT;
					break;
				case PARSE_DIVIDER:
					parse_state++;
					break;
				case PARSE_PROD_QUANT:
					raw_prod.quant = atoi(token);
					parse_state++;
					break;
				case PARSE_PROD_NAME:
					if (token[strlen(token) - 1] == '\n') {
						name_len = strlen(token) - 1;
					} else {
						name_len = strlen(token);
					}

					strncpy(raw_prod.name, token, name_len);	// drop newline
					raw_prod.name[name_len] = '\0';
					parse_state++;
					break;
			}

			token = strtok(NULL, " ");
		}

		reactions[i].prod.quant = raw_prod.quant;
		strcpy(reactions[i].prod.name, raw_prod.name);
		reactions[i].num_comps = num_comps;

		for (uint8_t x = 0; x < num_comps; x++) {
			strcpy(reactions[i].comps[x].name, raw_comps[x].name);
			reactions[i].comps[x].quant = raw_comps[x].quant;
		}

		i++;
	}

	// free up memory
	fclose(fp);
	if (line) { free(line); }

	parse_end = clock();
	printf(B_GREEN ">" B_WHITE " equations parsed " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (parse_end - time_start) * 100000.0 / CLOCKS_PER_SEC, parse_end - time_start);
	printf(YELLOW ">" B_WHITE " finding how much ore for 1 fuel...\n" RESET);
	p1_start = clock();

	// chemical_t* inventory;
	uint8_t num_eqs = i;
	uint64_t ore_use;

	// part 1
	p1 = trace_ore_use(reactions, inventory, num_eqs, "FUEL", 1);

	p1_end = clock();
	printf(B_GREEN ">" B_WHITE " ore requirement found " WHITE "(" BLUE "%.3f us, %ld clock ticks" WHITE ")\n" RESET, (p1_end - p1_start) * 100000.0 / CLOCKS_PER_SEC, p1_end - p1_start);

	// part 2
	// clear the inventory
	inventory_clear(inventory, num_eqs);

	// calculation variables
	uint64_t ore_avail = 1000000000000;
	uint32_t num_fuel;
	uint32_t upper = 0;
	uint32_t lower = 1;

	printf(YELLOW ">" B_WHITE " determining amount of fuel possible from 1000000000000 ore...\n" RESET);
	p2_start = clock();

	while (lower + 1 != upper) {
		if (upper == 0) {
			num_fuel = lower << 1;				// multiply by 2
		} else {
			num_fuel = (lower + upper) >> 1;	// divide by 2
		}
		
		ore_use = trace_ore_use(reactions, inventory, i, "FUEL", num_fuel);

		if (ore_use > ore_avail) {
			upper = num_fuel;
		} else { lower = num_fuel; }
	}

	p2 = lower;

	p2_end = clock();
	printf(B_GREEN ">" B_WHITE " answer found " WHITE "(" BLUE "%.3f ms" WHITE ")\n" RESET, (p2_end - p2_start) * 1000.0 / CLOCKS_PER_SEC);

	time_end = clock();

    printf(GREEN "done.\n" RESET);

	printf(B_WHITE "\ntotal time taken" WHITE "\t\t: " RED "%f" WHITE " seconds\n" RESET, (double) (time_end - time_start) / CLOCKS_PER_SEC);
	printf(B_RED "[" MAGENTA "part 1" B_RED "] " B_WHITE "ore for 1 fuel" WHITE "\t\t: " CYAN "%d\n" RESET, p1);
	printf(B_RED "[" MAGENTA "part 2" B_RED "] " B_WHITE "max producible fuel" WHITE "\t: " CYAN "%d\n" RESET, p2);

	return 0;
}
