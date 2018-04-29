GCC = gcc
AR = ar
ARFLAGS = -cvq
CFLAGS = -Wall -std=c99 -g -c
LDFLAGS = --static -g -lSDL -lcunit -pthread
LIBFRACTAL = libfractal/libfractal.a
LIBSTACK = stack/stack.a
LIBRARIES = libfractal/libfractal.a stack/stack.a

all: main

main.o: main.c
	@$(GCC) $(CFLAGS) main.c

main: main.o $(LIBRARIES)
	@$(GCC) $(LDFLAGS) -o main $(LIBRARIES) main.o
	@echo "Project made"

$(LIBFRACTAL):
	cd libfractal && make

$(LIBSTACK):
	cd stack && make

clean:
	@rm -f libfractal/*.o libfractal/*.a stack/*.o stack/*.a *.o main
	@echo "Cleaned project"

lib:
	cd libfractal && make
	@echo "Library made"
