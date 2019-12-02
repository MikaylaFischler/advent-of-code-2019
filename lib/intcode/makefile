all : intcode.a

intcode.a : ./bin/intcode.o
	ar rcs intcode.a ./bin/intcode.o

bin/intcode.o : src/intcode.c intcode.h
	gcc -O3 -Wall -c src/intcode.c -o bin/intcode.o

clean :
	rm intcode.a ./bin/*.o
