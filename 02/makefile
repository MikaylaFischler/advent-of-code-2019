all : sol

optimized : optim-sol

sol : ./bin/main.o
	gcc -Wall -o sol ./bin/main.o

optim-sol : ./bin/main-optimized.o
	gcc -O3 -Wall ./bin/main-optimized.o -o optim-sol

./bin/main.o : ./src/main.c ./src/main.h
	gcc -Wall -c ./src/main.c -o ./bin/main.o -L../lib/

./bin/main-optimized.o : ./src/main.c ./src/main.h
	gcc -O3 -Wall -c ./src/main.c -o ./bin/main-optimized.o -L../lib/

clean :
	-@rm sol optim-sol ./bin/*.o 2>/dev/null || true
