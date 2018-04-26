#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <memory.h>
#include "libfractal/fractal.h"


int main(int argc, char *argv[])
{
    /**
     * @var boolean : need to print all computed fractals
     */
    int printAll = false;

    /**
     * @var int : max amount of threads
     */
    int maxthreads = 2;

    /**
     * @var int : args counter
     */
    int argIndex = 1; // dummy variable

    printf("%s\n", "==== Starting fractal computer ====");

    /*
     * At least 3 arguments must be given
     */
    if (argc < 3){
        printf("%s\n","You must at least provide an input and an output file" );
        exit(EXIT_FAILURE);
    }

    /*
     * Is the -d arg given
     */
    if (strcmp(argv[argIndex], "-d") == 0) {
        printAll = true;
        printf("%s\n", "All fractals will be drawn");
        argIndex++;
    }

    /*
     * Is the thread amount overwritten
     */
    if (strcmp( argv[argIndex], "--maxthreads") == 0) {
        argIndex++;
        maxthreads =atoi(argv[argIndex]);
        printf("%s : %d\n", "Max amount of threads has been set to", maxthreads);
        argIndex++;
    }

    while (argIndex < argc - 1) {
        /* TODO */
        argIndex++;
    }

    char ouputFile = *argv[argIndex];

    return 0;
}
