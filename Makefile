GCC = gcc
AR = ar
ARFLAGS = -cvq
CFLAGS = -Wall -std=c99 -g -c
LDFLAGS = --static -g
LIBFRACTAL = libfractal/libfractal.a

all: main

lib:
	cd libfractal && make

main.o: main.c
	$(GCC) $(CFLAGS) main.c

main: main.o $(LIBFRACTAL)
	$(GCC) $(LDFLAGS) -o main $(LIBFRACTAL) main.o

clean:
	@rm -rf *.o libfractal/*.o libractal/*.a main.o
