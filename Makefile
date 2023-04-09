CC=g++
CFLAGS=-I./include

all: d_star.o d_star_lite.o
	$(CC) d_star.o d_star_lite.o -o ec

d_star.o : ./src/edge_costs/d_star.cpp
	$(CC) -c $(CFLAGS) ./src/edge_costs/d_star.cpp

d_star_lite.o: ./src/edge_costs/d_star_lite.cpp
	$(CC) -c $(CFLAGS) ./src/edge_costs/d_star_lite.cpp

clean:
	rm -f *.o