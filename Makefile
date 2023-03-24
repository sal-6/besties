CC=g++
CFLAGS=-I./include

all: d_star.o d_star_lite.o
	$(CC) d_star.o d_star_lite.o -o ds

d_star.o : ./src/d_star_lite/d_star.cpp
	$(CC) -c $(CFLAGS) ./src/d_star_lite/d_star.cpp

d_star_lite.o: ./src/d_star_lite/d_star_lite.cpp
	$(CC) -c $(CFLAGS) ./src/d_star_lite/d_star_lite.cpp

clean:
	rm -f *.o