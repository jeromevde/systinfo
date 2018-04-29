GCC = gcc
AR = ar
ARFLAGS = -cvq
CFLAGS = -Wall -std=c99 -g -c -I$HOME/local/include
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

test: test/test.o $(LIBRARIES)
	@$(GCC) $(CFLAGS) -o test/test test/test.o $(LIBRARIES)

test/test.o: test/test.c
	@$(GCC) $(CFLAGS) -o test/test.o test/test.c

testProgram: test
	./test
.PHONY: test

clean:
	@rm -f libfractal/*.o libfractal/*.a stack/*.o stack/*.a *.o main
	@rm -f test/test.o test/test
	@echo "Cleaned project"

lib:
	cd libfractal && make
	@echo "Library made"
