CC=mpicc
CFLAGS=-fopenmp -Wall
TARGET=torneo

all: $(TARGET)

$(TARGET): src/main.c
	$(CC) $(CFLAGS) -o $@ $

clean:
	rm -f $(TARGET)
