/*******************************************************************************
Filename: main.c
Name: Christopher Myers
Date: 12/10/2024
Course: CPE 2600-121
gcc -o main.out main.c dft.c -g -lm
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "row_filter_struct.h"
#include "dft.h"

//void generateData(double complex *x_in, int x_size);

void * row_filter(void *arg);

int main(int argc, char *argv[]) {

    FILE *fileIn;
    FILE *fileOut;
    char line[500];
    int width, height;
    char *filenameIn;
    int NUM_THREADS = 4;

    // Verify the user has input a filename
    if(getopt(argc, argv, "f:") != -1)
    {
        filenameIn = optarg;
    } else {
        perror("Please use the '-f' tag and a filename to process a .pgm");
        exit(1);
    }

    // Open input and output file
    fileIn = fopen(filenameIn, "rb");
    fileOut = fopen("myFileOut.pgm", "wb");
    if(fileIn == NULL || fileOut == NULL) {
        fputs("Error opening file\n", stderr);
        exit(1);
    }

    // Get the user input thread count, if specified
    if(getopt(argc, argv, "t:") != -1)
    {
        NUM_THREADS = strtol(optarg, NULL, 10);
        if(NUM_THREADS > 32 || NUM_THREADS < 1) {
            printf("The thread count must be within 1 to 32 inclusive\n");
            printf("Defaulting to 32 threads\n");
            NUM_THREADS = 32;
        }
    } else {
        printf("Use '-t' to specify number of threads\nDefaulting to %d threads\n", NUM_THREADS);
    }

    // Read through header strings
    for(int i = 0; i < 4; i++) {
        fgets(line, 500, fileIn);
        if(i == 1) {
            fprintf(fileOut, "#Edited by Christopher Myers\n");
            printf("#Edited by Christopher Myers\n");
        } else if(i == 2) { // Read in width and height data
            fprintf(fileOut, "%s", line);
            printf("%s", line);
            int newlineIndex = strcspn(line, "\n");
            if(newlineIndex != strlen(line)) {
                line[newlineIndex] = '\0';
            }
            char *widthPtr = strtok(line, " ");
            char *heightPtr = strtok(NULL, " ");
            if(widthPtr != NULL)
            {
                width = strtol(widthPtr, NULL, 10);
            } else
            {
                perror("failed to tokenize width");
                exit(1);
            }
            if(widthPtr != NULL)
            {
                height = strtol(heightPtr, NULL, 10);
            } else
            {
                perror("failed to tokenize heigth");
                exit(1);
            }
        } else {
            fprintf(fileOut, "%s", line);
            printf("%s", line);
        }
    }

    unsigned char *imgArray = malloc(sizeof(unsigned char) * width * height);
    for(int r = 0; r < height; r++)
    {
        // Read in row data
        for(int c = 0; c < width; c++) {
            *(imgArray + r*width + c) = (unsigned char)fgetc(fileIn);
        }
    }
    fclose(fileIn);

    // Produce output with multithreading
    printf("NUM_THREADS = %d\n", NUM_THREADS);
    unsigned char *imgArrayOut = malloc(sizeof(unsigned char) * width * height);
    pthread_t threads[NUM_THREADS];
    for(int chunkIndex = 0; chunkIndex < NUM_THREADS; chunkIndex++)
    {
        // Prepare data into thread
        row_filter_struct *row_filter_data_in = malloc(sizeof(row_filter_struct));
        row_filter_data_in->width = width;
        row_filter_data_in->height = height;
        row_filter_data_in->chunkIndex = chunkIndex;
        row_filter_data_in->NUM_THREADS = NUM_THREADS;
        row_filter_data_in->imgArray = imgArray;
        row_filter_data_in->imgArrayOut = imgArrayOut;
        // Create thread
        if(pthread_create(&threads[chunkIndex], NULL, row_filter, row_filter_data_in) != 0) {
            perror("pthread_create failed");
            free(row_filter_data_in);
            exit(1);
        }
    }
    
    for(int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    for(int r = 0; r < height; r++)
    {
        for(int c = 0; c < width; c++)
        {
            fputc(*(imgArrayOut + r*width + c), fileOut);
        }
    }

    // for(int n = 0; n < x_size; n++)
    // {
    //     // printf("x_in[%d] = %f + I*%f : x_out[%d] = %f + I*%f : x_n[%d] = %f + I*%f\n", n, creal(x_in[n]), cimag(x_in[n]), n, creal(x_out[n]), cimag(x_out[n]), n, creal(x_n[n]), cimag(x_n[n]));
    //     // printf("%f, %f\n", (float)n/x_size*7000, creal(x_out[n]));
    // }

    free(imgArray);
    free(imgArrayOut);
    fclose(fileOut);
    return 0;
}

void * row_filter(void *arg) {

    row_filter_struct row_filter_data_in = *((row_filter_struct *)arg);

    int width =         row_filter_data_in.width;
    int height =        row_filter_data_in.height;
    int chunkIndex =    row_filter_data_in.chunkIndex;
    int NUM_THREADS =   row_filter_data_in.NUM_THREADS;
    unsigned char *imgArray = row_filter_data_in.imgArray;
    unsigned char *imgArrayOut = row_filter_data_in.imgArrayOut;
    free(arg);

    // Calculate chunk of image for thread to calculate
    double startRowDouble = chunkIndex*height/NUM_THREADS;
    double endRowDouble = (chunkIndex + 1)*height/NUM_THREADS;
    int    startRow;
    int    endRow;
    if(startRowDouble < 0) {
        startRow = (int)(startRowDouble - 0.5);
    } else {
        startRow = (int)(startRowDouble + 0.5);
    }
    if(endRowDouble < 0) {
        endRow = (int)(endRowDouble - 0.5);
    } else {
        endRow = (int)(endRowDouble + 0.5);
    }
    double complex *row_k = malloc(sizeof(double complex) * width);
    double complex *row_n = malloc(sizeof(double complex) * width);

    for(int r = startRow; r < endRow; r++) {

        // Read in row data
        for(int c = 0; c < width; c++) {
            row_n[c] = *(imgArray + r*width + c);
        }

        // Perform dft on row
        dft(row_n, width, row_k);

        // filter row using high pass filter
        for(int k = 0; k < width; k++)
        {   
            if(k < width*20/100.0)
            {
                row_k[k] = 0;
            }
        }

        // Perform inverse dft on row
        idft(row_k, width, row_n);

        // Write filtered row data to output image array
        for(int c = 0; c < width; c++)
        {
            //printf("%d\n", (unsigned char)creal(row_n[n]));
            *(imgArrayOut + r*width + c) = (unsigned char)creal(row_n[c]);
        }
    }

    free(row_k);
    free(row_n);
    return NULL;
}

// void generateData(double complex *x_in, int x_size) {
//     double sampleRate = 2*M_PI*7000;
//     for(int n = 0; n < x_size; n++)
//     {
//         double t = n/sampleRate;
//         x_in[n] = cos(2*M_PI*3000*t) + cos(2*M_PI*650*t) + 2*cos(2*M_PI*67*t) + cos(2*M_PI*3100*t);
//     }
// }