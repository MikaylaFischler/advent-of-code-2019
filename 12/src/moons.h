#ifndef MOONS_H_
#define MOONS_H_

#include <math.h>
#include <inttypes.h>

#define NUM_MOONS 4

typedef struct cartesian {
	int32_t x;
	int32_t y;
	int32_t z;
} cartesian_t;

typedef struct moon {
	cartesian_t pos;
	cartesian_t vel;
	cartesian_t vel_prime;
} moon_t;

void copy_velocity(cartesian_t* to, cartesian_t* from);

void apply_gravity_x(moon_t* moons);
void apply_gravity_y(moon_t* moons);
void apply_gravity_z(moon_t* moons);
void apply_gravity(moon_t* moons);

void update_positions_x(moon_t* moons);
void update_positions_y(moon_t* moons);
void update_positions_z(moon_t* moons);
void update_positions(moon_t* moons);

uint32_t total_system_energy(moon_t* moons);

uint8_t is_initial_x(moon_t* moons, moon_t* moons_initial);
uint8_t is_initial_y(moon_t* moons, moon_t* moons_initial);
uint8_t is_initial_z(moon_t* moons, moon_t* moons_initial);

#endif
