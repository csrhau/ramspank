CC=gcc
CFLAGS= -O0 -Wall -Wextra -pedantic

all: ramspank.x

clean:
	rm -f ramspank.x *.o
.c.o:
	$(CC) -c $(CFLAGS) $<

ramspank.x: main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
