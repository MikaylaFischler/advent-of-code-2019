#include "moons.h"

uint8_t pairs[][2] = {
	{ 0, 1 },
	{ 0, 2 },
	{ 0, 3 },
	{ 1, 2 },
	{ 1, 3 },
	{ 2, 3 }
};

void copy_velocity(cartesian_t* to, cartesian_t* from) {
	to->x = from->x;
	to->y = from->y;
	to->z = from->z;
}

void apply_gravity_x(moon_t* moons) {
	for (uint8_t i = 0; i < 6; i++) {
		uint8_t m1 = pairs[i][0];
		uint8_t m2 = pairs[i][1];

		// update x
		if (moons[m1].pos.x > moons[m2].pos.x) {
			moons[m1].vel_prime.x--;
			moons[m2].vel_prime.x++;
		} else if (moons[m1].pos.x < moons[m2].pos.x) {
			moons[m1].vel_prime.x++;
			moons[m2].vel_prime.x--;
		}
	}
}

void apply_gravity_y(moon_t* moons) {
	for (uint8_t i = 0; i < 6; i++) {
		uint8_t m1 = pairs[i][0];
		uint8_t m2 = pairs[i][1];
		
		// update y
		if (moons[m1].pos.y > moons[m2].pos.y) {
			moons[m1].vel_prime.y--;
			moons[m2].vel_prime.y++;
		} else if (moons[m1].pos.y < moons[m2].pos.y) {
			moons[m1].vel_prime.y++;
			moons[m2].vel_prime.y--;
		}
	}
}

void apply_gravity_z(moon_t* moons) {
	for (uint8_t i = 0; i < 6; i++) {
		uint8_t m1 = pairs[i][0];
		uint8_t m2 = pairs[i][1];
		
		// update z
		if (moons[m1].pos.z > moons[m2].pos.z) {
			moons[m1].vel_prime.z--;
			moons[m2].vel_prime.z++;
		} else if (moons[m1].pos.z < moons[m2].pos.z) {
			moons[m1].vel_prime.z++;
			moons[m2].vel_prime.z--;
		}
	}
}

void apply_gravity(moon_t* moons) {
	for (uint8_t i = 0; i < 6; i++) {
		uint8_t m1 = pairs[i][0];
		uint8_t m2 = pairs[i][1];

		// update x
		if (moons[m1].pos.x > moons[m2].pos.x) {
			moons[m1].vel_prime.x--;
			moons[m2].vel_prime.x++;
		} else if (moons[m1].pos.x < moons[m2].pos.x) {
			moons[m1].vel_prime.x++;
			moons[m2].vel_prime.x--;
		}
		
		// update y
		if (moons[m1].pos.y > moons[m2].pos.y) {
			moons[m1].vel_prime.y--;
			moons[m2].vel_prime.y++;
		} else if (moons[m1].pos.y < moons[m2].pos.y) {
			moons[m1].vel_prime.y++;
			moons[m2].vel_prime.y--;
		}
		
		// update z
		if (moons[m1].pos.z > moons[m2].pos.z) {
			moons[m1].vel_prime.z--;
			moons[m2].vel_prime.z++;
		} else if (moons[m1].pos.z < moons[m2].pos.z) {
			moons[m1].vel_prime.z++;
			moons[m2].vel_prime.z--;
		}
	}
}

void update_positions_x(moon_t* moons) {
	for (uint8_t m = 0; m < NUM_MOONS; m++) {
		moons[m].vel.x = moons[m].vel_prime.x;
		moons[m].pos.x += moons[m].vel.x;
	}
}

void update_positions_y(moon_t* moons) {
	for (uint8_t m = 0; m < NUM_MOONS; m++) {
		moons[m].vel.y = moons[m].vel_prime.y;
		moons[m].pos.y += moons[m].vel.y;
	}
}

void update_positions_z(moon_t* moons) {
	for (uint8_t m = 0; m < NUM_MOONS; m++) {
		moons[m].vel.z = moons[m].vel_prime.z;
		moons[m].pos.z += moons[m].vel.z;
	}
}

void update_positions(moon_t* moons) {
	for (uint8_t m = 0; m < NUM_MOONS; m++) {
		copy_velocity(&(moons[m].vel), &(moons[m].vel_prime));
		
		moons[m].pos.x += moons[m].vel.x;
		moons[m].pos.y += moons[m].vel.y;
		moons[m].pos.z += moons[m].vel.z;
	}
}

uint32_t total_system_energy(moon_t* moons) {
	uint32_t sum_total = 0;
	uint32_t potential = 0;
	uint32_t kinetic = 0;

	for (uint8_t m = 0; m < NUM_MOONS; m++) {
		potential = abs(moons[m].pos.x) + abs(moons[m].pos.y) + abs(moons[m].pos.z);
		kinetic = abs(moons[m].vel.x) + abs(moons[m].vel.y) + abs(moons[m].vel.z);
		sum_total += potential * kinetic;
	}

	return sum_total;
}

uint8_t is_initial_x(moon_t* moons, moon_t* moons_initial) {
	for (uint8_t m = 0; m < NUM_MOONS; m++) {
		if (!(moons[m].pos.x == moons_initial[m].pos.x && 
			  moons[m].vel.x == moons_initial[m].vel.x)) {
			return 0;
		}
	}

	return 1;
}

uint8_t is_initial_y(moon_t* moons, moon_t* moons_initial) {
	for (uint8_t m = 0; m < NUM_MOONS; m++) {
		if (!(moons[m].pos.y == moons_initial[m].pos.y && 
			  moons[m].vel.y == moons_initial[m].vel.y)) {
			return 0;
		}
	}

	return 1;
}

uint8_t is_initial_z(moon_t* moons, moon_t* moons_initial) {
	for (uint8_t m = 0; m < NUM_MOONS; m++) {
		if (!(moons[m].pos.z == moons_initial[m].pos.z && 
			  moons[m].vel.z == moons_initial[m].vel.z)) {
			return 0;
		}
	}

	return 1;
}
