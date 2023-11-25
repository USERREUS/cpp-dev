CC=g++
CFLAGS=-Wall
LIBS=-lhiredis

all: server

server: main.cpp
	$(CC) $(CFLAGS) -o server main.cpp script/helper/helper.cpp script/store/store.cpp script/session/session.cpp script/session/redis.cpp script/http/http.cpp $(LIBS)

clean:
	rm -f server
