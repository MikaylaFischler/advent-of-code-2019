all : sol

optimized : optim-sol

sol : ./bin/main.o
	gcc -Wall -o sol ./bin/main.o

optim-sol : ./bin/main-optimized.o
	gcc -O3 -Wall ./bin/main-optimized.o -o optim-sol

./bin/main.o : ./src/main.c ./src/main.h ../lib/intcode/intcode.h
	gcc -Wall -c ./src/main.c -o ./bin/main.o -L../lib/ -lintcode

./bin/main-optimized.o : ./src/main.c ./src/main.h ../lib/intcode/intcode.h
	gcc -O3 -Wall -c ./src/main.c -o ./bin/main-optimized.o -L../lib/ -lintcode

clean :
	rm sol optim-sol ./bin/*.o
