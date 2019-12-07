all : libintcode.a

libintcode.a : ./bin/intcode.o bin/util.o
	ar rcs libintcode.a ./bin/intcode.o ./bin/util.o

bin/intcode.o : src/intcode.c ic_types.h intcode.h 
	gcc -O3 -Wall -c src/intcode.c -o bin/intcode.o

bin/util.o : src/util.c ic_types.h src/util.h
	gcc -O3 -Wall -c src/util.c -o bin/util.o

clean :
	-@rm libintcode.a ./bin/*.o 2>/dev/null || true
