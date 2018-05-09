#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <memory.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


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
 *     |      | Buffer with fractals to compute | toComputeBuffer   *
 *     |      '---------------------------------'                   *
 *     |            ||        ||         ||                         *
 *     |            ||        ||         ||     --computer          *
 *     |           \||/      \||/       \||/                        *
 *     |            \/        \/         \/                         *
 *     |      ,---------------------------------,                   *
 *     |      |  Buffer with computed fractals  | computedBuffer    *
 *     |      '---------------------------------'                   *
 *     |        |                                                   *
 *     | <------┘                                                   *
 *     |                                                            *
 *    ,---------------,                                             *
 *    | Print fractal |                                             *
 *    '---------------'                                             *
 *                                                                  *
 ****************************************************************** */

/** @var int : max amount of threads */
int maxthreads = 2;

/** @var boolean : need to print all computed fractals */
int printAll = false;

/* ******************************************************************
 *                          Buffer to compute
 ****************************************************************** */

/** @var node_t : First element of the "to compute" buffer */
node_t *toComputeBuffer = NULL;

/** @var pthread_mutex_t : mutex locker of the "to compute" buffer */
pthread_mutex_t toComputeBufferMutex;

/** @var sem_t : semaphore containing the amount of empty slots in the "to compute" buffer */
sem_t toComputeBufferEmpty;

/** @var sem_t : semaphore containing the amount of taken slots in the "to compute" buffer */
sem_t toComputeBufferFull;

int toComputeAmount;

int toReadAmount;

pthread_mutex_t toReadAmountMutex;


/* ******************************************************************
 *                          Buffer computed
 ****************************************************************** */

/** @var node_t : First element of the "computed" buffer */
node_t *computedBuffer;

/** @var pthread_mutex_t : mutex locker of the "computed" buffer */
pthread_mutex_t computedBufferMutex;



#define STDIN_FILE "user_input.txt"
/**
 * function for reading on standard input
 */
void readSTDIN(){
    FILE* userInput =  fopen(STDIN_FILE, "w");
    bool read = true;
    char buffer[100];
    while(read){
        printf("%s\n", "Enter fractal formatted as follow: [name width height a b]" );
        fgets(buffer,sizeof(buffer), stdin);
        int r = fputs(buffer,userInput);
        if (r<0) {
            printf("%s %s\n","Failed to print fractal in temporary file", STDIN_FILE);
        }
        printf("%s","Do you want to enter another fractal? Y/N :");
        fgets(buffer, sizeof(buffer), stdin);
        if(buffer[0]=='N'||buffer[0]=='n'){
            read =false;
        }
    }
    fflush(userInput); //push everything to memory
    fclose(userInput); //close stream before accessing it
}


/**
 * Thread reading the input files
 *
 * File reader inspired by the following forum topics :
 * - (Pattern Matching) : https://stackoverflow.com/questions/11168519/fscanf-or-fgets-reading-a-file-line-after-line
 * - (Regular expression) : https://stackoverflow.com/questions/20108334/traverse-file-line-by-line-using-fscanf
 *
 * @param filename
 * @return result
 */
void *fileReader(void *filename) {
    printf("%s : %s\n", "Reading the following file", (char *) filename);

    if (*((char *)filename)=='-') {
      readSTDIN();
      char stdin_file[] = STDIN_FILE;   //create string with filename of preprocessor
      filename=(void *)&stdin_file;    //assign pointer to created string in stack-memory
    }
    sleep(1);

    FILE *file = fopen(filename, "r");
    if (file== NULL) {
        fprintf(stderr, "%s : %s\n", "Unable to find the following input file", (char *)filename);
        pthread_exit((void *)-1);
    }

    /** @var string : Line to be read */
    char line[128];

    /** @var string : Fractal name to be read , size + 1 */
    char name[65];

    /** @var int : Image width */
    int width;

    /** @var int : Image height */
    int height;

    /** @var float : Real part of the complex number */
    float a;

    /** @var float : Imaginary part of the complex number */
    float b;

    int matched = fscanf(file, "%[^\n]\n", line);

    while (matched != EOF)
    {
        if (matched == 1 && line[0] != '#') {
            sscanf(line, "%s %i %i %f %f", name, &width, &height, &a, &b); // NOLINT

            fractal_t * fractal = fractal_new(name, width, height, a, b);

            sem_wait(&toComputeBufferEmpty);
            pthread_mutex_lock(&toComputeBufferMutex);

            if (pushInBuffer(&toComputeBuffer, fractal) == EXIT_FAILURE) {
                fprintf(stderr, "%s\n", "Error while pushing into the \"to compute\" buffer");
            }

            toComputeAmount++;


            pthread_mutex_unlock(&toComputeBufferMutex);
            sem_post(&toComputeBufferFull);
            printf("Added the fractal \"%s\" (%ix%i) %f + %fi to the Buffer\n", name, width, height, a, b);

        }
        matched = fscanf(file, "%[^\n]\n", line);
    }

    pthread_mutex_lock(&toReadAmountMutex);
    toReadAmount--;
    pthread_mutex_unlock(&toReadAmountMutex);

    pthread_exit(NULL);
}

