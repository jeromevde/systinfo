GCC = gcc

CFLAGS = -g -Wall -W -std=c99
LDFLAGS = -lm -L/usr/local/lib -L$(HOME)/local/lib -lSDL -lpthread
CUNITFLAGS= -L$(HOME)/local/lib -lcunit

LIBFRACTAL = libfractal/libfractal.a
LIBSTACK = stack/stack.a
LIBRARIES = libfractal/libfractal.a stack/stack.a

all: main

main: main.o $(LIBRARIES)
	@$(GCC) -o main main.o $(LIBRARIES) $(LDFLAGS)
	@echo "Project made"

main.o: main.c
	@$(GCC) $(CFLAGS) -c -o main.o  main.c

$(LIBFRACTAL):
	@cd libfractal && make

$(LIBSTACK):
	@cd stack && make

testProgram: test/test
	@LD_LIBRARY_PATH=$(HOME)/local/lib ./test/test

test/test: test/test.o $(LIBRARIES)
	@$(GCC)  $(LDFLAGS) -o test/test test/test.o $(LIBRARIES) $(CUNITFLAGS)

test/test.o: test/test.c
	@$(GCC) -c -o test/test.o test/test.c  $(CFLAGS)


clean:
	@find . -name \*.o -type f -delete
	@find . -name \*.a -type f -delete
	@rm -f main
	@rm -f test/test
	@echo "Cleaned project"

lib: $(LIBFRACTAL)
	@echo "Library made"
