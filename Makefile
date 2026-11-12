CC = gcc
CFLAGS = -Wall -pthread
FILES = main.c kernel.c

main: $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o main

clean:
	rm -f main