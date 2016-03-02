CC=gcc
CFLAGS= -O0 -std=gnu99 -Wall -Wextra -pedantic -g

all: ramspank.x

clean:
	rm -f ramspank.x *.o
.c.o:
	$(CC) -c $(CFLAGS) $<

ramspank.x: main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
