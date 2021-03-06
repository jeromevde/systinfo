GCC = gcc

CFLAGS = -g -Wall -W -std=c99
LDFLAGS = -lm -lSDL -lpthread

LCUNIT= -L/usr/local/lib -lcunit
CCUNIT = -I/usr/local/include -lcunit

LIBFRACTAL = libfractal/libfractal.a
LIBSTACK = stack/stack.a
LIBRARIES = libfractal/libfractal.a stack/stack.a

#default recipe will automatically clean and build project
#so no need to clean the project before building
.PHONY: all
all:
	@make clean
	@make main

.PHONY: tests
tests:
	@make all
	@./tests/test


main: main.o $(LIBRARIES) tests/test
	@$(GCC) -o main main.o $(LIBRARIES) $(LDFLAGS)
	@echo "Project made"

main.o: main.c
	@$(GCC) $(CFLAGS) -c -o main.o  main.c

$(LIBFRACTAL):
	@cd libfractal && make

$(LIBSTACK):
	@cd stack && make

tests/test: tests/test.o $(LIBRARIES)
	@$(GCC) -o tests/test tests/test.o $(LIBRARIES)  $(LDFLAGS) $(LCUNIT)

tests/test.o: tests/test.c
	@$(GCC) -c -o tests/test.o tests/test.c  $(CFLAGS) $(CCUNIT)

.PHONY: clean
clean:
	@find . -name \*.o -type f -delete
	@find . -name \*.a -type f -delete
	@rm -f main
	@rm -f tests/test
	@rm -f *.bmp
	@echo "Cleaned project"

.PHONY: lib
lib: $(LIBFRACTAL)
	@echo "Library made"
