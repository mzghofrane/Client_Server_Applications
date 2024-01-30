CC = gcc
CFLAGS = -Wall

all: clientTCP serveurTCP

clientTCP: clientTCP.c
	$(CC) $^ -o $@ $(CFLAGS)

serveurTCP: serveurTCP.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -f clientTCP serveurTCP


