.PHONY: all
all: tcp-socket-client.out
	./tcp-socket-client.out

tcp-socket-client.out: tcp-socket-client.c
	gcc -o tcp-socket-client.out --std=c99 -O2 -Wall tcp-socket-client.c

.PHONY: clean
clean:
	rm -f tcp-socket-client.out
