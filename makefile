CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGETS = proj1v2

all: $(TARGETS)


proj1v2: proj1v2.c
	$(CC) $(CFLAGS) -o proj1v2 proj1v2.c

clean:
	rm -f $(TARGETS)
