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

struct Params
{
    int *start;
    size_t len;
    int depth;
};

// only used for synchronizing stdout from overlap.
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

// forward declare our thread proc
void *merge_sort_thread(void *pv);


// a simple merge algorithm. there are *several* more efficient ways
//  of doing this, but the purpose of this exercise is to establish
//  merge-threading, so we stick with simple for now.
void merge(int *start, int *mid, int *end)
{
    int *res = malloc((end - start)*sizeof(*res));
    int *lhs = start, *rhs = mid, *dst = res;

    while (lhs != mid && rhs != end)
        *dst++ = (*lhs < *rhs) ? *lhs++ : *rhs++;

    while (lhs != mid)
        *dst++ = *lhs++;

    // copy results
    memcpy(start, res, (rhs - start) * sizeof *res);
    free(res);
}

// our multi-threaded entry point.
void merge_sort_mt(int *start, size_t len, int depth)
{
    if (len < 2)
        return;

    if (depth <= 0 || len < 4)
    {
        merge_sort_mt(start, len/2, 0);
        merge_sort_mt(start+len/2, len-len/2, 0);
    }
    else
    {
        struct Params params = { start, len/2, depth/2 };
        pthread_t thrd;

        pthread_mutex_lock(&mtx);
        printf("Starting subthread...\n");
        pthread_mutex_unlock(&mtx);

        // create our thread
        pthread_create(&thrd, NULL, merge_sort_thread, &params);

        // recurse into our top-end parition
        merge_sort_mt(start+len/2, len-len/2, depth/2);

        // join on the launched thread
        pthread_join(thrd, NULL);

        pthread_mutex_lock(&mtx);
        printf("Finished subthread.\n");
        pthread_mutex_unlock(&mtx);
    }

    // merge the partitions.
    merge(start, start+len/2, start+len);
}

// our thread-proc that invokes merge_sort. this just passes the
//  given parameters off to our merge_sort algorithm
void *merge_sort_thread(void *pv)
{
    struct Params *params = pv;
    merge_sort_mt(params->start, params->len, params->depth);
    return pv;
}

// public-facing api
void merge_sort(int *start, size_t len)
{
    merge_sort_mt(start, len, 2); // 4 is a nice number, will use 7 threads.
}


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Error in the number of parameters. Halting\n");
        return -1;
    }

    //At this point, we are sure that we have exactly two parameters.

    FILE *finput, *foutput;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    //We make sure we can read the input file and write in the output file
    finput = fopen(argv[1], "r");
    if (finput == NULL)
        exit(EXIT_FAILURE);

    foutput = fopen(argv[2], "w");
    if (foutput == NULL)
        exit(EXIT_FAILURE);

    //We read the input file line by line.

    while ((read = getline(&line, &len, finput)) != -1)
    {
        /*printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);*/

        printf("Retrieved: %s", line);

        char *rest = line;
        char *token;

        //We allocate the read elements in a dynamic array (we reconstruct it each time we find a new element).

        int_array inputArray;
        int_array_init(&inputArray);

        while ((token = strtok_r(rest, " ", &rest))){
            //printf("%s\n", token);
            int number = atoi(token);
            int_array_push_back(&inputArray, number);
        }

        /*int i;
        for(i = 0; i < inputArray.length; i++)
            printf("a[%d] = %d\n", i, inputArray.array[i]);
        */
        
        //We convert the dynamic array into an array in order to start the merging by threads

        int *data = malloc(inputArray.length * sizeof(*data));

        int i,k;
        for(k=0;k<inputArray.length;k++){
            data[k]=inputArray.array[k];
        }

        merge_sort(data, inputArray.length);    // HERE WE MERGE BY THREADS

        //At this point, data contains the sorted array.

        for (i=0; i<inputArray.length; ++i)
        {
            printf("%d ", data[i]);
            
            fprintf(foutput, "%d", data[i]);

            if(i<inputArray.length-1)
                fprintf(foutput, " ");
        }
        printf("\n");
        fprintf(foutput, "\n");

        free(data);
    }

    //We close the files and release the memory

    fclose(finput);
    fclose(foutput);

    if (line)
        free(line);

    printf("---Last line---");

    return 0;
}