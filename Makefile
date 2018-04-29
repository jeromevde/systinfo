GCC = gcc
AR = ar
ARFLAGS = -cvq
CFLAGS = -Wall -std=c99 -g -c
LDFLAGS = --static -g  -lcunit -pthread
LIBFRACTAL = libfractal/libfractal.a

all: main

gdb: main_gdb


lib:
	cd libfractal && make

main.o: main.c
	@$(GCC) $(CFLAGS) main.c

main: main.o $(LIBFRACTAL)
	@$(GCC) $(LDFLAGS) -o main $(LIBFRACTAL) main.o

main_gdb: main.o $(LIBFRACTAL)
	@$(GCC) $(LDFLAGS) -g -o main $(LIBFRACTAL) main.o

clean:
	@rm -f libfractal/*.o libractal/*.a *.o mainµ
	@echo "Cleaned project"
