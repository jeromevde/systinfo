#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <memory.h>
#include <pthread.h>
#include "libfractal/fractal.h"



void *reader(void *params) {

}

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

    /**
     * @var int : total amout of files
     */
    int totalFiles = argc - argIndex -1;

    /**
     * @var pthread_t[] : Table containing the file reading threads
     */
    pthread_t fileReaderThreads[totalFiles];

    /**
     * @var int : files counter
     */
    int fileIndex = 0;

    /*
     * Running through all args containing file inputs
     */
    for (fileIndex = 0; fileIndex < totalFiles; fileIndex++) {

        if (strcmp(argv[argIndex], "-") == 0) {

        } else {

            /* Initializing file reading threads */
            int result = pthread_create(&fileReaderThreads[fileIndex], NULL, *reader, (void *) argv[argIndex]);

            if (result != 0) {
                fprintf(stderr, "%s %s\n", "Error while creating the thread to read file", argv[argIndex]);
            }
        }

        argIndex++;
    }

    char ouputFile = *argv[argIndex];

    return 0;
}

