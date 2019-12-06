all : libintcode.a

libintcode.a : ./bin/intcode.o
	ar rcs libintcode.a ./bin/intcode.o

bin/intcode.o : src/intcode.c intcode.h
	gcc -O3 -Wall -c src/intcode.c -o bin/intcode.o

clean :
	-@rm libintcode.a ./bin/*.o 2>/dev/null || true
