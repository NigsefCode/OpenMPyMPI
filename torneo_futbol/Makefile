CC=mpicc
CFLAGS=-fopenmp -Wall
TARGET=torneo

all: $(TARGET)

$(TARGET): src/main.c
	$(CC) $(CFLAGS) src/main.c -o $(TARGET)

clean:
	rm -f $(TARGET)