void *computer() {

    while (true) {
        sem_wait(&toComputeBufferFull);
        pthread_mutex_lock(&toComputeBufferMutex);

        fractal_t *poppedFractal = popFromBuffer(&toComputeBuffer);
        //printf("> Computing fractal : %s\n\n", fractal_get_name(poppedFractal));


        pthread_mutex_unlock(&toComputeBufferMutex);
        sem_post(&toComputeBufferEmpty);

        uint64_t valuesSum = 0;

        for (int x = 0; x < fractal_get_width(poppedFractal); x++) {
            for (int y = 0; y < fractal_get_height(poppedFractal); y++) {
                valuesSum += fractal_compute_value(poppedFractal, x, y);
            }
        }

        float average = (float)(valuesSum) / (fractal_get_width(poppedFractal) * fractal_get_height(poppedFractal));

        poppedFractal->average = average;

        //printf("> Finished fractal : %s (average = %f)\n\n", fractal_get_name(poppedFractal), average);

        if (printAll) {
            write_bitmap_sdl(poppedFractal, strcat((char *)fractal_get_name(poppedFractal),".bmp"));
        }

            pthread_mutex_lock(&computedBufferMutex);

            if (computedBuffer == NULL) {
                pushInBuffer(&computedBuffer, poppedFractal);
            } else {
                float previousAverage = computedBuffer->fractal->average;
                if (average > previousAverage) {
                    flushBuffer(computedBuffer);
                }

                if (average >= previousAverage) {
                    pushInBuffer(&computedBuffer, poppedFractal);
                } else {
                    fractal_free(poppedFractal);
                }
            }

            pthread_mutex_unlock(&computedBufferMutex);


        pthread_mutex_lock(&toComputeBufferMutex);
        toComputeAmount--;
        pthread_mutex_unlock(&toComputeBufferMutex);
    }
    //pthread_exit(NULL); Willingly removed as it is never reached
}

void makeMutexSem() {
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

    /*
     * Initializing mutex concerning the "computed" buffer
     */
    if (pthread_mutex_init(&computedBufferMutex, NULL) != EXIT_SUCCESS) {
        fprintf(stderr, "%s\n", "Error while initializing the \"computed\" buffer mutex");
    }

    /*
     * Initializing mutex concerning the "computed" buffer
     */
    if (pthread_mutex_init(&toReadAmountMutex, NULL) != EXIT_SUCCESS) {
        fprintf(stderr, "%s\n", "Error while initializing the toReadAmount mutex");
    }

}

int main(int argc, char *argv[])
{

    /** @var int : args counter */
    int argIndex = 1;

    printf(
            "\n%s\n%s\n%s\n\n",
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

    /* TODO : change options reading system */

    /*
     * Is the -d arg given
     */
    if (strcmp(argv[argIndex], "-d") == 0) {
        printAll = true;
        //printf("%s\n", "All fractals will be drawn");
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

    makeMutexSem();

    /** @var int : total amout of files */
    int totalFiles = argc - argIndex -1;

    toReadAmount = totalFiles;

    /** @var pthread_t[] : Table containing the file reading threads */
    pthread_t fileReaderThreads[totalFiles];

    printf("Starting fileReaders :\n\n");

    /*
     * Running through all args containing file inputs
     */
    for (int fileIndex = 0; fileIndex < totalFiles; fileIndex++) {

        /* Initializing file reading threads */
        int threadCreationResult = pthread_create(&fileReaderThreads[fileIndex], NULL, &fileReader, (void *) argv[argIndex]);

        if (threadCreationResult != 0) {
            fprintf(stderr, "%s : %s\n", "Error while creating the thread to read the following file", argv[argIndex]);
        }

        argIndex++;
    }


    printf("\nStarting computers :\n\n");

    pthread_t computerThreads[maxthreads];

    /*
     * Starting the computing threads
     */
    for (int i = 0; i < maxthreads; i++) {
        pthread_create(&computerThreads[i], NULL, &computer, NULL);
    }

    /*
     * Joining the file reading threads
     */
    for (int i = 0; i < totalFiles; i++) {
        pthread_join(fileReaderThreads[i], NULL);
    }

    /*
     * This loop is used to make sure the computers have computed every fractals.
     * We use this one in order to be able to end the thread even if the program is waiting inside the infinite
     * loop at a sem_wait() point.
     */
    while (toComputeAmount > 0 || toReadAmount > 0);

    /*
     * Starting the computing threads
     */
    for (int i = 0; i < maxthreads; i++) {
        pthread_cancel(computerThreads[i]);
    }

    /*
    * remove temporary @STDIN_FILE
    */
    remove(STDIN_FILE);

    fractal_t *poppedFractal = popFromBuffer(&computedBuffer);
    printf("Printing fractal %s (average = %f) into the following file : %s\n", fractal_get_name(poppedFractal), poppedFractal->average, argv[argIndex]);
    write_bitmap_sdl(poppedFractal, argv[argIndex]);
    fractal_free(poppedFractal);
    flushBuffer(computedBuffer);

    return 0;
}
