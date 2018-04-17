#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define _GNU_SOURCE
#define SIZE_T_MAX ((size_t)(~0UL))



int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Error in the number of parameters. Halting\n");
        return -1;
    }

    //At this point, we have the two parameters:

    FILE *finput, *foutput;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    finput = fopen(argv[1], "r");
    if (finput == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, finput)) != -1)
    {
        printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);

        char *rest = line;
        char *token;

        int_array inputArray;
        int_array_init(&inputArray);

        while ((token = strtok_r(rest, " ", &rest))){
            printf("%s\n", token);
            int number = atoi(token);
            int_array_push_back(&inputArray, number);
        }

        int i;
        for(i = 0; i < inputArray.length; i++)
            printf("a[%d] = %d\n", i, inputArray.array[i]);

        //static const unsigned int N = a.length;
        int *data = malloc(inputArray.length * sizeof(*data));

        int k;
        for(k=0;k<inputArray.length;k++){
            data[k]=inputArray.array[k];
        }

        merge_sort(data, inputArray.length);
        for (i=0; i<inputArray.length; ++i)
        {
            printf("%4d ", data[i]);
            if ((i+1)%8 == 0)
                printf("\n");
        }
        printf("\n");

        free(data);
    }

    fclose(finput);
    if (line)
        free(line);
    //exit(EXIT_SUCCESS);

    printf("---Last line---");

    return 0;
}