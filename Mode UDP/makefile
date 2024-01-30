CC = gcc
CFLAGS = -Wall

all: clientUDP serveurUDP

clientUDP: clientUDP.c
	$(CC) $^ -o $@ $(CFLAGS)

serveurUDP: serveurUDP.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -f clientUDP serveurUDP


