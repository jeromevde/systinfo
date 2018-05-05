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


/* ******************************************************************
 *                          Buffer computed
 ****************************************************************** */

/** @var node_t : First element of the "computed" buffer */
node_t *computedBuffer;

/** @var pthread_mutex_t : mutex locker of the "computed" buffer */
pthread_mutex_t computedBufferMutex;



#define STDIN_FILE "user_input.txt"
/*
* function for reading on standard input
*/
  void readSTDIN(){
    FILE* userInput =  fopen(STDIN_FILE, "w");
    bool read = true;
    char buffer[100];
    while(read){
      printf("%s\n", "Enter fractal formatted as follow: [name width height a b]" );
      fgets(buffer,sizeof(buffer),stdin);
      int r = fputs(buffer,userInput);
      if (r<0) {
        printf("%s\n","Failed to print fractal in temporary file user_input.txt" );
      }
      printf("%s","Do you want to enter another fractal? Y/N :");
      fgets(buffer, sizeof(buffer),stdin);
      printf("%s\n",buffer );
      if(buffer[0]=='N'||buffer[0]=='n'){
        read =false;
      }
    }
    fflush(userInput); //push everything to memory
    close(userInput); //close stream before accessing it
  }


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

    /** @var string : Fractal name to be read , size+1*/
    char name[65];

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


    /* TODO : if # in line, skip */
    while (matched != EOF)
    {
        lineNumber++;
        if (matched == 5 && name[0] != '#') {
            fractal_t * fractal = fractal_new(name, width, height, a, b);

            sem_wait(&toComputeBufferEmpty);
            pthread_mutex_lock(&toComputeBufferMutex);

            if (pushInBuffer(&toComputeBuffer, fractal) == EXIT_FAILURE) {
                fprintf(stderr, "%s\n", "Error while pushing into the \"to compute\" buffer");
            } else {
                printf(" - Pushed fractal %s->%s (%f + %fi. Image size : %i x %i) into the \"to compute\" buffer\n", (char *)filename, name, a, b, width, height);
            }

            pthread_mutex_unlock(&toComputeBufferMutex);
            sem_post(&toComputeBufferFull);


        }
        matched = fscanf(file, "%s %i %i %f %f\n", name, &width, &height, &a, &b);
    }

    pthread_exit(NULL);
}

void *computer() {
    while (true) {
        sem_wait(&toComputeBufferFull);
        pthread_mutex_lock(&toComputeBufferMutex);

        fractal_t *poppedFractal = popFromBuffer(&toComputeBuffer);
        printf("> Computing fractal : %s\n\n", fractal_get_name(poppedFractal));

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

        printf("> Finished fractal : %s (average = %f)\n\n", fractal_get_name(poppedFractal), average);

        if (printAll) {
            write_bitmap_sdl(poppedFractal, strcat((char *)fractal_get_name(poppedFractal),".bmp"));
            fractal_free(poppedFractal);
        } else {
            pthread_mutex_lock(&computedBufferMutex);
            if (computedBuffer == NULL) {
                pushInBuffer(&computedBuffer, poppedFractal);
            } else {
                float previousAverage = computedBuffer->fractal->average;
                if (average > previousAverage) {
                    flushBuffer(computedBuffer);
                }

                if (average >= previousAverage) {
                    pushInBuffer(computedBuffer, poppedFractal);
                }
            }

            pthread_mutex_unlock(&computedBufferMutex);
        }
    }
    pthread_exit(NULL);
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
     * Initializing mutex and semaphores concerning the "computed" buffer
     */
    if (pthread_mutex_init(&computedBufferMutex, NULL) != EXIT_SUCCESS) {
        fprintf(stderr, "%s\n", "Error while initializing the \"computed\" buffer mutex");
    }
}

int main(int argc, char *argv[])
{

    /* TODO : Wait for everyone to be finished */

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

    makeMutexSem();

    /** @var int : total amout of files */
    int totalFiles = argc - argIndex -1;

    /** @var pthread_t[] : Table containing the file reading threads */
    pthread_t fileReaderThreads[totalFiles];

    printf("Starting fileReaders :\n\n");

    /*
     * Running through all args containing file inputs
     */
    for (int fileIndex = 0; fileIndex < totalFiles; fileIndex++) {
        /* TODO : Only one stdin */

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
     * Joining the computing threads
     */
    for(int i = 0; i<maxthreads; i++) {
        pthread_join(computerThreads[i], NULL);
    }

    char outputFile = *argv[argIndex];

    if (printAll && outputFile) {

    }

    /*
     * How do we know if it's finished ?
     * - stdin must be ok
     * - all fractals from inputs + stdin must be done
     */

    /* TODO : FREE */

    return 0;
}
