#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define _GNU_SOURCE
#define SIZE_T_MAX ((size_t)(~0UL))

typedef struct int_array
{
    int *array;
    size_t length;
    size_t capacity;
} int_array;

void int_array_init(int_array *array)
{
    array->array = NULL;
    array->length = 0;
    array->capacity = 0;
}

void int_array_free(int_array *array)
{
    free(array->array);
    array->array = NULL;
    array->length = 0;
    array->capacity = 0;
}

void int_array_push_back(int_array *array, int value)
{
    if(array->length == array->capacity)
    {
        // Not enough space, reallocate.  Also, watch out for overflow.
        //int new_capacity = array->capacity * 2;
        int new_capacity = array->capacity + 1;
        if(new_capacity > array->capacity && new_capacity < SIZE_T_MAX / sizeof(int))
        {
            int *new_array = realloc(array->array, new_capacity * sizeof(int));
            if(new_array != NULL)
            {
               array->array = new_array;
               array->capacity = new_capacity;
            }
            else
                ; // Handle out-of-memory
        }
        else
            ; // Handle overflow error
    }

    // Now that we have space, add the value to the array
    array->array[array->length] = value;
    array->length++;
}


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