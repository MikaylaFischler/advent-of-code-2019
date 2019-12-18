all : sol

optimized : optim-sol

sol : ./bin/main.o ./bin/moons.o
	gcc -Wall -o sol ./bin/main.o ./bin/moons.o

optim-sol : ./bin/main-optimized.o ./bin/moons-optimized.o
	gcc -O3 -Wall ./bin/main-optimized.o ./bin/moon-optimized.o -o optim-sol

./bin/main.o : ./src/main.c ./src/main.h
	gcc -Wall -c ./src/main.c -o ./bin/main.o

./bin/moons.o : ./src/moons.c ./src/moons.h
	gcc -Wall -c ./src/moons.c -o ./bin/moons.o

./bin/main-optimized.o : ./src/main.c ./src/main.h
	gcc -O3 -Wall -c ./src/main.c -o ./bin/main-optimized.o

./bin/moons-optimized.o : ./src/moons.c ./src/moons.h
	gcc -O3 -Wall -c ./src/moons.c -o ./bin/moons-optimized.o

clean :
	-@rm sol optim-sol ./bin/*.o 2>/dev/null || true
