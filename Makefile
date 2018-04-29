GCC = gcc
CFLAGS = -g -Wall -std=c99 -I$(HOME)/local/include
LDFLAGS = --static -g -lSDL -pthread -L$(HOME)/local/lib -lcunit
LIBFRACTAL = libfractal/libfractal.a
LIBSTACK = stack/stack.a
LIBRARIES = libfractal/libfractal.a stack/stack.a

all: main

main: main.o $(LIBRARIES)
	@$(GCC) $(LDFLAGS) -o main main.o $(LIBRARIES)
	@echo "Project made"

main.o: main.c
	@$(GCC) $(CFLAGS) -c -o main.o  main.c


$(LIBFRACTAL):
	cd libfractal && make

$(LIBSTACK):
	cd stack && make

test/test.o: test/test.c
	@$(GCC) $(CFLAGS) -c -o test/test.o test/test.c

test/test: test/test.o $(LIBRARIES)
	@$(GCC) $(LDFLAGS) $(CFLAGS) -o test/test test/test.o $(LIBRARIES)

testProgram: test/test
	@LD_LIBRARY_PATH=$(HOME)/local/lib ./test/test

clean:
	@find . -name \*.o -type f -delete
	@find . -name \*.a -type f -delete
	@rm -f main
	@rm -f test/test
	@echo "Cleaned project"

lib:
	cd libfractal && make
	@echo "Library made"
