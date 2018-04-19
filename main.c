#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <memory.h>
#include "libfractal/fractal.h"

int main(int argc, char *argv[])
{
    int print = false;
    int maxthread = 4;
    int argIndex = 1;

    if (*(argv[argIndex]) == "-d") {
        print = true;
        argIndex++;
    }

    if (*(argv[argIndex]) == "--maxthreads") {
        argIndex++;
        maxthread == *(argv[argIndex]);
        argIndex++;
    }

    while (argIndex < argc - 1) {
        /* TODO */
        argIndex++;
    }

    char ouputFile = *argv[argIndex];

    return 0;
}