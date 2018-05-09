GCC = gcc

CFLAGS = -g -Wall -W -std=c99
LDFLAGS = -lm -lSDL -lpthread

LCUNIT= -L$(HOME)/local/lib -lcunit
CCUNIT = -I$(HOME)/local/include -lcunit

LIBFRACTAL = libfractal/libfractal.a
LIBSTACK = stack/stack.a
LIBRARIES = libfractal/libfractal.a stack/stack.a

#default recipe will automatically clean and build project
#so no need to clean the project before building
all:
	@make clean
	@make main


main: main.o $(LIBRARIES)
	@$(GCC) -o main main.o $(LIBRARIES) $(LDFLAGS)
	@echo "Project made"

main.o: main.c
	@$(GCC) $(CFLAGS) -c -o main.o  main.c

$(LIBFRACTAL):
	@cd libfractal && make

$(LIBSTACK):
	@cd stack && make

test:
	@make all
	@make tests/test
	@./tests/test

tests/test: tests/test.o $(LIBRARIES)
	@$(GCC) -o tests/test tests/test.o $(LIBRARIES)  $(LDFLAGS) $(LCUNIT)

tests/test.o: tests/test.c
	@$(GCC) -c -o tests/test.o tests/test.c  $(CFLAGS) $(CCUNIT)


clean:
	@find . -name \*.o -type f -delete
	@find . -name \*.a -type f -delete
	@rm -f main
	@rm -f tests/test
	@rm -f *.bmp
	@echo "Cleaned project"

lib: $(LIBFRACTAL)
	@echo "Library made"
