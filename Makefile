CC=g++
CFLAGS=-I./include

all: field.o field_d_star.o
	$(CC) field.o field_d_star.o -o fds

field.o : ./src/field_d_star/field.cpp
	$(CC) -c $(CFLAGS) ./src/field_d_star/field.cpp

field_d_star.o: ./src/field_d_star/field_d_star.cpp
	$(CC) -c $(CFLAGS) ./src/field_d_star/field_d_star.cpp

clean:
	rm -f *.o