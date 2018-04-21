#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <memory.h>
#include "libfractal/fractal.h"

int main(int argc, char *argv[])
{
    int print = false;
    int maxthread = 4;
    int argIndex = 1;

    if (strcmp( argv[argIndex], "-d")==1) {
        print = true;
        argIndex++;
    }
    int a = 3;
    if (strcmp( argv[argIndex], "--maxthreads") ==1) {
        argIndex++;
        maxthread =*(argv[argIndex]);
        argIndex++;
    }

    while (argIndex < argc - 1) {
        /* TODO */
        argIndex++;
    }

    char ouputFile = *argv[argIndex];

    return 0;
}
