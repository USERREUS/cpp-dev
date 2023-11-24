CC=g++
CFLAGS=-Wall
LIBS=-lhiredis

all: server

server: main.cpp
	$(CC) $(CFLAGS) -o server main.cpp $(LIBS)

clean:
	rm -f server
