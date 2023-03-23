CC=g++
CFLAGS=-I./include

all: priority_queue_test.o d_star_lite.o
	$(CC) priority_queue_test.o d_star_lite.o -o pqt

priority_queue_test.o: ./src/d_star_lite/priority_queue_test.cpp
	$(CC) -c $(CFLAGS) ./src/d_star_lite/priority_queue_test.cpp

d_star_lite.o: ./src/d_star_lite/d_star_lite.cpp
	$(CC) -c $(CFLAGS) ./src/d_star_lite/d_star_lite.cpp

clean:
	rm -f *.o