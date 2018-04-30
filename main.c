#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <memory.h>
#include <pthread.h>
#include <semaphore.h>
#include "libfractal/fractal.h"
#include "stack/stack.h"

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



/* ******************************************************************
 *                          Buffer to compute
 ****************************************************************** */

/** @var node_t : First element of the "to compute" buffer */
node_t *toComputeBuffer;

/** @var pthread_mutex_t : mutex locker of the "to compute" buffer */
pthread_mutex_t toComputeBufferMutex;

/** @var sem_t : semaphore containing the amount of empty slots in the "to compute" buffer */
sem_t toComputeBufferEmpty;

/** @var sem_t : semaphore containing the amount of taken slots in the "to compute" buffer */
sem_t toComputeBufferFull;


/* ******************************************************************
 *                          Buffer computed
 ****************************************************************** */

/** @var node_t : First element of the "computed" buffer */
node_t *computedBuffer;

/** @var pthread_mutex_t : mutex locker of the "computed" buffer */
pthread_mutex_t computedBufferMutex;

/** @var sem_t : semaphore containing the amount of empty slots in the "computed" buffer */
sem_t computedBufferEmpty;

/** @var sem_t : semaphore containing the amount of taken slots in the "computed" buffer */
sem_t computedBufferFull;


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

            sem_wait(&toComputeBufferEmpty);
            pthread_mutex_lock(&toComputeBufferMutex);

            if (push(&toComputeBuffer, fractal) == EXIT_FAILURE) {
                fprintf(stderr, "%s\n", "Error while pushing into the \"to compute\" buffer");
            }

            pthread_mutex_unlock(&toComputeBufferMutex);
            sem_post(&toComputeBufferFull);

            printf("- Added %s : %f + %fi. Image size : %i x %i\n", name, a, b, width, height);
            fractal_free(fractal);
        }
        matched = fscanf(file, "%s %i %i %f %f\n", name, &width, &height, &a, &b);
    }


    printf("\n\n");
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    /** @var boolean : need to print all computed fractals */
    int printAll = false;

    /** @var int : max amount of threads */
    int maxthreads = 2;

    /** @var int : args counter */
    int argIndex = 1;

    printf(
            "%s\n%s\n%s\n\n",
            "===================================================================",
            "==                  Starting fractal computer                    ==",
            "==================================================================="
    );

    /*
     * At least 3 arguments must be given
     */
    if (argc < 3){
        fprintf(stderr, "%s\n","You must at least provide an input and an output file" );
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
        argIndex++;
    }
    printf("%s %d\n\n", "Max amount of threads has been set to", maxthreads);

    /*
     * Initializing mutex and semaphores concerning the "to compute" buffer
     */
    if (pthread_mutex_init(&toComputeBufferMutex, NULL) != EXIT_SUCCESS) {
        fprintf(stderr, "%s\n", "Error while initializing the \"to compute\" buffer mutex");
    }

    if (sem_init(&toComputeBufferEmpty, 0, (unsigned int) maxthreads * 2) != EXIT_SUCCESS) {
        fprintf(stderr, "%s\n", "Error while initializing the \"to compute\" buffer's empty semaphore");
    }

    if (sem_init(&toComputeBufferFull, 0, 0) != EXIT_SUCCESS) {
        fprintf(stderr, "%s\n", "Error while initializing the \"to compute\" buffer's full semaphore");
    }


    /** @var int : total amout of files */
    int totalFiles = argc - argIndex -1;

    /** @var pthread_t[] : Table containing the file reading threads */
    pthread_t fileReaderThreads[totalFiles];

    /** @var int : files counter */
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

            pthread_join(fileReaderThreads[fileIndex], NULL);
        }

        argIndex++;
    }

    char outputFile = *argv[argIndex];

    if (printAll && outputFile) {

    }

    return 0;
}

