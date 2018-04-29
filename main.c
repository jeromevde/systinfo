#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <memory.h>
#include <pthread.h>
#include <semaphore.h>
#include "libfractal/fractal.h"

/* ******************************************************************
 *                                                                  *
 *    ,--------,                                                    *
 *    |  main  | ==> Reads input arguments                          *
 *    '--------'                                                    *
 *     |            ||        ||         ||                         *
 *     |            ||        ||         ||     --fileReader        *
 *     |           \||/      \||/       \||/                        *
 *     |            \/        \/         \/                         *
 *     |      ,---------------------------------,                   *
 *     |      | Buffer with fractals to compute |                   *
 *     |      '---------------------------------'                   *
 *     |            ||        ||         ||                         *
 *     |            ||        ||         ||     --computer          *
 *     |           \||/      \||/       \||/                        *
 *     |            \/        \/         \/                         *
 *     |      ,---------------------------------,                   *
 *     |      |  Buffer with computed fractals  |                   *
 *     |      '---------------------------------'                   *
 *     |        |                                                   *
 *     | <------┘                                                   *
 *     |                                                            *
 *    ,---------------,                                             *
 *    | Print fractal |                                             *
 *    '---------------'                                             *
 *                                                                  *
 ****************************************************************** */


/**
 * Thread reading the input files
 *
 * File reader inspired by the following forum topic :
 * https://stackoverflow.com/questions/11168519/fscanf-or-fgets-reading-a-file-line-after-line
 *
 * @param filename
 * @return result
 */
void *fileReader(void *filename) {
    printf("%s : %s\n", "Reading the following file", (char *) filename);

    FILE *file = fopen(filename, "r");
    if (file== NULL) {
        fprintf(stderr, "%s : %s\n", "Unable to find the following input file", (char *)filename);
        pthread_exit((void *)-1);
    }

    /** @var string : Fractal name to be read */
    char name[64];

    /** @var int : Image width */
    int width;

    /** @var int : Image height */
    int height;

    /** @var float : Real part of the complex number */
    float a;

    /** @var float : Imaginary part of the complex number */
    float b;

    /** @var int : Line counter */
    int lineNumber;

    int matched = fscanf(file, "%s %i %i %f %f\n", name, &width, &height, &a, &b);
    while (matched != EOF)
    {
        lineNumber++;
        if (matched == 5) {
            fractal_t * fractal = fractal_new(name, width, height, a, b);

            fractal_free(fractal);
        } else {
            printf("Line %d ignored\n", lineNumber);
        }
        matched = fscanf(file, "%s %i %i %f %f\n", name, &width, &height, &a, &b);
    }


    return NULL;
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
    int argIndex = 1;

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
        printf("%s %d\n", "Max amount of threads has been set to", maxthreads);
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
            int threadCreationResult = pthread_create(&fileReaderThreads[fileIndex], NULL, *fileReader, (void *) argv[argIndex]);

            if (threadCreationResult != 0) {
                fprintf(stderr, "%s : %s\n", "Error while creating the thread to read the following file", argv[argIndex]);
            }
        }

        argIndex++;
    }

    char outputFile = *argv[argIndex];

    if (printAll) {

    }

    return 0;
}

