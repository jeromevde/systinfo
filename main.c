#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <memory.h>
#include "libfractal/fractal.h"




int main(int argc, char *argv[])
{
    int printAll = false; //si on a l'argument -d
    int maxthread = 2;  //nombre de thread max par defaut
    int argIndex = 1; // dummy variable

    if (argc < 3){
      printf("%s\n","Veuillez preciser un fichier d'entree et de sortie" );
      exit(EXIT_FAILURE);
    }

    if (strcmp( argv[argIndex], "-d") == 0) {
        printAll = true;
        printf("%s\n", "printing is true");
        argIndex++;
    }

    if (strcmp( argv[argIndex], "--maxthreads") == 0) {
        argIndex++;
        maxthread =*(argv[argIndex]);
        argIndex++;
    }

    char* outputFiles = *argv;
    while (argIndex < argc - 1) {
        /* TODO */
        argIndex++;
    }

    char ouputFile = *argv[argIndex];

    return 0;
}
