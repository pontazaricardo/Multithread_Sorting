#include<stdlib.h>
#include<stdio.h>

#define _GNU_SOURCE

int main(int argc, char* argv[])
{
	if(argc!=3){
		printf("Error in the number of parameters. Halting\n");
		return -1;
	}

	//At this point, we have the two parameters:

	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(argv[1], "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);
    }

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);



    return 0;
}